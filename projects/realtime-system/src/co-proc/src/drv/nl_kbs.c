/******************************************************************************/
/**	@file		nl_kbs.c
	@brief    	Scans Keys of the Fatar Keyboard TP8S 61 Keys
	@date		2020-04-29 Klaus Strohhaecker : heavy optimizing
  	@author		Stephan Schmitt, Daniel Tzschentke, [2012-12-20]
  	Optimizations :
  	            The whole thing now moved to inside the M0 timer interrupt, rather
  	            that being invoked once per 125us.
  	            The M0 IRQ now fires every 1us and each time the scanner is called.
  	            The scanner is organized as a step chain to split up the load across
  	            16 IRQs. The wait time between IRQs is used as the settling time the
  	            decoder hardware needs after selected a new bank of switches to be
  	            scanned, 1 us is required and this defines the max IRQ call rate.
  	            By this, the time resolution for velocity calculation (by M4)
  	            can be increased down to 16us... but the IRQ routine must be completed
  	            in less than 1us, of course (so main M0 process gets any cpu at all,
  	            not suffering from repeated IRQ overruns). This applies for the
  	            normal state of scanner when no changes are found for the switches in
  	            question. When switches change, the IRQ may take longer than 1us, but as
  	            long as it stays below 2us this is no problem (no missed IRQ, no wrong
  	            or missing time-slices and ticker updates). Those single overruns are
  	            very seldom because of the high call rate, and normally no more than one
  	            single switch will change for the same reason.

  	            Literals are used everywhere, together with inlining the compliler can
  	            then track down every parameter/variable used in the function down
  	            to constants, including the nested calls in the CheckForStateChanges()
  	            functions.

	            We use a function pointer for the main process routine called by the
	            M0 interrupt. A set of 16 functions is used which update the function
	            pointer to form a step chain. This is more efficient that using
	            a step counter and switch() statement.

	            The cost of compares and loads/stores for the switch states is reduced
	            considerably by saving the bulk switch states per bank, not individually
	            per key number.

	            Functions are located in RAM for a little speed increase.

	            Finally, the access to the GPIOs was optimized for speed.
	            NOTE : the definitions made in "boards/emphase_v5.c" are
	            not used anymore  !!

	            For better EMC, a little bit of jitter is introduced by
	            constantly varying the time until the next timer interrupt triggers.
*******************************************************************************/
#include "drv/nl_kbs.h"
#include <stdint.h>
#include "ipc/emphase_ipc.h"
#include "drv/nl_cgu.h"

__attribute__((aligned(4))) static uint16_t keyOff[NUM_KEYS];   // flag array to store logical on/off state
__attribute__((aligned(4))) static uint32_t keyTime[NUM_KEYS];  // in M0 systicks, which are already left-shifted (for speed)

typedef union
{  // union defined for speed, by accessing "both" in single instruction
  uint16_t both;
  struct __attribute__((packed))
  {
    uint8_t lower;
    uint8_t upper;
  };
} SwitchUnion_T;

__attribute__((aligned(4))) static SwitchUnion_T currentSwitches;   // current state of switches
__attribute__((aligned(4))) static SwitchUnion_T changedSwitches;   // which switches have changed
__attribute__((aligned(4))) static SwitchUnion_T savedSwitches[8];  // saved switch state
__attribute__((aligned(4))) static uint8_t       tempLower;         // we need a temp because of the interleaving
__attribute__((aligned(4))) static uint32_t      ticker;            // incremented after one full scanning round

// direct GPIO access for max execution speed
// NOTE: Overrides definitions made in "boards/emphase_v5.c" !!!
#define pLINE_MASKREG (uint32_t *) (0x400F6080 + 0 * 4)  // masking setup for GPIO0[0..31]
#define pLINE         (uint8_t *) (0x400F6180 + 0 * 4)   // masked read/write for GPIO0[0..31]
#define pKEYS_MASKREG (uint32_t *) (0x400F6080 + 5 * 4)  // masking setup for GPIO5[0..31]
#define pKEYS         (uint8_t *) (0x400F6180 + 5 * 4)   // masked read/write for GPIO5[0..31]

// local prototypes, all local functions are hard-inlined for max speed
static __attribute__((always_inline)) inline void SetHwLine(uint32_t const line);
static __attribute__((always_inline)) inline void GetHwLowerSwitches(void);
static __attribute__((always_inline)) inline void GetHwUpperSwitches(void);
static __attribute__((always_inline)) inline void CheckForStateChanges1(uint32_t const baseKey);
static __attribute__((always_inline)) inline void CheckForStateChanges2(uint32_t const baseKey);
static __attribute__((always_inline)) inline void DetectNotes(uint32_t const key, uint16_t const mask);

