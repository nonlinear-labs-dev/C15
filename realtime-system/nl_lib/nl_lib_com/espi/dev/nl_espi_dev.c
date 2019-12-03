#include <stdint.h>

#include "sys/nl_coos.h"

#include "espi/dev/nl_espi_dev.h"
#include "espi/dev/nl_espi_dev_volpoti.h"
#include "espi/dev/nl_espi_dev_aftertouch.h"
#include "espi/dev/nl_espi_dev_pitchbender.h"
#include "espi/dev/nl_espi_dev_ribbons.h"
#include "espi/dev/nl_espi_dev_pedals.h"

void ESPI_DEV_Init(void)
{
  ESPI_DEV_VolPoti_Init();
  COOS_Task_Add(ESPI_DEV_VolPoti_EspiPull, 0, 500);

#if 0
	ESPI_DEV_Pedals_Init();
	COOS_Task_Add(ESPI_DEV_Pedals_Process, 25, 100);

	ESPI_DEV_Aftertouch_Init();
	COOS_Task_Add(ESPI_DEV_Aftertouch_Process, 5, 100);

	ESPI_DEV_Pitchbender_Init();
	COOS_Task_Add(ESPI_DEV_Pitchbender_Process,	10, 100);

	ESPI_DEV_Ribbons_Init();
	COOS_Task_Add(ESPI_DEV_Ribbons_Process, 20, 100);
	COOS_Task_Add(ESPI_DEV_Attenuator, 0, 0);
#endif
}
