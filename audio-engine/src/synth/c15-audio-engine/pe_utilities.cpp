#include "pe_utilities.h"

/* single id list methods (reset list, add id to list) */

void id_list::reset()
{
    m_length = 0;
}

void id_list::add(const uint32_t _id)
{
    m_data[m_length] = _id;
    m_length++;
}

/* dual id list methods (reset list, add _id to specified sub-list) */

void dual_id_list::reset()
{
    m_data[0].reset();
    m_data[1].reset();
}

void dual_id_list::add(const uint32_t _listId, const uint32_t _id)
{
    m_data[_listId].add(_id);
}

/* polyDual id list methods (reset list, add _id to specified sub-list) */

void polyDual_id_list::reset()
{
    for(uint32_t i = 0; i < dsp_poly_types; i++)
    {
        m_data[i].reset();
    }
}

void polyDual_id_list::add(const uint32_t _polyId, const uint32_t _listId, const uint32_t _id)
{
    m_data[_polyId].add(_listId, _id);
}

/* poly id list methods (reset list, add _id to specified sub-list) */

void poly_id_list::reset()
{
    for(uint32_t i = 0; i < dsp_poly_types; i++)
    {
        m_data[i].reset();
    }
}

void poly_id_list::add(const uint32_t _polyId, const uint32_t _id)
{
    m_data[_polyId].add(_id);
}

/* env id list methods (reset list, add _id to list) */

void env_id_list::reset()
{
    m_length = 0;
}

void env_id_list::add(const uint32_t _id)
{
    m_data[m_length] = _id;
    m_length++;
}

/* dual env id list methods (reset list, add _id to specified sub-list) */

void dual_env_id_list::reset()
{
    m_data[0].reset();
    m_data[1].reset();
}

void dual_env_id_list::add(const uint32_t _listId, const uint32_t _id)
{
    m_data[_listId].add(_id);
}
