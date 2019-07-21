#include "pe_key_event.h"

/* proper init */
void env_event::init()
{
    /* iterate through member arrays in order to fill them with zeros (levels) or ones (times) */
    for(uint32_t v = 0; v < dsp_number_of_voices; v++)
    {
        m_levelFactor[v] = 0.f;
        for(uint32_t i = 0; i < sig_number_of_env_segments; i++)
        {
            m_timeFactor[v][i] = 1.f;
        }
    }
}
