#include "LoaderConfig.h"

#include <cxxabi.h>

bool LoaderConfig::isLoadable() const {
    return static_cast<bool>(loader);
}

bool LoaderConfig::hasChildren() const {
    return !children.empty();
}

bool LoaderConfig::hasDependencies() const {
    return !dependsOn.empty();
}

std::string LoaderConfig::demangle(std::string_view mangled) {
    const auto dmg = abi::__cxa_demangle(mangled.data(), nullptr, nullptr, nullptr);
    std::string name = dmg;
    std::free(dmg);
    return name;
}