static void Process01(void);
static void Process02(void);
static void Process03(void);
static void Process04(void);
static void Process05(void);
static void Process06(void);
static void Process07(void);
static void Process08(void);
static void Process09(void);
static void Process10(void);
static void Process11(void);
static void Process12(void);
static void Process13(void);
static void Process14(void);
static void Process15(void);
static void Process16(void);

__attribute__((aligned(4))) void (*KBS_Process)(void) = Process01;

/******************************************************************************/
/**	@brief  	General initialization of the input and output pins. (public)
*******************************************************************************/
void KBS_Init(void)
{
  *pLINE_MASKREG = 0xFFFFFFF0;  // set up masking for "line select" : bits 0..3
  *pKEYS_MASKREG = 0xFFFFFF00;  // set up masking for "keys" : bits 0..7

  uint16_t mask = 0x0101;
  for (int i = 0; i < 64; i++)
  {
    keyOff[i] = mask;
    mask <<= 1;
    if (mask == 0x0100)
      mask = 0x0101;
  }
  KBS_Process = Process01;
  SetHwLine(0);  // Initially select bank 0 lower switches, before main process starts stepping lines
}

/******************************************************************************/
/**	@brief  	general keyboard process function (step-chaining itself, for speed)
	@details	This must be called continuously with a time grid of no less than 1us.

	            Intended to be called from within a timer interrupt. During each
	            interrupt either upper or lower switches are read, and either the
	            first 4 lines or the second 4 lines of a bank are processed.

	            After each call of SetHwLine(x) at least 1us of settling time
	            is required for the hardware, and the execution of the whole
	            IRQ may also take ~1us, so don't overdo it with the call rate,
	            also to give the main M0 process some air to breathe.

	            SetHwLine(x) must be called early to give the hardware
	            the largest possible time to settle between interrupts.

	            Note that switch readout and  processing is interleaved,
	            that's why GetHwLowerSwitches() reads to a temp to avoid
	            clobbering lower switches for a bank before CheckForStateChanges2()
	            had a chance to process it.
*******************************************************************************/
static void Process01(void)
{
  ticker += (1 << IPC_KEYBUFFER_TIME_SHIFT);
  GetHwLowerSwitches();
  SetHwLine(1);               // prepare readout upper switches of bank 0
  CheckForStateChanges2(56);  // check (stage 2), bank 7
  KBS_Process = Process02;
}
static void Process02(void)
{
  GetHwUpperSwitches();
  SetHwLine(2);              // prepare readout lower switches of bank 1
  CheckForStateChanges1(0);  // check (stage 1), bank 0
  KBS_Process = Process03;
}
static void Process03(void)
{
  GetHwLowerSwitches();
  SetHwLine(3);              // prepare readout upper switches of bank 1
  CheckForStateChanges2(0);  // check (stage 2), bank 0
  KBS_Process = Process04;
}
static void Process04(void)
{
  GetHwUpperSwitches();
  SetHwLine(4);              // prepare readout lower switches of bank 2
  CheckForStateChanges1(8);  // check (stage 1), bank 1
  KBS_Process = Process05;
}
static void Process05(void)
{
  GetHwLowerSwitches();
  SetHwLine(5);              // prepare readout upper switches of bank 2
  CheckForStateChanges2(8);  // check (stage 2), bank 1
  KBS_Process = Process06;
}
static void Process06(void)
{
  GetHwUpperSwitches();
  SetHwLine(6);               // prepare readout lower switches of bank 3
  CheckForStateChanges1(16);  // check (stage 1), bank 2
  KBS_Process = Process07;
}
static void Process07(void)
{
  GetHwLowerSwitches();
  SetHwLine(7);               // prepare readout upper switches of bank 3
  CheckForStateChanges2(16);  // check (stage 2), bank 2
  KBS_Process = Process08;
}
static void Process08(void)
{
  GetHwUpperSwitches();
  SetHwLine(8);               // prepare readout lower switches of bank 4
  CheckForStateChanges1(24);  // check (stage 1), bank 3
  KBS_Process = Process09;
}
static void Process09(void)
{
  GetHwLowerSwitches();
  SetHwLine(9);               // prepare readout upper switches of bank 4
  CheckForStateChanges2(24);  // check (stage 2), bank 3
  KBS_Process = Process10;
}
static void Process10(void)
{
  GetHwUpperSwitches();
  SetHwLine(10);              // prepare readout lower switches of bank 5
  CheckForStateChanges1(32);  // check (stage 1), bank 4
  KBS_Process = Process11;
}
static void Process11(void)
{
  GetHwLowerSwitches();
  SetHwLine(11);              // prepare readout upper switches of bank 5
  CheckForStateChanges2(32);  // check (stage 2), bank 4
  KBS_Process = Process12;
}
static void Process12(void)
{
  GetHwUpperSwitches();
  SetHwLine(12);              // prepare readout lower switches of bank 6
  CheckForStateChanges1(40);  // check (stage 1), bank 5
  KBS_Process = Process13;
}
static void Process13(void)
{
  GetHwLowerSwitches();
  SetHwLine(13);              // prepare readout upper switches of bank 6
  CheckForStateChanges2(40);  // check (stage 2), bank 5
  KBS_Process = Process14;
}
static void Process14(void)
{
  GetHwUpperSwitches();
  SetHwLine(14);              // prepare readout lower switches of bank 7
  CheckForStateChanges1(48);  // check (stage 1), bank 6
  KBS_Process = Process15;
}
static void Process15(void)
{
  GetHwLowerSwitches();
  SetHwLine(15);              // prepare readout upper switches of bank 7
  CheckForStateChanges2(48);  // check (stage 2), bank 6
  KBS_Process = Process16;
}
static void Process16(void)
{
  GetHwUpperSwitches();
  SetHwLine(0);               // prepare readout lower switches of bank 0
  CheckForStateChanges1(56);  // check (stage 1), bank 7
  KBS_Process = Process01;
}

