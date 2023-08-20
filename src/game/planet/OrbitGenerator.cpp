#include "OrbitGenerator.h"

#include "game/math/MathConstants.h"
#include "game/math/VecTools.h"

#include <algine/core/Window.h>
#include <algine/std/model/ShapeBuilder.h>

#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;
using namespace tulz;

OrbitGenerator::OrbitGenerator(Object *parent)
    : Object(parent),
      m_platformShape() {}

PlanetOrbit* OrbitGenerator::generate() {
    constexpr int maxPlatformNumber = 6;

    struct Info {float angle; int chunkNumber;};
    std::vector<Info> platforms;
    platforms.reserve(maxPlatformNumber);

    constexpr int minChunkNumber0 = 7;
    constexpr int maxChunkNumber0 = 15;
    platforms.emplace_back((Info){0.0f, Random::get(minChunkNumber0, maxChunkNumber0)});

    constexpr int overallChunkOffset = 3 * 2;

    for (int i = 1; i < maxPlatformNumber; ++i) {
        float chunkAngle = PlanetOrbit::ChunkAngle;
        const float previousPlatform = platforms[i - 1].angle + static_cast<float>(platforms[i - 1].chunkNumber) * chunkAngle;

        const float availableAngleRange = TWO_PI - (previousPlatform + overallChunkOffset * chunkAngle);
        const int availableChunksRange = static_cast<int>(availableAngleRange / chunkAngle);

        constexpr int minChunkNumber = 4;

        if (availableChunksRange > minChunkNumber) {
            const int chunks = Random::get(minChunkNumber, availableChunksRange);

            const float minOffset = chunkAngle * 2.5f;
            const float maxOffset = minOffset + availableAngleRange - static_cast<float>(chunks) * chunkAngle;
            const float offset = Random::get(minOffset, maxOffset);

            platforms.emplace_back((Info){previousPlatform + offset, chunks});
        } else {
            break;
        }
    }

    Array<PlanetOrbit::Platform> tmp(platforms.size());

    const float offsetAngle = Random::get(0.0f, TWO_PI);

    int i = 0;
    for (auto & platform : platforms) {
        tmp[i++] = {
            adjustedAngle(platform.angle + offsetAngle),
            tulz::Array<PlanetOrbit::Chunk>(platform.chunkNumber, {
                .radius = PlanetOrbit::defaultPlatformRadius,
                .velocity = 0.0f,
                .isFell = false
            })
        };
    }

    Array<PlanetOrbit::Platform> tmp2 {
        {0, tulz::Array<PlanetOrbit::Chunk>(38 / 2, {PlanetOrbit::defaultPlatformRadius, 0.0f, false})},
        {PI, tulz::Array<PlanetOrbit::Chunk>(38 / 2, {PlanetOrbit::defaultPlatformRadius, 0.0f, false})}
    };

    auto orbit = new PlanetOrbit(std::move(tmp2), this);
    orbit->setShape(m_platformShape);
    orbit->setRotatorType(Rotator::Type::Euler);
    orbit->translate();
    orbit->setScale({0.122f, 0.122f, 0.03f}); // TODO: !!!
    orbit->scale();
    orbit->transform();

    return orbit;
}

LoaderConfig OrbitGenerator::resourceLoaderConfig() {
    return LoaderConfig::create<OrbitGenerator>({
        .loader = [this]() {
            ShapeBuilder shapeBuilder;
            shapeBuilder.setParent(this);
            shapeBuilder.setModelPath("models/platform/platform5.obj");
            shapeBuilder.loadFile();
            shapeBuilder.applyParams();
            shapeBuilder.genBuffers();

            auto shape = shapeBuilder.getCurrentShape();

            getParentWindow()->invokeLater([shape, this]() {
                shape->createInputLayout(SpaceModel::getLocations());
                m_platformShape = shape;
                return true;
            });
        },
        .useContext = true
    });
}
