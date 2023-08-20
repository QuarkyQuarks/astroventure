#ifndef ASTROVENTURE_LOADERUILAYER_H
#define ASTROVENTURE_LOADERUILAYER_H

#include <algine/core/widgets/Layer.h>
#include <algine/core/widgets/Widget.h>

using namespace algine;

class LoaderUIScene;

class LoaderUILayer: public Widgets::Layer {
public:
    explicit LoaderUILayer(LoaderUIScene *scene);

    void draw(const algine::WidgetDisplayOptions &options) override;

public:
    Widget *m_label;
    Widget *m_orbits;
};

#endif //ASTROVENTURE_LOADERUILAYER_H
