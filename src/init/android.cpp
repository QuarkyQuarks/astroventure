#ifdef __ANDROID__

#include <algine/core/Screen.h>

#include "loader/LoaderContent.h"

void init() {
    Screen::instance()->setContent(new LoadingContent);
}

#endif