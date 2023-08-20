#ifndef ASTROVENTURE_LOADERCONFIG_H
#define ASTROVENTURE_LOADERCONFIG_H

#include <string>
#include <functional>
#include <vector>

class Loadable;

class LoaderConfig {
public:
    bool isLoadable() const;
    bool hasChildren() const;
    bool hasDependencies() const;

    template<typename T>
    static LoaderConfig create(LoaderConfig config) {
        config.name = demangle(typeid(T).name());
        return std::move(config);
    }

private:
    static std::string demangle(std::string_view mangled);

public:
    std::string name {};
    std::function<void()> loader {};
    std::vector<Loadable*> children {};
    std::vector<Loadable*> dependsOn {};
    bool useContext {false};
};

#endif //ASTROVENTURE_LOADERCONFIG_H
