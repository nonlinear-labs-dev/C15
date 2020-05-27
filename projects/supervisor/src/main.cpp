/******************************************************************************/
/** @version	V5.2 2020-05 KSTR
    @date		
    @author		KSTR
    @brief    	        systick 1ms

	!!! Set fuse LOW.SUT_CKSEL to "INTRCOSC_2MHZ_6CK_64MS" !!!
	!!! Set fuse LOW.BODLEVEL to "2.7V" !!!
	!!! Set fuse LOW.BODON to "2ON" !!!
	
	

    Changes :
	2019-09-11 KSTR : Add infinite time power fail recovery via EEPROM
	2019-10-22 KSTR : Add BBB signal communication to allow mute/unmute via BBB
	2019-10-25 KSTR : Adapt default muting and ePC/BBB monitoring
    
*****/

#if defined __PWR_CYCLING
#if (!defined __PWR_CYCLING_HARDCODED && !defined __PWR_CYCLING_HIDDEN_FEATURE) || (defined __PWR_CYCLING_HARDCODED && defined __PWR_CYCLING_HIDDEN_FEATURE)
#error "You must supply either __PWR_CYCLING_HARDCODED or __PWR_CYCLING_HIDDEN_FEATURE when using __PWR_CYCLING!"
#else
#warning "Power-Cycling enabled. For cycle-time see definition of POWER_CYCLING_TIME in globals.h"
#endif
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "globals.h"
#include "hardware.h"
#include "pin_manipulation.h"
#include "bit_manipulation.h"
#include "coos.h"
#include "led.h"
#include "pwr_switch.h"
#include "pwr_monitor.h"
#include "BBB.h"
#include "ePC.h"
#include "audio.h"
#include "ESPI.h"
#include "LPC.h"
#include "comm.h"

#define REBOOT_TIME (POWER_CYCLING_TIME / SM_TIMESLICE);

static uint32_t reboot_timer = 0;
#if defined __PWR_CYCLING && defined __PWR_CYCLING_HARDCODED
#warning "hardcoded Power-Cycling, normal device operation will be impossible!"
#define powerCycling (1)
#else
static uint8_t powerCycling = 0;
#endif

// state machine handler variable
struct StateMachine_t
{
  void (*pState)(void);  // points to current state procedure
  uint16_t pause;        // wait timer (down-counting to zero if set)
  uint8_t  step;         // current step in state procedure step-chains
  //	uint8_t		armed_for_autorestart;	// is set during the periods a power-fail cold restart make sense
};
StateMachine_t sm;

// state machine handling
void SM_Init(void);
void SM_TransitionTo(void (*pNewState)(void));
void SM_Pause(uint16_t ms);
// state procedures
void SM_WaitForPwrGood(void);
void SM_Standby(void);
void SM_Booting(void);
void SM_Running(void);
void SM_Shutdown(void);
void SM_HotReboot(void);

void SM_TransitionTo(void (*pNewState)(void))
{
  sm.pState = pNewState;
  sm.step   = 0;
  sm.pause  = 0;

  if (pNewState == SM_Standby)
  {
    config.SetRunState(RS_STANDBY);
    BitClr(config.status, STAT_SYSTEM_LOCKED);
  };
  if (pNewState == SM_Booting || pNewState == SM_HotReboot)
  {
    config.SetRunState(RS_BOOTING);
  };
  if (pNewState == SM_Running)
  {
    config.SetRunState(RS_RUNNING);
  };
  if (pNewState == SM_Shutdown)
  {
    config.SetRunState(RS_SHUTDOWN);
    BitClr(config.status, STAT_SYSTEM_LOCKED);
  };

  SetRunningState((pNewState == SM_Running));
  if ((pNewState == SM_Booting)
      || (pNewState == SM_Running))  // Set auto-reboot condition only when switch to Booting or Running
    SetActiveState(1);
  else if ((pNewState == SM_Shutdown))  // Reset auto-reboot condition when switch to Shutdown
    SetActiveState(0);
}

void SM_Init(void)
{
  if (config.hot_reset)  // hot reset means we must re-enter normal running state (if we came from there)
    SM_TransitionTo(SM_HotReboot);
  else
    SM_TransitionTo(SM_WaitForPwrGood);
};

