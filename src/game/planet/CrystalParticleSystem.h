#ifndef ASTROVENTURE_CRYSTALPARTICLESYSTEM_H
#define ASTROVENTURE_CRYSTALPARTICLESYSTEM_H

#include "loader/Loadable.h"
#include "game/base/ParticleSystem.h"

class CrystalParticleSystem: public ParticleSystem, public Loadable {
public:
    explicit CrystalParticleSystem(GameScene *parent);

    void setEndPoint(const glm::vec3 &endPoint);
    void spawn(const glm::vec3 &pos, const glm::vec3 &dir, int count);

    LoaderConfig resourceLoaderConfig() override;

protected:
    bool isRemove(const ParticleSystem::Particle &particle) const override;
    void removed(const ParticleSystem::Particle &particle) override;

    void updateParticle(ParticleSystem::Particle &particle) override;

private:
    void loadResources();

private:
    glm::vec3 m_endPoint;
};


#endif //ASTROVENTURE_CRYSTALPARTICLESYSTEM_H
