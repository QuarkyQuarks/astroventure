#ifndef SPACE_EXPLORER_SPACECRAFTREGISTRY_H
#define SPACE_EXPLORER_SPACECRAFTREGISTRY_H

#include "Spacecraft.h"
#include "loader/Loadable.h"

#include <algine/core/context/ContextPool.h>
#include <tulz/threading/ThreadPool.h>

#include <functional>

using namespace algine;
using namespace tulz;

class SpacecraftManager: public Object {
public:
    using Callback = std::function<void(Spacecraft*)>;

public:
    explicit SpacecraftManager(Object *parent = defaultParent());
    ~SpacecraftManager();

    /**
     * Returns fully-initialized spacecraft by the specified id.
     * Loads it if necessary.
     * @param id
     * @return
     */
    Spacecraft* get(int id);

    /**
     * Returns spacecraft by the specified id asynchronously.
     * Loads it if necessary.
     * @note callback will be called in the main UI rendering thread
     * @param id
     * @param callback
     */
    void getAsync(int id, Callback callback);

    bool isLoaded(int id);

private:
    /**
     * Loads spacecraft by the provided id
     * @note this function just *loads*, it doesn't
     * create input layouts
     * @param id
     * @return
     */
    Spacecraft* load(int id);

    /**
     * @param id
     * @return spacecraft with the specified id or nullptr
     * if such a spacecraft doesn't exist
     */
    Spacecraft* findById(int id);

private:
    ThreadPool m_threadPool;
    ContextPool m_contextPool;

    // loaded spacecrafts
    std::forward_list<Spacecraft*> m_spacecrafts;

    std::mutex m_spacecraftsMutex;
};

#endif //SPACE_EXPLORER_SPACECRAFTREGISTRY_H