void SM_Pause(uint16_t ms)
{
  sm.pause = ms / SM_TIMESLICE;
}

static void CheckPowerCyclingRequest(void)
{
#if defined __PWR_CYCLING && defined __PWR_CYCLING_HIDDEN_FEATURE
#warning "Power-Cycling enabled as hidden feature of the power button/switch! For required number of presses see definition of POWER_CYCLING_NO_OF_TGLS in globals.h"
  if (PwrSwitch.getStateCntr() > POWER_CYCLING_NO_OF_TGLS)
  {
    powerCycling = !powerCycling;
    Led.PowerCyclingOverride(powerCycling);
    reboot_timer = powerCycling * REBOOT_TIME;
    PwrSwitch.clrStateCntr();
  }
#endif
}

void SM_ProcessStates(void)
{
  CheckPowerCyclingRequest();
  if (sm.pause)  // wait first ?
    sm.pause--;
  else
  {
    if (sm.pState)     // catch potential NULL pointer, otherwise ...
      (*sm.pState)();  // ... call state
  }
  COMM_StartStatusWrite();
}

void SM_WaitForPwrGood(void)
{
  if (sm.step == 0)
  {
    sm.step = 1;
    Led.Blink_VeryFast();
  }
  if (PwrMonitor.PowerIsGood())
  {
    Led.Show_Hardware_and_Firmware_ID();  //  then don't display IDs after a warm start
    SM_TransitionTo(SM_Standby);
  }
}

void SM_Standby(void)
{
  switch (sm.step)
  {
    case 0:
      Led.Blink_Standby();
      sm.step++;
      return;
    case 1:
      if (config.power_switch_is_latching || (PwrSwitch.GetState() == -1))
      // ^ wait for released momentary button first, if applicable
      {
        PwrSwitch.Arm();
        sm.step++;
      }
      return;
    case 2:
      if (PwrSwitch.On_Event() || config.auto_reboot || powerCycling)
      {
        if (config.auto_reboot)
          sm.step = 3;  // start wait time
        else
          SM_TransitionTo(SM_Booting);
        config.auto_reboot = 0;
        PwrSwitch.clrStateCntr();
      }
      return;
    default:
      sm.step++;
      if (sm.step >= 1000 / SM_TIMESLICE)  // wait a second before auto-restart, to allow for bouncing power plug etc.
        SM_TransitionTo(SM_Booting);
      return;
  }
}

void SM_Booting(void)
{
  switch (sm.step)
  {
    case 0:
      sm.step++;
      Led.Blink_Medium();
      Audio.Mute(1);
      reboot_timer = powerCycling * REBOOT_TIME;
      SM_Pause(200);
      return;
    case 1:
      sm.step++;
      ePC.Init();
      PwrMonitor.SystemOn();
      ESPI_StartOverride();
      SM_Pause(1000);
      return;
    case 2:
      sm.step++;
      ePC.SwitchOn();
      SM_Pause(1000);
      return;
    case 3:
      sm.step++;
      COMM_Init();
      LPC_Reset();
      LPC_Start_MonitorAudioEngineStatus(UNMUTE_TIMEOUT);
      SM_Pause(BOOTING_FINISHED);
      return;
    case 4:
      SM_TransitionTo(SM_Running);
      PwrSwitch.Arm();  // arm button again after boot period, if momentary type
      return;
  }
}

void SM_Running(void)
{
  uint8_t reboot = 0;
  if (reboot_timer)
  {
    if (!--reboot_timer)
      reboot = 1;
  }
  if ((PwrSwitch.Off_Event() || reboot) && !BitGet(config.status, STAT_SYSTEM_LOCKED))
    SM_TransitionTo(SM_Shutdown);
  else
  {
    switch (sm.step)
    {
      case 0:
        sm.step++;
        Led.On();
        BBB.ArmHeartbeat();
        ePC.StateChange();
        BBB.StateChange();  // clearing calls
        ePC.StateChange();
        BBB.StateChange();  // clearing calls
        break;
      case 1:
        sm.step++;
        // Audio.UnMute();
        break;
    }
  }

#if __CHECK_EPC__
  if (ePC.StateChange())
    Led.On_IndicateGlitch1();
#endif

#if __CHECK_BBB__
  if (BBB.StateChange() || BBB.HeartbeatTimedOut())
    Led.On_IndicateGlitch2();
#endif
}

