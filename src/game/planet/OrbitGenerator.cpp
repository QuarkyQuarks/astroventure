#include "OrbitGenerator.h"

#include "game/math/MathConstants.h"
#include "game/math/VecTools.h"
#include "game/GameScene.h"

#include <algine/core/Window.h>
#include <algine/std/model/ShapeBuilder.h>
#include <algine/core/log/Log.h>
#include <algine/core/assert_cast.h>

#include <cmath>

#include "effolkronium/random.hpp"

#define LOG_TAG "OrbitGenerator"

using Random = effolkronium::random_static;
using namespace tulz;

OrbitGenerator::OrbitGenerator(Object *parent)
    : Object(parent),
      m_platformShape() {}

static std::vector<float> segmentsGenerator(float density, float minSegmentLength, float minMargin) {
    auto maxSegments = static_cast<int>(density / minSegmentLength);
    auto maxMargins = static_cast<int>((1.0f - density) / minSegmentLength);
    maxSegments = maxSegments > maxMargins ? maxMargins : maxSegments;

    const int partitionsNumber = Random::get(0, maxSegments);

    auto segmentation = [](int gridsNumber, float lowerEdge, float upperEdge, float offset) {
        std::vector<float> partitions {lowerEdge, upperEdge};
        partitions.reserve(gridsNumber + 2);

        for (int i = 0; i < gridsNumber; ++i) {
            float segment = Random::get(lowerEdge + offset, upperEdge - offset);
            partitions.push_back(segment);

            if ((upperEdge - segment) > (segment - lowerEdge)) {
                lowerEdge = segment;
            } else {
                upperEdge = segment;
            }
        }

        std::sort(partitions.begin(), partitions.end());

        std::vector<float> segments;
        segments.reserve(partitions.size() - 1);

        for (int i = 0; i < (partitions.size() - 1); ++i) {
            segments.push_back(partitions[i + 1] - partitions[i]);
        }

        return segments;
    };

    auto segments = segmentation(partitionsNumber, 0.0, density, minSegmentLength);
    auto margins = segmentation(partitionsNumber, density, 1.0, minMargin);

    std::vector<float> processedSegments;
    processedSegments.reserve(2 * segments.size());

    for (int i = 0; i < segments.size(); ++i) {
        processedSegments.push_back(segments[i]);
        processedSegments.push_back(margins[i]);
    }

    return processedSegments;
}

PlanetOrbit* OrbitGenerator::generate() {
    /**
     * density is a main parameter of the whole algorithm, it is normalised from 0.0 to 1.0
     * basically it indicates how much our orbit is filled with platforms
     * density is calculated using some predefined mathematical function (of timestep) that will
     * asymptotically describe behaviour of our generation process
     */

    auto asymptoticDensity = [](float t) {
        return 1.25f - (2.f / PI) * std::atan((t + 14.f) * 0.05f);
    };

    int score = *parentGameScene()->getScore();
    float density = asymptoticDensity((float) score);

    Log::debug(LOG_TAG) << score << " : " << density << Log::endl;

    /// `segmentsGenerator` returns array of segments distributed on a unit segment
    /// then we map this segments into our orbit in form of platforms consisting of chunks

    // minimal size of the platform - 6 chunks, minimal margin between platforms - 4 chunks
    constexpr float minSegmentSize = (float) 4 / (float) PlanetOrbit::ChunkConstant;
    constexpr float minMargin = (float) 2 / (float) PlanetOrbit::ChunkConstant;
    auto processedSegments = segmentsGenerator(density, minSegmentSize, minMargin);

    Array<PlanetOrbit::Platform> platforms(processedSegments.size() / 2);

    const float offsetAngle = Random::get(0.0f, TWO_PI);

    // mapping
    for (int i = 0; i < processedSegments.size(); i += 2) {
        float ratio = 0;
        for (int j = 0; j < i; ++j) {
            ratio += processedSegments[j];
        }

        float angle = ratio * TWO_PI;
        auto chunkNumber = static_cast<int>(processedSegments[i] * PlanetOrbit::ChunkConstant);

        platforms[i / 2] = {
            adjustedAngle(angle + offsetAngle),
            tulz::Array<PlanetOrbit::Chunk>(chunkNumber, {
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

    auto orbit = new PlanetOrbit(std::move(platforms), this);
    orbit->setShape(m_platformShape);
    orbit->setRotatorType(Rotator::Type::Euler);
    orbit->translate();
    orbit->setScale({0.122f, 0.122f, 0.03f}); // TODO: !!!
    orbit->scale();
    orbit->transform();

    return orbit;
}

GameScene* OrbitGenerator::parentGameScene() {
    return assert_cast<GameScene*>(getParentScene());
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
