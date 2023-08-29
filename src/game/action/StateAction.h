#ifndef ASTROVENTURE_STATEACTION_H
#define ASTROVENTURE_STATEACTION_H

#include <tulz/observer/Subject.h>

#include <atomic>

using namespace tulz;

class GameScene;

/**
 * State action is an action that triggers
 * two events: when the action is triggered
 * and when it has completed.
 */
class StateAction {
public:
    explicit StateAction(GameScene &scene);

    /**
     * @note This function must be called from the main rendering thread.
     */
    void trigger();

    /**
     * Use this function to lock the action from being completed.
     * Must be called in an on trigger listener.
     * @note This function must be called from the main rendering thread.
     */
    void lock();

    /**
     * Use this function to indicate that some object has completed all the
     * work that was preventing the action from being completed.
     * This function triggers a check to determine if the number of `lock`
     * calls is equal to the number of `end` calls. If they are equal, the
     * `onCompleted` event will be triggered.
     * @note This function may be called from any thread.
     */
    void unlock();

    /**
     * @return `true` if the action is locked, `false` otherwise.
     */
    bool isLocked() const;

    /**
     * The event `onTrigger` is triggered by the `trigger` function.
     * @param listener The listener function to be added.
     * @return Subscription object representing the added listener.
     */
    Subscription<> addOnTriggerListener(const Observer<> &listener);

    /**
     * The event `onCompleted` is triggered when the action is completed.
     * @param listener The listener function to be added.
     * @return Subscription object representing the added listener.
     */
    Subscription<> addOnCompletedListener(const Observer<> &listener);

protected:
    GameScene &m_gameScene;

private:
    Subject<> m_onTrigger;
    Subject<> m_onCompleted;
    std::atomic_int m_lockCount;
};

#endif //ASTROVENTURE_STATEACTION_H
