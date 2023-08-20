#include "SettingsManager.h"

#include <algine/core/io/StandardIOSystem.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

#include <tulz/Path.h>

constexpr auto settingsFile = "settings.xml";

#ifdef __ANDROID__
inline auto settingsFilePath() {
    return tulz::Path::join(Engine::Android::getAppDataDirectory(), settingsFile);
}
#else
inline auto settingsFilePath() {
    return settingsFile;
}
#endif

SettingsManager::SettingsManager()
    : m_io(PtrMaker::make<StandardIOSystem>()) {}

void SettingsManager::load() {
    if (auto path = settingsFilePath(); m_io->exists(path)) {
        fromXMLFile(path, m_io);
    }
}

void SettingsManager::save() {
    auto stream = m_io->open(settingsFilePath(), IOStream::Mode::WriteText);
    stream->writeStr(SettingsManager::toXMLString());
    stream->flush();
    stream->close();
}

int SettingsManager::getHighScore() const {
    return getIntValue(HighScore);
}

int SettingsManager::getCrystals() const {
    return getIntValue(Crystals);
}

int SettingsManager::getCurrentSpacecraftId() const {
    return getIntValue(CurrentSpaceshipId);
}

int SettingsManager::getIntValue(std::string_view name) const {
    if (hasInt(name.data())) {
        return getInt(name.data());
    } else {
        return 0;
    }
}