void SM_Shutdown(void)
{
  if (sm.step == 0)
  {
    sm.step = 1;
    Led.Blink_Fast();
    LPC_Stop_MonitorAudioEngineStatus();
    BitClr(config.status, STAT_MUTING_OVERRIDE_ENABLE);
    Audio.Mute(1);
    ePC.SwitchOff();
    BBB.SwitchOff();
    SM_Pause(100);
  }
  else
  {
    if (ePC.IsOff() && BBB.IsOff())
    {
      COMM_DeInit();
      PwrMonitor.SystemOff();
      SM_TransitionTo(SM_Standby);
      SM_Pause(1000);  // wait a second to enter StandBy
    }
  }
}

void SM_HotReboot(void)
{
  COMM_Init();
  LPC_Start_MonitorAudioEngineStatus(0);
  SM_TransitionTo(SM_Running);
  PwrSwitch.Arm();  // arm button again after boot period, if momentary type
}

//----------------------------------------------------------------------
int main(void)
{
  cli();
  HardwareInit_1();  // primary hardware init that doesn't need IRQ (timer) functionality
  sei();             // start ticker etc, IRQ based stuff
  HardwareInit_2();  // secondary hardware init that *DOES* need IRQ (timer) functionality
  cli();             // prevent task scheduling updater to accumulate pending calls during below coos_add_task() calls

  PwrSwitch.Init();
  PwrMonitor.Init();
  BBB.Init();
  ePC.Init();
  Audio.Init();
  SM_Init();

  // all tasks are non-accumulative ==> negative timelice values
  // distribute tasks evenly with the TS_OFFSET multipliers
  coos_init();
  coos_add_task(SM_ProcessStates, TS_OFFSET * 0, -SM_TIMESLICE);  // main state machine
  coos_add_task(LED_Process, TS_OFFSET * 1, -LED_TIMESLICE);      // LED handler
  coos_add_task(PSW_Process, TS_OFFSET * 2, -PSW_TIMESLICE);      // powerswitch/-button handler
  coos_add_task(Audio_Process, TS_OFFSET * 3, -AUDIO_TIMESLICE);  // audio mute/unmute handler
  coos_add_task(BBB_Process, TS_OFFSET * 4, -BBB_TIMESLICE);      // BBB control handler
  coos_add_task(ePC_Process, TS_OFFSET * 5, -ePC_TIMESLICE);      // ePC control handler

  sei();     // allow task scheduling updater to work now
  while (1)  // repeat forever
  {
    // real time stuff
    if (sm.pState != SM_WaitForPwrGood && sm.pState != SM_Standby)
    {  // states in which communication with BBB is possibly up and running
      COMM_ProccessReadCommands();
      COMM_ProccessWriteStatus();
    }

    if (power_failed || PwrMonitor.PowerFail())
    {
      Audio.Mute(1);  // forced mute again, just in case the IRQ didn't catch it
      Wait(50);       // give relays time to react before finally cutting power (relevant for 7.x Hardware only)
      PwrMonitor.SystemOff();

      // if it is a true power cut:
      //		give external supply some time to drop further, to avoid a false "power-good" after reset from unloading the supply
      // if it is only a temporary drop below threshold:
      //		give external supply some time to recover to a more stable state
      Wait(2000);

      FullReset();  // if we are still alive, force uC reset and wait for power good again
    }

    // time-sliced stuff
    coos_dispatch();
  }
}

/******************************************************************************/
/** IRQ related
*******************************************************************************/

// timer 0 overflow interrupt
ISR(TIMER0_COMP_vect)
{
  coos_update();
  ticker++;
}

// analog comparator interrupt
ISR(ANA_COMP_vect)
{
  if (BitGet(ACSR, ACO))  // Comp Out set == falling edge has occured
  {
    Audio.Mute(1);     // forced mute
    power_failed = 1;  // prevents Audio.Unmute() and PwrMonitor.SystemOn()
  }
}

// END OF FILE
