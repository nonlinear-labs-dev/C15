#include "DSL.h"
#include <proxies/hwui/descriptive-layouts/TemplateEnums.h>
#include <proxies/hwui/HWUIEnums.h>
#include <nltools/logging/Log.h>
#include "Parser.h"
#include "Algorithm.h"

#include <glib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <limits>
#include <variant>

namespace DescriptiveLayouts
{
  static Element parseFile(const std::string& fileName)
  {
    std::ifstream in(fileName);
    std::string content((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
    return Parser::parseString(content);
  }
}
