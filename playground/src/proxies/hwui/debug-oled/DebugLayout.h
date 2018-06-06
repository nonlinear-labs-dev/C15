#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <tools/json.h>

class DebugLayout : public DFBLayout {
protected:
    using super = DFBLayout;
public:
    explicit DebugLayout(Glib::ustring e);
};
