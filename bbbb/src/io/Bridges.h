#pragma once

#include "bbbb.h"
#include "FromButtonsBridge.h"
#include "ToLPCBridge.h"

class Bridge;

class Bridges
{
  public:
    Bridges();
    virtual ~Bridges();

  private:
    using tBridge = std::unique_ptr<Bridge>;
    std::list<tBridge> m_bridges;

  public:
    template<typename T>
      T* getBridge()
      {
        for(auto& b : m_bridges)
        {
          if(auto t = dynamic_cast<T*>(b.get()))
          {
            return t;
          }
        }
        return nullptr;
      }

};

