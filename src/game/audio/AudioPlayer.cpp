#include "AudioPlayer.h"

#include <algine/core/Engine.h>

using namespace algine;

AudioPlayer::AudioPlayer() {
    m_soloud.setMaxActiveVoiceCount(64);
    m_soloud.init();
    m_soloud.play(m_musicBus);
    m_soloud.play(m_sfxBus);
}

void AudioPlayer::pause() {
    m_soloud.setPauseAll(true);
}

void AudioPlayer::resume() {
    m_soloud.setPauseAll(false);
}

SoLoud::Wav& AudioPlayer::load(std::string_view path, const Params &params) {
    assert(params.id >= 0);

    auto stream = Engine::getDefaultIOSystem()->open(path.data(), IOStream::Mode::Read);
    auto size = stream->size();
    auto buffer = new unsigned char[size];
    stream->read(buffer, size, 1);
    stream->close();

    SoLoud::Wav &wave = (m_sources[params.id] = {
        .wave = SoLoud::Wav(),
        .type = params.type,
        .volume = params.volume
    }).wave;
    wave.loadMem(buffer, size); // Load a wave file & pass ownership of the buffer to SoLoud
    wave.setLooping(params.loop);

    return wave;
}

SoLoud::handle AudioPlayer::play(int id) {
    auto &source = m_sources[id];
    return getAudioBus(source.type).play(source.wave, source.volume);
}

SoLoud::Soloud& AudioPlayer::getAudioEngine() {
    return m_soloud;
}

SoLoud::Bus& AudioPlayer::getAudioBus(Source::Type type) {
    switch (type) {
        case Source::Type::Music: return m_musicBus;
        case Source::Type::SFX: return m_sfxBus;
        default: throw std::invalid_argument("Invalid type: " + std::to_string(static_cast<int>(type)));
    }
}

AudioPlayer::Source& AudioPlayer::getSource(int id) {
    return m_sources[id];
}
