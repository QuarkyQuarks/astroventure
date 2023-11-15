#include "Layer.h"
#include "GameUIScene.h"
#include "game/GameScene.h"

#include <algine/core/assert_cast.h>
#include <algine/core/widgets/Container.h>
#include <algine/core/log/Log.h>

namespace UI {
Layer::Layer(GameUIScene *scene)
    : Widgets::Layer(scene)
{
    auto &colorSchemeManager = scene->parentGameScene()->getColorSchemeManager();
    colorSchemeManager.addOnChangedListener([this] {
        updateColorScheme();
    });
}

GameUIScene* Layer::parentGameUIScene() const {
    return assert_cast<GameUIScene*>(getParent());
}

static void updateColorScheme(Container *inContainer) {
    auto update = [](Widget *widget) {
        auto env = widget->getEnv();

        if (env.valid() && env["updateColorScheme"].valid()) {
            auto result = env["updateColorScheme"].get<sol::function>()(widget);

            if (!result.valid()) {
                sol::error error = result;
                Log::error() << error.what();
            } else {
                widget->invalidate();
            }
        }
    };

    update(inContainer);

    for (auto child : inContainer->getWidgets()) {
        if (auto container = dynamic_cast<Container*>(child); container) {
            updateColorScheme(container);
        } else {
            update(child);
        }
    }
}

void Layer::updateColorScheme() {
    UI::updateColorScheme(getContainer());
}

void Layer::onShow() {
    updateColorScheme();
    unmuteSubscriptions();
}

void Layer::onHide() {
    muteSubscriptions();
}

USubscription& Layer::addSubscription(USubscription subscription) {
    return m_subscriptions.emplace_back(std::move(subscription));
}

void Layer::muteSubscriptions() {
    forEachSubscription([](auto &s) { s->mute(); });
}

void Layer::unmuteSubscriptions() {
    forEachSubscription([](auto &s) { s->unmute(); });
}

void Layer::unsubscribeSubscriptions() {
    forEachSubscription([](auto &s) { s->unsubscribe(); });
}
} // UI
