#include "dimension/DBPerSTDimension.h"
#include "LinearBipolar1DbstScaleConverter.h"

LinearBipolar1DbstScaleConverter::LinearBipolar1DbstScaleConverter()
    : LinearScaleConverter(tTcdRange(-8000, 8000), tDisplayRange(-1, 1), DBPerSTDimension::get())
{
}

LinearBipolar1DbstScaleConverter::~LinearBipolar1DbstScaleConverter()
{
}
