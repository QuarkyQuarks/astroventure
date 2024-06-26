#ifdef ALGINE_QT_PLATFORM

#include <algine/core/window/QtWindow.h>

#include "loader/LoaderContent.h"

void init() {
    auto window = new QtWindow();
    window->setDimensions(500, 1000);
    window->setDPI(200); // TODO: just for testing
    window->addOnInitializedListener([window]() {
        window->setContent(new LoaderContent(window));
        window->renderLoop();
    });

    window->show();
}

#endif