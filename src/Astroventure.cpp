#include <algine/core/Engine.h>

#include "loader/LoaderContent.h"

using namespace algine;

#ifdef __ANDROID__
#include <algine/Android.h>

#include <algine/core/Screen.h>

AlgineAppInit {
    Screen::instance()->setContent(new LoadingContent);
}
#else
#include <algine/core/window/QtWindow.h>

int main(int argc, char *argv[]) {
    Engine::init();

    auto window = new QtWindow();
    window->setDimensions(500, 1000);
    window->addOnInitializedListener([window]() {
        window->setContent(new LoaderContent());
        window->renderLoop();
    });

    window->show();

    Engine::wait();
    Engine::destroy();

    return 0;
}
#endif
