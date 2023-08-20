#ifndef ASTROVENTURE_SETTINGSMANAGER_H
#define ASTROVENTURE_SETTINGSMANAGER_H

#include <algine/core/Resources.h>
#include <algine/core/Ptr.h>

using namespace algine;

class SettingsManager: public Resources {
public:
    constexpr static auto HighScore = "high_score";
    constexpr static auto Crystals = "crystals";
    constexpr static auto CurrentSpaceshipId = "spaceship_id";

public:
    SettingsManager();

    void load();
    void save();

    int getHighScore() const;
    int getCrystals() const;

    int getCurrentSpacecraftId() const;

private:
    int getIntValue(std::string_view name) const;

private:
    Ptr<IOSystem> m_io;
};

#endif //ASTROVENTURE_SETTINGSMANAGER_H
