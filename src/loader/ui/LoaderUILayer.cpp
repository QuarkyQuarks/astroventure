#include "LoaderUILayer.h"
#include "LoaderUIScene.h"

#include <algine/core/widgets/Container.h>

LoaderUILayer::LoaderUILayer(LoaderUIScene *scene)
    : Widgets::Layer(scene)
{
    auto container = Widget::constructFromXMLFile<Container*>("ui/LoadingScreen.xml", this);
    setContainer(container);

    m_orbits = container->findChild<Widget*>("loading_animation");
    m_label = container->findChild<Widget*>("loading_label");
}

void LoaderUILayer::draw(const WidgetDisplayOptions &options) {
    m_orbits->invalidate();
    m_label->invalidate();
    Widgets::Layer::draw(options);
}
