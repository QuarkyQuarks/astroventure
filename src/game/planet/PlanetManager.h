#ifndef ASTROVENTURE_PLANETMANAGER_H
#define ASTROVENTURE_PLANETMANAGER_H

#include "PlanetGenerator.h"
#include "OrbitGenerator.h"

#include <map>
#include <any>

class PlanetManager: public Object, public Loadable {
public:
    enum Flag {
        None = 0b000,
        DisableOrbit = 0b001,
        JustClear = 0b010,
        CalcPosition = 0b100
    };

    enum class Property {
        /**
         * Key must be of type `PlanetManager::PositionDetails`.
         * If specified, the planet's position will be set
         * according to its index and sign.
         */
        PositionDetails
    };

    struct PositionDetails {
        int index;
        int xSign;
    };

    using Properties = std::map<Property, std::any>;

    using Callback = std::function<void(Planet*)>;

public:
    explicit PlanetManager(Object *parent = defaultParent());

    /**
     * Returns a planet with the specified flags applied.
     * @param flags The flags to be applied.
     * @param properties The map of properties.
     * @return The planet.
     */
    Planet* get(int flags = 0, const Properties &properties = {});

    /**
     * Returns a planet asynchronously with the
     * specified flags applied.
     * @param callback The callback that will be executed
     * in the caller thread or in the rendering thread.
     * @param flags The flags to be applied.
     * @param properties The map of properties.
     */
    void getAsync(Callback callback, int flags = 0, const Properties &properties = {});

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

    void applyFlags(Planet *planet, int flags, const Properties &properties);

    static void applyJustClear(Planet *planet);
    static void applyCalcPosition(Planet *planet, const Properties &properties);

private:
    OrbitGenerator *m_orbitGenerator;
    PlanetGenerator *m_planetGenerator;

    // We need this mutex for function <code>findBase</code>,
    // since we need to be sure that the same base will not be
    // returned for two different threads
    std::mutex m_basePlanetMutex;
};

#endif //ASTROVENTURE_PLANETMANAGER_H
