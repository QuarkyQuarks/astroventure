#include "SpacecraftManager.h"

#include <algine/core/Window.h>
#include <algine/std/model/ShapeBuilder.h>

SpacecraftManager::SpacecraftManager(Object *parent)
    : Object(parent)
{
    m_threadPool.setMaxThreadCount(1);
    m_contextPool.resize(1);
}

SpacecraftManager::~SpacecraftManager() {
    m_threadPool.stop();
}

Spacecraft* SpacecraftManager::get(int id) {
    if (auto spacecraft = findById(id); spacecraft != nullptr)
        return spacecraft;

    auto spacecraft = load(id);

    std::atomic_bool isLoaded {false};

    getParentWindow()->invokeLater([spacecraft, &isLoaded]() {
        spacecraft->getShape()->createInputLayout(SpaceModel::getLocations());
        isLoaded = true;
        isLoaded.notify_one();
        return true;
    });

    isLoaded.wait(false);

    {
        std::lock_guard locker(m_spacecraftsMutex);
        m_spacecrafts.emplace_front(spacecraft);
    }

    return spacecraft;
}

void SpacecraftManager::getAsync(int id, Callback callback) {
    if (auto spacecraft = findById(id); spacecraft != nullptr) {
        getParentWindow()->invokeLater([spacecraft, callback = std::move(callback)]() {
            callback(spacecraft);
        });

        return;
    }

    m_threadPool.start([this, id, callback = std::move(callback)]() mutable {
        auto contextIndex = m_contextPool.attach();
        assert(contextIndex != ContextPool::InvalidIndex);

        auto spacecraft = load(id);

        getParentWindow()->invokeLater([this, spacecraft, callback = std::move(callback)]() {
            spacecraft->getShape()->createInputLayout(SpaceModel::getLocations());

            {
                std::lock_guard locker(m_spacecraftsMutex);
                m_spacecrafts.emplace_front(spacecraft);
            }

            callback(spacecraft);

            return true;
        });

        m_contextPool.detach(contextIndex);
    });
}

Spacecraft* SpacecraftManager::load(int id) {
    ShapeBuilder shapeCreator;
    shapeCreator.setModelPath("models/spacecrafts/0/cylinder.obj");

    shapeCreator.loadFile();
    shapeCreator.applyParams();
    shapeCreator.genBuffers();

    auto spacecraft = new Spacecraft(id, this);
    spacecraft->setShape(shapeCreator.getCurrentShape());
    spacecraft->setRotatorType(Rotator::Type::Euler);

    return spacecraft;
}

static auto spacecraftIdFilter(int id) {
    return [id](Spacecraft *spacecraft) {
        return spacecraft->getId() == id;
    };
}

bool SpacecraftManager::isLoaded(int id) {
    std::lock_guard locker(m_spacecraftsMutex);
    return std::ranges::any_of(m_spacecrafts, spacecraftIdFilter(id));
}

Spacecraft* SpacecraftManager::findById(int id) {
    std::lock_guard locker(m_spacecraftsMutex);
    if (auto it = std::ranges::find_if(m_spacecrafts, spacecraftIdFilter(id)); it != m_spacecrafts.end())
        return *it;
    return nullptr;
}
