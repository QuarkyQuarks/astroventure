#include "ParticleSystem.h"
#include "game/GameScene.h"

#include <algine/core/assert_cast.h>

ParticleSystem::ParticleSystem(GameScene *parent)
    : Object(parent) {}

GameScene* ParticleSystem::parentGameScene() const {
    return assert_cast<GameScene*>(getParent());
}

void ParticleSystem::update() {
    auto scene = parentGameScene();
    auto time = scene->getFrameTimeSec();

    auto prevIt = m_particles.before_begin();

    for (auto it = m_particles.begin(); it != m_particles.end();) {
        Particle &particle = *it;

        if (isRemove(particle)) {
            removed(particle);
            --m_particleCount;
            it = m_particles.erase_after(prevIt);
            continue;
        }

        particle.pos += particle.velocity * time;

        glm::vec3 acceleration = getAcceleration(particle);
        particle.velocity += acceleration * time;

        if (particle.rotationVelocity != glm::vec3(0.0f))
            particle.rotate += particle.rotationVelocity * time;

        ++prevIt;
        ++it;
    }
}

glm::mat4 ParticleSystem::getTransform(const ParticleSystem::Particle &particle) const {
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), particle.pos);
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(particle.scale));

    if (particle.rotate == glm::vec3(0.0f))
        return translation * scaling;

    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(rotation, particle.rotate.x, {1.0f, 0.0f, 0.0f});
    rotation = glm::rotate(rotation, particle.rotate.y, {0.0f, 1.0f, 0.0f});
    rotation = glm::rotate(rotation, particle.rotate.z, {0.0f, 0.0f, 1.0f});

    return translation * scaling * rotation;
}

Shape* ParticleSystem::getShape(int id) const {
    return m_pool[id];
}

const std::forward_list<ParticleSystem::Particle>& ParticleSystem::getParticles() const {
    return m_particles;
}

size_t ParticleSystem::getCount() const {
    return m_particleCount;
}

void ParticleSystem::addParticles(std::forward_list<Particle> particles) {
    m_particleCount += std::distance(particles.begin(), particles.end());

    if (m_particles.empty()) {
        std::swap(m_particles, particles);
    } else {
        m_particles.splice_after(m_particles.cbegin(), particles);
    }
}

void ParticleSystem::addParticle(Particle particle) {
    m_particles.emplace_front(particle);
    ++m_particleCount;
}

Subscription<> ParticleSystem::addOnParticleSpawnedListener(const Observer<> &observer) {
    return m_onParticleSpawned.subscribe(observer);
}

Subscription<> ParticleSystem::addOnParticleRemovedListener(const Observer<> &observer) {
    return m_onParticleRemoved.subscribe(observer);
}

std::forward_list<ParticleSystem::Particle>::iterator ParticleSystem::begin() {
    return m_particles.begin();
}

std::forward_list<ParticleSystem::Particle>::iterator ParticleSystem::end() {
    return m_particles.end();
}

void ParticleSystem::removed(const Particle &particle) {}
