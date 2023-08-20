#ifndef ASTROVENTURE_ATMOSPHERERENDERER_H
#define ASTROVENTURE_ATMOSPHERERENDERER_H

#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/InputLayout.h>
#include <algine/std/Camera.h>

#include "loader/Loadable.h"
#include "Planet.h"

using namespace algine;

class AtmosphereRenderer: public Object, public Loadable {
public:
    /**
     * @note must be called from the main UI rendering thread
     * @param camera
     * @param parent
     */
    explicit AtmosphereRenderer(Camera &camera, Object *parent = defaultParent());

    void begin();

    void render(Planet *planet);

    LoaderConfig resourceLoaderConfig() override;

private:
    void loadResources();

private:
    Camera &m_camera;

private:
    InputLayout *m_layout;
    ArrayBuffer *m_vertices;
    ShaderProgram *m_program;
};

#endif //ASTROVENTURE_ATMOSPHERERENDERER_H
