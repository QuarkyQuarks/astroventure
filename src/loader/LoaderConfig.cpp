#include "LoaderConfig.h"

#include <tulz/demangler.h>

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
    return tulz::demangler::demangle(mangled);
}