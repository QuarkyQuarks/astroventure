#ifndef ASTROVENTURE_CONTROLLER_H
#define ASTROVENTURE_CONTROLLER_H

#include <algine/core/unified/Event.h>

#include <glm/vec2.hpp>

class GameScene;

/**
 * Used to control behaviour for the different game
 * scenarios, "what must happen on non-gui click",
 * e.g. start screen, in game screen
 */
class Controller {
public:
    class Event;

public:
    explicit Controller(GameScene &gameScene);
    virtual ~Controller() = default;

    virtual void onActivated() = 0;
    virtual void onDeactivated() = 0;

    virtual void update() = 0;
    virtual void event(const algine::Event &event) = 0;

protected:
    template<typename T>
    void emitEvent(T::EventType id) {
        assert(typeid(T) == typeid(*this));
        emitEventImpl(Event {id});
    }

private:
    void emitEventImpl(const Event &event);

protected:
    GameScene &m_gameScene;
};

/**
 * Represents event emitted by a controller.
 * Should be emitted by `Controller::emitEvent`.
 */
class Controller::Event {
    friend class Controller;

public:
    auto getId() const {
        return m_id;
    }

    bool operator==(const Event &other) const {
        return m_id == other.m_id && m_info == other.m_info;
    }

    template<typename T>
    bool operator==(T id) const {
        static_assert(std::is_enum_v<T>, "Only enums are accepted");
        return m_id == static_cast<int>(id) && m_info == typeid(T);
    }

private:
    template<typename T>
    explicit Event(T id)
        : m_info(typeid(id)),
          m_id(static_cast<int>(id)) {}

private:
    const std::type_info &m_info;
    int m_id;
};

#endif //ASTROVENTURE_CONTROLLER_H
