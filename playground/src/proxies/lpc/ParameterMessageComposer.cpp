#include "ParameterMessageComposer.h"
#include "parameters/Parameter.h"
#include "MessageParser.h"
#include "device-settings/DebugLevel.h"

ParameterMessageComposer::ParameterMessageComposer(const Parameter *p)
    : MessageComposer(MessageParser::PARAM)
{
  m_parameterID = p->getID();
  *this << m_parameterID;
  p->writeToLPC(*this);
}

ParameterMessageComposer::~ParameterMessageComposer()
{
}

bool ParameterMessageComposer::canReplace(const MessageComposer *other) const
{
  if(auto *otherParameterMessageComposer = dynamic_cast<const ParameterMessageComposer *>(other))
  {
    DebugLevel::info("can replace existing message with new!");
    return otherParameterMessageComposer->m_parameterID == m_parameterID;
  }

  DebugLevel::info("cannot replace existing message with new now");
  return false;
}
