#ifndef ASTROVENTURE_PLANETORBIT_H
#define ASTROVENTURE_PLANETORBIT_H

#include <list>
#include <tulz/Array.h>

#include "game/base/ClusterModel.h"
#include "game/math/MathConstants.h"

class PlanetOrbit: public ClusterModel {
public:
    constexpr static int ChunkConstant = 38;
    constexpr static float ChunkAngle = TWO_PI / ChunkConstant;
    constexpr static float defaultPlatformRadius = 0.95f;
    constexpr static float finalPlatformRadius = 0.65f;
    constexpr static float initialScale = 0.122f; //TODO !!!

public:
    struct Chunk {
        float radius;
        float velocity;
        bool isFell;
    };

    struct Platform {
        float angle;
        tulz::Array<Chunk> chunks;
    };

    class Chunks;

    enum class State {
        AroundPlanet,
        Falling,
        OnPlanet
    };

public:
    explicit PlanetOrbit(tulz::Array<Platform> platforms, Object *parent = defaultParent()); // TODO: planet

    float getRadius() const;

    bool isAroundPlanet() const;
    bool isFalling() const;
    bool isOnPlanet() const;

    State getState() const;

    glm::mat4 getChunkTransform(int platformId, int chunkId);

    float getChunkRoll(int platformId, int chunkId) const;
    float getChunkRadius(int platformId, int chunkId) const;

    int getPlatformsCount() const;
    int getChunksCount(int platformId) const;
    int getChunksCount() const;
    int getFallenChunksCount() const;

    void fall(int touchdownPlatformId);

    /**
     * @note function doesn't take into account orbit's roll
     * @param angle value in radians in range [0, 2Pi]
     * @return <b>index of the platform</b> which contains point with the provided angle
     * <br>if there is no such platform, returns <b>-1</b>
     **/
    int platformAt(float angle) const;

    /**
     * Same as platformAt, but takes into account orbit's roll
     */
    int platformAtAbs(float absAngle) const;

    /**
     * @param angle value in radians
     * @return absolute angle, i.e. `angle + getRoll()`
     * @note there is no guarantee that the result will be in range [0, 2Pi]
     */
    float absAngle(float angle) const;

    Chunks chunks();

    Planet* parentPlanet() const;

private:
    glm::mat4 (PlanetOrbit::*m_step_ptr)(int platformId, int chunkId) {nullptr};

    State m_state;

private:
    void setState(State state);

    glm::mat4 chunkTransformMat(float radius, float roll, float scaleFactor);

    glm::mat4 aroundPlanet(int platformID, int chunkID);
    glm::mat4 falling(int platformID, int chunkID);
    glm::mat4 onPlanet(int platformID, int chunkID);

private:
    tulz::Array<Platform> m_platforms;
    int m_fallenChunks;

    float m_radius;
};

class PlanetOrbit::Chunks {
public:
    class Iterator;

    class Info {
        friend class Iterator;

    public:
        inline Info(int platformId, int chunkId, PlanetOrbit *orbit)
                : m_platformId(platformId), m_chunkId(chunkId), m_orbit(orbit) {}

        inline int platformId() const { return m_platformId; }
        inline int chunkId() const { return m_chunkId; }

        inline Platform& platform() { return m_orbit->m_platforms[m_platformId]; }
        inline Chunk& chunk() { return m_orbit->m_platforms[m_platformId].chunks[m_chunkId]; }

        inline float roll() const { return m_orbit->getChunkRoll(m_platformId, m_chunkId); }
        inline float absRoll() const { return m_orbit->absAngle(roll()); }
        inline float radius() const { return m_orbit->getChunkRadius(m_platformId, m_chunkId); }
        inline glm::mat4 getTransform() { return m_orbit->getChunkTransform(m_platformId, m_chunkId); }

        inline bool operator==(const Info &other) const {
            return m_orbit == other.m_orbit && m_platformId == other.m_platformId && m_chunkId == other.m_chunkId;
        }

    private:
        int m_platformId;
        int m_chunkId;
        PlanetOrbit *m_orbit;
    };

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Info;
        using difference_type = ssize_t;
        using pointer = Info*;
        using reference = Info&;

        explicit Iterator(const Info &info);
        Iterator& operator++(); // Prefix increment
        const Iterator operator++(int); // Postfix increment
        bool operator==(Iterator other) const;
        bool operator!=(Iterator other) const;
        reference operator*() const;

    private:
        Info m_info;
    };

    explicit Chunks(PlanetOrbit &orbit);

    Iterator begin();
    Iterator end();

private:
    PlanetOrbit &m_orbit;
};

#endif
