/******************************************************************************/
/** @file		nl_espi_dev
    @date		2014-12-12 dtz
    @brief    	Calling ESPI_DEV_Tasks_Init will schedule all espi devices.
    @example  	<TBD>
    @author		Daniel Tzschentke [2014-12-12]

    nl_cfg_espi

nl_espi_core
nl_espi_core_adc
nl_espi_core_io

nl_espi_dev_tasks
nl_espi_dev_attenuator		20 ms
nl_espi_dev_pitchbender	    20 ms
nl_espi_dev_aftertouch		20 ms
nl_espi_dev_pedals
	- ADC 8					20 ms
	- OUT 8					20 ms
	- IN  4					20 ms
		if(IN)	get_adc
nl_espi_dev_volpoti		20 ms
nl_espi_dev_ribbons			20 ms
*******************************************************************************/

#ifndef NL_ESPI_DEV_H_
#define NL_ESPI_DEV_H_

void ESPI_DEV_Init(void);

#endif