/******************************************************************************/
/**	@warning	The GetHw Functions are optimized. They will only work with
				HW-Pins starting at pin 0 on a port.
*******************************************************************************/
static __attribute__((always_inline)) inline void GetHwUpperSwitches(void)
{
  currentSwitches.upper = ~(*pKEYS);  // contacts are inverting
}

static __attribute__((always_inline)) inline void GetHwLowerSwitches(void)
{
  tempLower = ~(*pKEYS);  // contacts are inverting. Read to temp because of interleaving
}

/******************************************************************************/
/**	@brief  	Sends the number of a Line (0...15) binary coded to 4 outputs
				(private)
				This function can only be used with a 0-alligned port
	@details 	The four pins drive a 'binary to 1-of-n hardware decoder'.
				Each bank of keys in the diode matrix has two lines, the even line (N)
				for the lower switches and the odd line (N+1) for the upper switches.
	@param		line number - 0..15
*******************************************************************************/
static __attribute__((always_inline)) inline void SetHwLine(uint32_t const line)
{
  *pLINE = line;  // direct write via the masked gpio register, for speed.
}

/******************************************************************************/
/**	@brief  	Detect and apply changes of the state of the keys.
	@param		baseKey: base key of a bank of 8 keys
				For speed and load balancing, processing is split in two stages,
				stage 1 does switches 0..3, stage 2 does switches 4..7
*******************************************************************************/
static __attribute__((always_inline)) inline void CheckForStateChanges1(uint32_t const baseKey)
{
  currentSwitches.lower = tempLower;  // set lower switches from previously read in value
  changedSwitches.both  = (currentSwitches.both ^ savedSwitches[baseKey >> 3].both);
  if (!(changedSwitches.both & 0x0F0F))
    return;  // no pattern change in the lower 4 bits

  DetectNotes(baseKey + 0, 0x0101);
  DetectNotes(baseKey + 1, 0x0202);
  DetectNotes(baseKey + 2, 0x0404);
  DetectNotes(baseKey + 3, 0x0808);
}

static __attribute__((always_inline)) inline void CheckForStateChanges2(uint32_t const baseKey)
{
  savedSwitches[baseKey >> 3].both = currentSwitches.both;  // save switch state for next pass of this bank
  if (!(changedSwitches.both & 0xF0F0))
    return;  // no pattern change in the higher 4 bits

  DetectNotes(baseKey + 4, 0x1010);
  DetectNotes(baseKey + 5, 0x2020);
  DetectNotes(baseKey + 6, 0x4040);
  DetectNotes(baseKey + 7, 0x8080);
}

/******************************************************************************/
/**	@brief  	handles changes of the key state
	@param		key
				state (on:lower:upper)
				- 0 = 000: key off, fully released
				- 1 = 001: key off, between the two contact points
				- 3 = 011: key off, fully pressed -> will go on
				- 4 = 100: key on, fully released -> will go off
				- 5 = 101: key on, between the two contact points
				- 7 = 111: key on, fully pressed
*******************************************************************************/
static __attribute__((always_inline)) inline void DetectNotes(uint32_t const key, uint16_t const mask)
{
  if (!(changedSwitches.both & mask))
    return;

  if ((currentSwitches.both & mask) == keyOff[key])
  {  // either both switches on AND key off, or both switches off AND key on ==> key event (either on or off)
    Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(key | ((keyOff[key] ^= mask) ? 0 : IPC_KEYBUFFER_NOTEON) | (ticker - keyTime[key]));
    return;
  }

  // now upper contact is closed and lower contact must be open (or vice-versa, irrelevant)
  keyTime[key] = ticker;  // start time measuring
}

// EOF
