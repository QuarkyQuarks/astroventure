#include "Loader.h"

#include <algine/core/log/Log.h>

#include <thread>
#include <cassert>
#include <ranges>

using namespace algine;

#define LOG_TAG "Loader"

Loader::Loader()
    : m_progress(0) {}

Loader::~Loader() {
    m_threadPool.stop();
}

void Loader::loadAsync(Loadable *root,
                       OnCompleteCallback onComplete,
                       OnProgressCallback onProgress)
{
    m_threadPool.setMaxThreadCount(std::max((int) std::thread::hardware_concurrency(), 2));
    m_contextPool.resize(m_threadPool.getMaxThreadCount());

    m_onComplete = std::move(onComplete);
    m_onProgress = std::move(onProgress);

    buildTasks(root);
}

void Loader::pollTasks() {
    std::forward_list<Task*> readyTasks;

    // step 1: collect ready tasks

    auto it_prev = m_pendingTasks.before_begin();
    auto it = m_pendingTasks.begin();

    while (it != m_pendingTasks.end()) {
        auto &task = m_tasks[*it];

        bool isReady = std::ranges::all_of(task.config.dependsOn, [this](Loadable *dep) {
            return static_cast<bool>(m_tasks[dep].isLoaded);
        });

        if (isReady) {
            readyTasks.emplace_front(&task);
            it = m_pendingTasks.erase_after(it_prev);
        } else {
            ++it_prev;
            ++it;
        }
    }

    // step 2: execute ready tasks
    for (auto task : readyTasks) {
        loadAsync(*task);
    }
}

int Loader::getTotalTasks() {
    return static_cast<int>(m_tasks.size());
}

inline static std::string configToString(const LoaderConfig &config) {
    return config.name.empty() ? "<unnamed config>" : "'" + config.name + "'";
}

void Loader::buildTasks(Loadable *loader) {
    auto config = loader->resourceLoaderConfig();

    Log::verbose(LOG_TAG) << "Found "
        << (config.isLoadable() ? "loadable" : "non-loadable") << " config "
        << configToString(config);

    if (config.isLoadable()) {
        m_tasks.emplace(std::piecewise_construct,
                        std::forward_as_tuple(loader),
                        std::forward_as_tuple());

        auto &task = m_tasks[loader];
        task.config = config;

        m_pendingTasks.emplace_front(loader);
    } else {
        // non-loadable ResourceLoader cannot have any dependencies
        assert(!config.hasDependencies());
    }

    for (auto child : config.children) {
        buildTasks(child);
    }
}

void Loader::loadAsync(Task &task) {
    m_threadPool.start([&task, this] {
        Log::verbose(LOG_TAG, "Loading " + configToString(task.config));

        Index contextIndex = ContextPool::InvalidIndex;

        if (task.config.useContext) {
            contextIndex = m_contextPool.attach();
            assert(contextIndex != ContextPool::InvalidIndex);
        }

        task.load();

        if (task.config.useContext) {
            m_contextPool.detach(contextIndex);
        }

        taskLoaded(task);
    });
}

void Loader::taskLoaded(Task &task) {
    Log::verbose(LOG_TAG, configToString(task.config) + " has been loaded");

    task.isLoaded = true;

    std::lock_guard locker(m_progressMutex);

    ++m_progress;

    // yes, do not unlock the mutex here
    // we must be sure that m_onProgress and m_onComplete
    // will be called in a rational order, i.e. we don't
    // want something like
    //   m_onProgress(5) -> m_onComplete() => m_onProgress(4)
    // to happen

    m_onProgress(m_progress);

    if (m_progress == getTotalTasks()) {
        m_onComplete();
    }
}
