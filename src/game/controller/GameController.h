#ifndef ASTROVENTURE_GAMECONTROLLER_H
#define ASTROVENTURE_GAMECONTROLLER_H

#include "Controller.h"

class Planet;

/**
 * The class that describes in-game logic.
 */
class GameController: public Controller {
public:
    enum class EventType {
        Game
    };

public:
    explicit GameController(GameScene &gameScene);

    void onActivated() override;
    void onDeactivated() override;

    void update() override;
    void event(const algine::Event &event) override;

private:
    void reset();
    void resetPlanets();
    void resetSpacecraft();
    void resetCamera();

private:
    void blockLaunch();
    void unblockLaunch();

    void launchEventHandler(const algine::Event &event);

    void (GameController::*eventHandler)(const algine::Event &event);

private:
    void generatePlanets(int count);

    void triggerScroll(Planet *dst);

    void acquireScrollLock();
    void releaseScrollLock();
    bool isScrollLocked() const;

public:
    /**
     * This function is executed before the actual scroll.
     * It checks if all the requirements for the scroll to
     * start are met.
     */
    void beforeScroll();

    /**
     * The function that actually performs the scroll.
     */
    void onScroll();

    /**
     * The function that performs post scroll actions:
     * sets final position
     * @note it is not a scroll step
     */
    void afterScroll();

    void (GameController::*scroll)();

private:
    struct {
        int lockCount {0};
        float startTimeMs {0};
        float currentDistance {0};
        float totalDistance {0};
        Planet *dst {};
    } m_scrollDetails;
};

#endif //ASTROVENTURE_GAMECONTROLLER_H
