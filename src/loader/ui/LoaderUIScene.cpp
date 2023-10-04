#include "LoaderUIScene.h"
#include "LoaderUILayer.h"

LoaderUIScene::LoaderUIScene(Object *parent)
    : Widgets::Scene(parent) {}

void LoaderUIScene::onShow() {
    auto layer = new LoaderUILayer(this);
    layer->show();
}
