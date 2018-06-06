#pragma once
#include <functional>

class ConditionRegistry {
public:
    typedef std::function<bool()> tCondition;
    tCondition getLambda(std::string key);
    static ConditionRegistry& get();
private:
    ConditionRegistry();
    std::map<std::string, std::function<bool()>> m_theConditonMap;
};
