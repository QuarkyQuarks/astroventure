#include "AudioManager.h"
#include "game/GameScene.h"
#include "game/GameContent.h"
#include "game/controller/StartController.h"
#include "game/controller/GameController.h"

#include <algine/core/log/Log.h>

AudioManager::AudioManager(GameScene &scene)
    : m_scene(scene)
{
    auto playOnEvent = [this](Audio audio) {
        return [audio, this]() {
            m_player.play(audio);
        };
    };

    auto &mechanics = m_scene.getMechanics();
    mechanics.addOnLaunchListener(playOnEvent(SfxTakeoff));
    mechanics.addOnLandingListener([this](Planet*) {
        m_player.play(SfxTouchdown);
    });
    // game->addOnScrollListener(playOnEvent(SfxScroll));

    // TODO: scene
    auto &crystalParticles = m_scene.getCrystalParticles();
    crystalParticles.addOnParticleSpawnedListener(playOnEvent(SfxCrystal1));
    crystalParticles.addOnParticleRemovedListener(playOnEvent(SfxCrystal2));

    auto gameContent = m_scene.parentGameContent();
    gameContent->addOnFocusLostListener([this]() {
        m_player.pause();
    });
    gameContent->addOnFocusRestoredListener([this]() {
        m_player.resume();
    });

    m_scene.addOnControllerEventListener([this](const Controller::Event &event) {
        if (event == StartController::EventType::Start) {
            Log::warn("AudioManager", "TODO: play start theme here"); // TODO
        } else if (event == GameController::EventType::Game) {
            Log::warn("AudioManager", "TODO: play game theme here"); // TODO
        }
    });
}

inline static AudioPlayer::Params getParamsFor(AudioManager::Audio audio) {
    switch (audio) {
        case AudioManager::Audio::GameTheme: {
            return {
                .id = audio,
                .type = AudioPlayer::Source::Type::Music,
                .loop = true
            };
        }
        default: {
            return {
                .id = audio,
                .type = AudioPlayer::Source::Type::SFX,
                .loop = false
            };
        }
    }
}

LoaderConfig AudioManager::resourceLoaderConfig() {
    return LoaderConfig::create<AudioManager>({
        .loader = [this]() {
            m_player.load("audio/GameTheme.ogg", getParamsFor(AudioManager::GameTheme));
            m_player.load("audio/SfxTakeoff.ogg", getParamsFor(AudioManager::SfxTakeoff));
            m_player.load("audio/SfxTouchdown.ogg", getParamsFor(AudioManager::SfxTouchdown));
            m_player.load("audio/SfxButton1.ogg", getParamsFor(AudioManager::SfxButton1));
            m_player.load("audio/SfxScroll.ogg", getParamsFor(AudioManager::SfxScroll));
            m_player.load("audio/SfxCrystal1.ogg", getParamsFor(AudioManager::SfxCrystal1));
            m_player.load("audio/SfxCrystal2.ogg", getParamsFor(AudioManager::SfxCrystal2));
        }
    });
}
