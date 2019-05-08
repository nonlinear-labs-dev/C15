#include "dimension/DBTDimension.h"
#include "Linear60DbtScaleConverter.h"

Linear60DbtScaleConverter::Linear60DbtScaleConverter()
    : LinearScaleConverter(tTcdRange(0, 12000), tDisplayRange(0, 60), DBTDimension::get())
{
}

Linear60DbtScaleConverter::~Linear60DbtScaleConverter()
{
}
