#include "LoaderUIScene.h"
#include "LoaderUILayer.h"

LoaderUIScene::LoaderUIScene(Object *parent)
    : Widgets::Scene(parent)
{
    auto layer = new LoaderUILayer(this);
    layer->show();
}
