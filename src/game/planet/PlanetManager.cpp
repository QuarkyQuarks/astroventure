#include "PlanetManager.h"

PlanetManager::PlanetManager(Object *parent)
    : Object(parent),
      m_orbitGenerator(new OrbitGenerator(this)),
      m_planetGenerator(new PlanetGenerator(this))
{
    setThreadSafety(true);
}

Planet* PlanetManager::get(int flags) {
    auto base = popBase();

    // clear existing planet, do not regenerate it
    if ((base != nullptr) && (flags & Flag::JustClear)) {
        clear(base, flags);
        return base;
    }

    auto planet = m_planetGenerator->generate(base);

    if (!(flags & Flag::DisableOrbit))
        planet->addChild(m_orbitGenerator->generate());

    return planet;
}

void PlanetManager::getAsync(Callback callback, int flags) {
    auto base = popBase();

    // clear existing planet, do not regenerate it
    if ((base != nullptr) && (flags & Flag::JustClear)) {
        clear(base, flags);
        callback(base);
        return;
    }

    m_planetGenerator->generateAsync([flags, this, callback = std::move(callback)](Planet *planet) {
        if (!(flags & Flag::DisableOrbit))
            planet->addChild(m_orbitGenerator->generate());
        callback(planet);
    }, base);
}

void PlanetManager::done(Planet *planet) {
    planet->setParent(this);
}

Planet* PlanetManager::popBase() {
    std::lock_guard locker(m_basePlanetMutex);

    auto base = findChild<Planet*>(FindOption::Direct);

    if (base != nullptr) {
        base->removeChildren<PlanetOrbit*>(Object::FindOption::Direct);
        base->setParent(nullptr);
    }

    return base;
}

void PlanetManager::clear(Planet *base, int flags) {
    base->restoreCrystals();

    if (!(flags & Flag::DisableOrbit)) {
        base->addChild(m_orbitGenerator->generate());
    }
}

LoaderConfig PlanetManager::resourceLoaderConfig() {
    return LoaderConfig::create<PlanetManager>({
        .loader = [] {
            /*
             * empty, needed in order to make PlanetManager
             * loadable to be able to use it as dependency
             */
        },
        .children = {m_planetGenerator, m_orbitGenerator},
        .dependsOn = {m_planetGenerator, m_orbitGenerator}
    });
}
