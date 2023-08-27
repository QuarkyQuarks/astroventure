#ifndef ASTROVENTURE_LAYER_H
#define ASTROVENTURE_LAYER_H

#include <algine/core/widgets/Layer.h>

#include <tulz/observer/USubscription.h>

using namespace algine;

namespace UI {
class GameUIScene;

class Layer: public Widgets::Layer {
public:
    explicit Layer(GameUIScene *scene);

    GameUIScene* parentGameUIScene() const;

    void updateColorScheme();

protected:
    void onShow() override;
    void onHide() override;

protected:
    /**
     * Adds the subscription to the subscription list.
     * Subscriptions from this list are muted when the
     * layer is being hidden and unmuted when the layer
     * is being shown.
     * @param subscription The subscription to add.
     * @return The reference to the added subscription.
     */
    tulz::USubscription& addSubscription(tulz::USubscription subscription);

    /**
     * Performs the same action as making multiple calls to
     * `addSubscription`.
     * @tparam T The type of the first subscription.
     * `tulz::USubscription` must be constructible from `T`.
     * @tparam Args The types of the additional subscriptions.
     * @param subscription The first subscription to be added.
     * @param args The additional subscriptions to be added.
     * @note The arguments will be automatically moved.
     */
    template<typename T, typename ...Args>
    void addSubscriptions(T &&subscription, Args&&... args) {
        static_assert(std::is_constructible_v<tulz::USubscription, std::remove_cvref_t<T>>);

        addSubscription(std::move(subscription));

        if constexpr (sizeof...(args) > 0) {
            addSubscriptions(std::forward<Args>(args)...);
        }
    }

    template<typename C>
    void forEachSubscription(C &&consumer) {
        static_assert(std::is_invocable_r_v<void, C, tulz::USubscription&>);

        for (tulz::USubscription &subscription : m_subscriptions) {
            consumer(subscription);
        }
    }

    void muteSubscriptions();
    void unmuteSubscriptions();
    void unsubscribeSubscriptions();

private:
    std::vector<tulz::USubscription> m_subscriptions;
};
} // UI

#endif //ASTROVENTURE_LAYER_H
