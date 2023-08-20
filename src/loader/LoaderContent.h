#ifndef ASTROVENTURE_LOADERCONTENT_H
#define ASTROVENTURE_LOADERCONTENT_H

#include <algine/core/Content.h>
#include <algine/core/unified/UnifiedEventHandler.h>

#include "ui/LoaderUIScene.h"
#include "Loader.h"

using namespace algine;

class GameContent;

class LoaderContent: public Content, public UnifiedEventHandler {
public:
    LoaderContent();

    void onShow() override;

    void render() override;

private:
    Loader m_loader;
    GameContent *m_gameContent;

private:
    LoaderUIScene *m_uiScene;
};

#endif //ASTROVENTURE_LOADERCONTENT_H
