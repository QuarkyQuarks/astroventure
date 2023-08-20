#ifndef ASTROVENTURE_AUDIOMANAGER_H
#define ASTROVENTURE_AUDIOMANAGER_H

#include "loader/Loadable.h"
#include "AudioPlayer.h"

class GameScene;

// Listens to game events & plays corresponding audio
class AudioManager: public Loadable {
public:
    enum Audio {
        GameTheme,
        SfxTouchdown,
        SfxTakeoff,
        SfxButton1,
        SfxScroll,
        SfxCrystal1,
        SfxCrystal2
    };

public:
    explicit AudioManager(GameScene &scene);

    LoaderConfig resourceLoaderConfig() override;

private:
    GameScene &m_scene;
    AudioPlayer m_player;
};


#endif //ASTROVENTURE_AUDIOMANAGER_H
