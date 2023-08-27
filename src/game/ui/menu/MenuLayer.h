#ifndef ASTROVENTURE_MENULAYER_H
#define ASTROVENTURE_MENULAYER_H

#include "game/ui/Layer.h"

#include <algine/core/widgets/Widget.h>

#include <string_view>
#include <functional>
#include <bitset>

namespace UI {
class GameUIScene;

class MenuLayer: public Layer {
public:
    enum class Flag {
        Open,
        Close,
        BlockClose
    };

    using CloseAction = std::function<void()>;

public:
    MenuLayer(GameUIScene *scene, std::string_view file);

    bool isAnimationActive() const;

    void setFlag(Flag flag, bool enabled = true);
    bool isFlagEnabled(Flag flag) const;

    //void draw(const WidgetDisplayOptions &options) override;

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
    std::bitset<3> m_flags;
};
} // UI

#endif //ASTROVENTURE_MENULAYER_H
