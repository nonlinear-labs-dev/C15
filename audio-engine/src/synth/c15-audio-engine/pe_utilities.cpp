#include "pe_utilities.h"

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
