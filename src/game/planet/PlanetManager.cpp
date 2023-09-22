#include "PlanetManager.h"

#include <format>

PlanetManager::PlanetManager(Object *parent)
    : Object(parent),
      m_orbitGenerator(new OrbitGenerator(this)),
      m_planetGenerator(new PlanetGenerator(this))
{
    setThreadSafety(true);
}

Planet* PlanetManager::get(int flags, const Properties &properties) {
    auto base = popBase();

    // clear existing planet, do not regenerate it
    if ((base != nullptr) && (flags & Flag::JustClear)) {
        applyFlags(base, flags, properties);
        return base;
    }

    auto planet = m_planetGenerator->generate(base);

    applyFlags(planet, flags, properties);

    return planet;
}

void PlanetManager::getAsync(Callback callback, int flags, const Properties &properties) {
    auto base = popBase();

    // clear existing planet, do not regenerate it
    if ((base != nullptr) && (flags & Flag::JustClear)) {
        applyFlags(base, flags, properties);
        callback(base);
        return;
    }

    m_planetGenerator->generateAsync([flags, properties, this, callback = std::move(callback)](Planet *planet) {
        applyFlags(planet, flags, properties);
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
        base->setPos(0, 0, 0);
    }

    return base;
}

void PlanetManager::applyFlags(Planet *planet, int flags, const Properties &properties) {
    if (!(flags & Flag::DisableOrbit)) {
        planet->addChild(m_orbitGenerator->generate());
    }

    if (flags & Flag::JustClear) {
        applyJustClear(planet);
    }

    if (flags & CalcPosition) {
        applyCalcPosition(planet, properties);
    }

    planet->setRoll(0);
    planet->rotate();
    planet->transform();
}

void PlanetManager::applyJustClear(Planet *planet) {
    planet->restoreCrystals();
}

void PlanetManager::applyCalcPosition(Planet *planet, const Properties &properties) {
    auto it = properties.find(Property::PositionDetails);

    if (it == properties.end())
        throw std::runtime_error("Flag::SetIndex is specified, but Property::Index is not");

    auto &value = it->second;

    assert(value.has_value());

    if (value.type() != typeid(PositionDetails)) {
        throw std::runtime_error(
                std::format("Invalid type: expected PlanetManager::PositionDetails, got '{}'", value.type().name()));
    }

    constexpr float planet_x = 0.125f; // horizontal distance between two neighboring planets
    constexpr float planet_y = 0.45f; // vertical distance between two neighboring planets
    constexpr float planet_z = 0.0f;

    // positions of planets:
    // 0: (-planet_x, -planet_y, planet_z)
    // 1: ( planet_x, 0.0f,      planet_z)
    // 2: (-planet_x, planet_y,  planet_z)
    // and so on

    auto &details = std::any_cast<const PositionDetails&>(value);

    glm::vec3 pos {
        planet_x * ((details.index % 2 == 0) ? -1.0f : 1.0f) * static_cast<float>(details.xSign),
        planet_y * static_cast<float>(details.index - 1),
        planet_z
    };

    planet->setIndex(details.index);
    planet->changeClusterPos(pos - planet->getPos());
    planet->translate();
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
