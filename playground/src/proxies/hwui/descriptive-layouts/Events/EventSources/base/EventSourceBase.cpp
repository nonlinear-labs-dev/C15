#include "EventSourceBase.h"
namespace DescriptiveLayouts {

  sigc::connection EventSourceBase::connect(const EventSourceBase::Callback &cb) {
    cb(getLastValue());
    return m_outputSignal.connect(cb);
  }
}