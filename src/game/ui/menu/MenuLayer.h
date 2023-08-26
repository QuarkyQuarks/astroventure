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
        Close
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

private:
    std::bitset<2> m_flags;
};
} // UI

#endif //ASTROVENTURE_MENULAYER_H
