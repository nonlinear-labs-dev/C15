#include "XmlClass.h"

XmlClass::XmlClass(const Glib::ustring &classes)
    : Attribute("class", classes)
{
}

XmlClass::~XmlClass()
{
}
