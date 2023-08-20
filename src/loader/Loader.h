#ifndef ASTROVENTURE_LOADER_H
#define ASTROVENTURE_LOADER_H

#include <algine/core/context/ContextPool.h>
#include <tulz/threading/ThreadPool.h>

#include <functional>
#include <atomic>
#include <mutex>
#include <map>

#include "Loadable.h"

using namespace algine;
using namespace tulz;

class Loader {
public:
    using OnCompleteCallback = std::function<void()>;
    using OnProgressCallback = std::function<void(int)>;

public:
    Loader();
    ~Loader();

    /**
     * @param root The root resource loader node
     * @param onComplete callback
     * @param onProgress callback that takes int –
     * current progress *in tasks* as an argument
     * @note must be called from the main rendering thread.
     * Callbacks will be called from the worker (non-
     * rendering) thread.
     */
    void loadAsync(Loadable *root,
                   OnCompleteCallback onComplete,
                   OnProgressCallback onProgress);

    /**
     * Checks whether there are ready tasks and if so,
     * executes them.
     * @note Ready task is such a task that has all
     * dependencies loaded.
     * @note This function must be executed from the
     * rendering loop.
     */
    void pollTasks();

    int getTotalTasks();

private:
    class Task {
    public:
        void load() const {
            config.loader();
        }

    public:
        LoaderConfig config;
        std::atomic_bool isLoaded {false};
    };

    // only loadable tasks here
    std::map<Loadable*, Task> m_tasks;

    // tasks that are waiting for execution
    // set for fast random deletions
    std::forward_list<Loadable*> m_pendingTasks;

private:
    /**
     * Builds tasks map
     * @param loader
     */
    void buildTasks(Loadable *loader);

    /**
     * Loads the specified task asynchronously
     * @param task
     */
    void loadAsync(Task &task);

    /**
     * Changes task status to loaded & increases counter
     * @param task
     */
    void taskLoaded(Task &task);

private:
    ThreadPool m_threadPool;
    ContextPool m_contextPool;
    OnCompleteCallback m_onComplete;
    OnProgressCallback m_onProgress;
    std::mutex m_progressMutex;
    int m_progress;
};

#endif //ASTROVENTURE_LOADER_H
