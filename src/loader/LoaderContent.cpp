#include "LoaderContent.h"
#include "game/GameContent.h"

#include <algine/core/Engine.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/Window.h>
#include <algine/core/Resources.h>
#include <algine/core/font/FontLibrary.h>
#include <algine/core/log/Log.h>

LoaderContent::LoaderContent(Object *parent)
    : Content(parent)
{
    // enable thread safety, so we can safely run lua scripts from different threads
    Lua::getDefault().setThreadSafety(true);

    FontLibrary::setDefault(Font("fonts/roboto-condensed.regular.ttf"));

    // load locale
    auto &ioSystem = Engine::getDefaultIOSystem();
    auto resources = Resources::instance();
    auto localeFile = "res/locale/" + Engine::getLanguage() + ".xml";

    if (ioSystem->exists(localeFile)) {
        resources->fromXMLFile(localeFile);
    } else {
        resources->fromXMLFile("res/locale/en.xml");
    }

    // create UI scene
    m_uiScene = new LoaderUIScene(this); // TODO
    m_uiScene->listen(this);

    // load game resources, set parent as this to be able
    // to use current window's rendering queue
    m_gameContent = new GameContent(this);

    m_loader.loadAsync(m_gameContent, [this] {
        Log::info("LoaderContent") << "Loaded";

        // set to nullptr to prevent from auto destroying
        m_gameContent->setParent(nullptr);

        getWindow()->setContentLater(m_gameContent);
    }, [this](int progress) {
        Log::info("LoaderContent") << progress << " / " << m_loader.getTotalTasks();
    });

    Framebuffer::setClearColor(0.0f, 0.0f, 0.0f);
}

void LoaderContent::onShow() {
    m_uiScene->setSize(width(), height());
    m_uiScene->show();
}

void LoaderContent::render() {
    Framebuffer::getDefault()->bind();
    Framebuffer::getDefault()->clearColorBuffer();

    m_uiScene->render();

    m_loader.pollTasks();
}
