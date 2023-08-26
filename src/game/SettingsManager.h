#ifndef ASTROVENTURE_SETTINGSMANAGER_H
#define ASTROVENTURE_SETTINGSMANAGER_H

#include <algine/core/Resources.h>
#include <algine/core/Ptr.h>

using namespace algine;

class GameScene;

class SettingsManager: public Resources {
public:
    constexpr static auto HighScore = "high_score";
    constexpr static auto Crystals = "crystals";
    constexpr static auto CurrentSpaceshipId = "spaceship_id";

public:
    explicit SettingsManager(GameScene &scene);

    void load();
    void save();

    /**
     * Updates & saves the progress (crystal count & score)
     */
    void saveProgress();

    int getHighScore() const;
    int getCrystals() const;

    int getCurrentSpacecraftId() const;

private:
    int getIntValue(std::string_view name) const;

private:
    Ptr<IOSystem> m_io;
    GameScene &m_scene;
};

#endif //ASTROVENTURE_SETTINGSMANAGER_H
