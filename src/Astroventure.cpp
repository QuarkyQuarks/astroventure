#include <algine/core/Engine.h>

#include "init/init.h"

#ifndef __ANDROID__
int main(int argc, char *argv[]) {
    algine::Engine::exec(argc, argv, &init);
    return 0;
}
#else
#include <algine/Android.h>

AlgineAppInit {
    init();
};
#endif
