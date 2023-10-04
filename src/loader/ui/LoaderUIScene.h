#ifndef ASTROVENTURE_LOADERUISCENE_H
#define ASTROVENTURE_LOADERUISCENE_H

#include <algine/core/widgets/Scene.h>

using namespace algine;

class LoaderUIScene: public Widgets::Scene {
public:
    explicit LoaderUIScene(Object *parent);

protected:
    void onShow() override;
};

#endif //ASTROVENTURE_LOADERUISCENE_H
