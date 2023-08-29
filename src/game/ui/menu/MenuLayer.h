#ifndef ASTROVENTURE_MENULAYER_H
#define ASTROVENTURE_MENULAYER_H

#include "game/ui/Layer.h"

#include <algine/core/widgets/Widget.h>
#include <algine/std/Blur.h>

#include <string_view>
#include <functional>
#include <bitset>

namespace UI {
class GameUIScene;

class MenuLayer: public Layer {
public:
    MenuLayer(GameUIScene *scene, std::string_view file);

    /**
     * Draws the current layer. If necessary, updates the background.
     */
    void draw(const WidgetDisplayOptions &options) override;

protected:
    enum class Flag {
        Open,
        Close,
        BlockClose,
        BackgroundUpdateRequired
    };

    using CloseAction = std::function<void()>;

    class MenuAnimation;

protected:
    void setFlag(Flag flag, bool enabled = true);
    bool isFlagEnabled(Flag flag) const;

protected:
    void onShow() override;
    void setCloseWidget(Widget *widget, const CloseAction &closeAction = {});

    /**
     * Closes the menu with a fade animation.
     * @return `true` is success, `false` otherwise
     * @note `false` will be returned if the menu is already being closed
     */
    bool close();

    /**
     * Blocks the ability to close the menu
     */
    void blockClose();

    /**
     * Unblocks the ability to close the menu
     */
    void unblockClose();

    /**
     * @return `true` is close is blocked, `false` otherwise
     */
    bool isCloseBlocked() const;

    /**
     * The menu can be closed if the following conditions
     * are met:
     * 1. The menu is not being closed;
     * 2. User block is disabled (close is unblocked);
     * @return `true` if the menu can be closed, `false`
     * otherwise.
     */
    bool canBeClosed() const;

private:
    /**
     * Configures blurred background. Must be called
     * from the constructor.
     */
    void cfgBackground();

    void updateBackground(const WidgetDisplayOptions &options);

    void requestBackgroundUpdate();
    void cancelBackgroundUpdate();

    /**
     * Checks if background needs to be updated.
     * @return `true` if the `Flag::BackgroundUpdateRequired`
     * is set, `false` otherwise.
     */
    bool shouldUpdateBackground() const;

private:
    std::bitset<4> m_flags;

private:
    Blur *m_background;
};
} // UI

#endif //ASTROVENTURE_MENULAYER_H
