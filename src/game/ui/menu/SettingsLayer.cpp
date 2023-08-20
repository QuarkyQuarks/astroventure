#include "SettingsLayer.h"
#include "game/ui/GameUIScene.h"

#include <algine/core/widgets/Container.h>

namespace UI {
SettingsLayer::SettingsLayer(GameUIScene *scene)
    : MenuLayer(scene, "ui/Settings.xml")
{
    setCloseWidget(getContainer()->findChild<Widget*>("exit"));
}
} // UI
