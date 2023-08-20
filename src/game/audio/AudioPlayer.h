#ifndef ASTROVENTURE_AUDIOPLAYER_H
#define ASTROVENTURE_AUDIOPLAYER_H

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_bus.h"

#include <string_view>
#include <unordered_map>

class AudioPlayer {
public:
    struct Source {
        enum class Type {
            None, Music, SFX
        };

        SoLoud::Wav wave;
        Type type;
        float volume;
    };

    struct Params {
        int id {-1};
        Source::Type type {Source::Type::None};
        float volume {1.0f};
        bool loop {false};
    };

public:
    AudioPlayer();

    void pause();
    void resume();

    SoLoud::Wav& load(std::string_view path, const Params &params);
    SoLoud::handle play(int id);

    SoLoud::Soloud& getAudioEngine();
    SoLoud::Bus& getAudioBus(Source::Type type);
    Source& getSource(int id);

private:
    SoLoud::Soloud m_soloud;
    SoLoud::Bus m_musicBus;
    SoLoud::Bus m_sfxBus;
    std::unordered_map<int, Source> m_sources;
};

#endif //ASTROVENTURE_AUDIOPLAYER_H
