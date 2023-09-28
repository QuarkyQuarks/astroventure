#ifndef ASTROVENTURE_PARTICLESYSTEM_H
#define ASTROVENTURE_PARTICLESYSTEM_H

#include <algine/std/model/Shape.h>
#include <tulz/observer/Subject.h>
#include <tulz/Array.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <algine/core/math/BezierCurve.h>

using namespace algine;
using namespace tulz;

class GameScene;

class ParticleSystem: public Object {
public:
    struct Particle {
        glm::vec3 pos;
        glm::vec3 rotate;
        float scale;
        int shapeId;
        void *data;
    };

public:
    explicit ParticleSystem(GameScene *parent);

    GameScene* parentGameScene() const;

    void update();

    glm::mat4 getTransform(const Particle &particle) const;
    Shape* getShape(int id) const;
    const std::forward_list<Particle>& getParticles() const;
    size_t getCount() const;

    void addParticles(std::forward_list<Particle> particles);
    void addParticle(Particle particle);

    Subscription<> addOnParticleSpawnedListener(const Observer<> &observer);
    Subscription<> addOnParticleRemovedListener(const Observer<> &observer);

    std::forward_list<Particle>::iterator begin();
    std::forward_list<Particle>::iterator end();

protected:
    virtual bool isRemove(const Particle &particle) const = 0;
    virtual void removed(const Particle &particle);

    virtual void updateParticle(Particle &particle);

protected:
    tulz::Array<Shape*> m_pool;
    std::forward_list<Particle> m_particles;
    size_t m_particleCount {0};

    Subject<> m_onParticleSpawned;
    Subject<> m_onParticleRemoved;
};

#endif //ASTROVENTURE_PARTICLESYSTEM_H
