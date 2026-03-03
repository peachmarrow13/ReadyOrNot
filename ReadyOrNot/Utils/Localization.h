#pragma once
#include <string>
#include <map>

enum class Language {
    English,
    Chinese
};

namespace Localization
{
    Language GetSystemLanguage();
    void Initialize();
    
    // Global language state
    extern Language CurrentLanguage;
    
    // Dictionary management
    void AddString(const std::string& key, const std::string& en, const std::string& zh);
    const char* T(const std::string& key);
}
