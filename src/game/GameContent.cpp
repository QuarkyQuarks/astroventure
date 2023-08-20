#include "GameContent.h"

#include <algine/core/Engine.h>
#include <algine/core/Window.h>

#include <algine/core/log/Log.h>

#include <format>

#if defined(__ANDROID__) && defined(DISABLE_FPS_LIMIT)
#include <EGL/egl.h>
#endif

#define LOG_TAG "GameContent"

using namespace glm;

GameContent::GameContent(Object *parent)
    : Content(parent),
      m_scene(new GameScene(this))
{
    Engine::enableDepthTest();
    Engine::enableDepthMask();
    Engine::enableFaceCulling();

#if defined(__ANDROID__) && defined(DISABLE_FPS_LIMIT)
    eglSwapInterval(eglGetCurrentDisplay(), 0);
#endif

    m_fpsMeter.addOnDataReadyListener([](FPSMeter::Data data) {
        Log::info(LOG_TAG, std::format("{} FPS, {} ms", data.framesPerSecond, data.timePerFrame));
    });
}

void GameContent::render() {
    m_fpsMeter.tick();
    m_scene->render();
}

void GameContent::onShow() {
    getWindow()->setEventHandler(this);

    // must be triggered due to some init reasons
    onSizeChanged(width(), height());
}

void GameContent::onSizeChanged(int width, int height) {
    m_onSizeChanged.notify(width, height);
}

void GameContent::onFocusChanged(FocusInfo info) {
    auto reason = info.getReason();

    if (reason == FocusInfo::AppPaused || reason == FocusInfo::FocusLost) {
        m_onFocusLost.notify();
    } else if (reason == FocusInfo::AppResumed || reason == FocusInfo::Focused) {
        m_onFocusRestored.notify();
    }
}

Subscription<> GameContent::addOnFocusLostListener(const Observer<> &listener) {
    return m_onFocusLost.subscribe(listener);
}

Subscription<> GameContent::addOnFocusRestoredListener(const Observer<> &listener) {
    return m_onFocusRestored.subscribe(listener);
}

Subscription<int, int> GameContent::addOnSizeChangedListener(const Observer<int, int> &listener) {
    return m_onSizeChanged.subscribe(listener);
}

LoaderConfig GameContent::resourceLoaderConfig() {
    return LoaderConfig::create<GameContent>({
        .children = {m_scene}
    });
}
