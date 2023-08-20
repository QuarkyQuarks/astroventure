#ifndef ASTROVENTURE_GAMELAYER_H
#define ASTROVENTURE_GAMELAYER_H

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/Label.h>

#include "Layer.h"

namespace UI {
class GameUIScene;

class GameLayer: public Layer {
public:
    explicit GameLayer(GameUIScene *scene);

    void reset();

private:
    void calcCrystalsEndPoint(float width, float height);

    void scoreChanged();
    void crystalsChanged();

private:
    Container *m_scoreContainer;
    Container *m_crystalsContainer;
    Label *m_labelScore;
    Label *m_labelCrystals;
};
} // UI

#endif //ASTROVENTURE_GAMELAYER_H
