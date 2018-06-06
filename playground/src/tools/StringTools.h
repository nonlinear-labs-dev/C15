#pragma once

namespace StringTools {
    inline std::vector<std::string> splitStringOnAnyDelimiter(const std::string& s, char delimiter)
    {
        std::vector<std::string> strings;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
          strings.push_back(token);
        }
        return strings;
    }

    inline std::vector<std::string> splitStringOnStringDelimiter(std::string s, std::string delimiter)
    {
      size_t pos = 0;
      std::vector<std::string> strings;
      std::string token;
      while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        if(token.length() > 0)
          strings.push_back(token);
        s.erase(0, pos + delimiter.length());
      }
      if(s.length() > 0)
        strings.push_back(s);

      return strings;
    }
};
