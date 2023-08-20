/*
 * Main Game Content
 */

#ifndef GLES3JNI_APPCONTENT_H
#define GLES3JNI_APPCONTENT_H

#include <algine/core/Content.h>
#include <algine/core/unified/UnifiedEventHandler.h>

#include <tulz/observer/Subject.h>

#include "GameScene.h"
#include "FPSMeter.h"
#include "loader/Loadable.h"

using namespace algine;
using namespace tulz;

class GameContent: public Content, public UnifiedEventHandler, public Loadable {
public:
    explicit GameContent(Object *parent);

    void render() override;

    void onShow() override;

    void onSizeChanged(int width, int height) override;
    void onFocusChanged(FocusInfo info) override;

    Subscription<> addOnFocusLostListener(const Observer<> &listener);
    Subscription<> addOnFocusRestoredListener(const Observer<> &listener);
    Subscription<int, int> addOnSizeChangedListener(const Observer<int, int> &listener);

    LoaderConfig resourceLoaderConfig() override;

private:
    Subject<> m_onFocusLost;
    Subject<> m_onFocusRestored;
    Subject<int, int> m_onSizeChanged;

private:
    // This member must be initialized after subjects.
    // Otherwise, we will get undefined behaviour.
    // http://eel.is/c++draft/class.base.init#13.3
    GameScene *m_scene;

private:
    FPSMeter m_fpsMeter;
};

#endif //GLES3JNI_APPCONTENT_H
