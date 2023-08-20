#ifndef ASTROVENTURE_PLANETMANAGER_H
#define ASTROVENTURE_PLANETMANAGER_H

#include "PlanetGenerator.h"
#include "OrbitGenerator.h"

class PlanetManager: public Object, public Loadable {
public:
    enum Flag {
        None = 0b00,
        DisableOrbit = 0b01,
        JustClear = 0b10
    };

    using Callback = std::function<void(Planet*)>;

public:
    explicit PlanetManager(Object *parent = defaultParent());

    Planet* get(int flags = 0);
    void getAsync(Callback callback, int flags = 0);

    /**
     * Takes ownership of <code>planet</code>
     * @note You should call this function
     * instead of deleting planet
     * @param planet
     */
    void done(Planet *planet);

    LoaderConfig resourceLoaderConfig() override;

private:
    /**
     * @return child of PlanetManager of type Planet*
     * with removed orbit and parent
     * @note since parent will be set to null, you must
     * take care of lifecycle of the returned object
     */
    Planet* popBase();

    void clear(Planet *base, int flags);

private:
    OrbitGenerator *m_orbitGenerator;
    PlanetGenerator *m_planetGenerator;

    // We need this mutex for function <code>findBase</code>,
    // since we need to be sure that the same base will not be
    // returned for two different threads
    std::mutex m_basePlanetMutex;
};

#endif //ASTROVENTURE_PLANETMANAGER_H
