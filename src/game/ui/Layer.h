#ifndef ASTROVENTURE_LAYER_H
#define ASTROVENTURE_LAYER_H

#include <algine/core/widgets/Layer.h>

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
};
} // UI

#endif //ASTROVENTURE_LAYER_H
