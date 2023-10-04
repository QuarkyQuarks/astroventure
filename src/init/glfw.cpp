#if !defined(__ANDROID__) && !defined(ALGINE_QT_PLATFORM)

#include <algine/core/window/GLFWWindow.h>

#include "loader/LoaderContent.h"

void init() {
    GLFWWindow window("Astroventure", 500, 1000);
    window.setDPI(200); // TODO: just for testing
    window.setMouseTracking(true);
    window.setKeyboardTracking(true);
    window.setWindowStateTracking(true);
    window.setContentLater<LoaderContent>(&window);
    window.renderLoop();
}

#endif