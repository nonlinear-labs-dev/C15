#pragma once

#include "playground.h"
#include <map>
#include <functional>

template <typename tStates, typename tEvents, tStates initalState, tStates invalidState> class StateMachine
{
 public:
  using tHandler = std::function<bool()>;

  StateMachine()
      : m_currentState(initalState)
  {
  }

  virtual ~StateMachine() = default;

  void registerHandler(tStates s, tHandler h)
  {
    m_handlers[s] = h;
  }

  void registerTraversal(tStates from, tEvents on, tStates to)
  {
    m_traversals[from][on] = to;
  }

  virtual bool traverse(tEvents event)
  {
    const auto &itTraversal = m_traversals.find(m_currentState);

    if(itTraversal != m_traversals.end())
    {
      const auto &actions = itTraversal->second;
      const auto &it = actions.find(event);

      if(it != actions.end())
      {
        return setState(it->second);
      }
    }

    return setState(invalidState);
  }

  virtual bool setState(tStates s)
  {
    m_currentState = s;

    auto handler = m_handlers.find(s);

    if(handler != m_handlers.end())
      return handler->second();

    return false;
  }

  tStates getState() const
  {
    return m_currentState;
  }

 private:
  tStates m_currentState;

  typedef std::map<tEvents, tStates> tActions;
  typedef std::map<tStates, tActions> tTraversals;
  typedef std::map<tStates, tHandler> tHandlers;

  tTraversals m_traversals;
  tHandlers m_handlers;
};
