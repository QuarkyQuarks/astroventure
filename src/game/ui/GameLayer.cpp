#include "GameLayer.h"
#include "AnimTools.h"
#include "ButtonAnimation.h"
#include "game/GameScene.h"
#include "game/GameContent.h"
#include "game/math/VecTools.h"

#include <algine/core/widgets/Container.h>
#include <algine/core/widgets/Label.h>
#include <algine/core/widgets/animation/ColorAnimation.h>
#include <algine/core/widgets/animation/CubicSplineInterpolator.h>

namespace UI {
GameLayer::GameLayer(GameUIScene *scene)
    : Layer(scene)
{
    auto container = Widget::constructFromXMLFile<Container*>("ui/GameContent.xml", this);
    setContainer(container);

    container->setEventListener(Event::Click, [scene](Widget*, const Event &event) {
        auto controller = scene->parentGameScene()->getController();
        controller->event(event);
    });

    auto pause = container->findChild<Widget*>("pause");
    pause->setEventListener(Event::Click, [scene](Widget*, const Event&) {
        scene->pause->show();
    });

    AnimTools::setButtonAnimation(pause);

    m_labelScore = container->findChild<Label*>("score");
    m_labelCrystals = container->findChild<Label*>("crystals");

    m_scoreContainer = container->findChild<Container*>("score_container");
    m_crystalsContainer = container->findChild<Container*>("crystals_container");

    auto gameScene = scene->parentGameScene();

    gameScene->getScore().subscribe([this](int score) {
        scoreChanged();
    });

    gameScene->getCrystals().subscribe([this](int crystals) {
        crystalsChanged();
    });

    parentGameUIScene()->addOnSizeChangedListener([this](int width, int height) {
        calcCrystalsEndPoint(static_cast<float>(width), static_cast<float>(height));
    });
}

void GameLayer::calcCrystalsEndPoint(float width, float height) {
    auto container = getContainer();
    container->forceLayout();

    // calculate absolute position on the screen
    // x -> [0, width()] -> right
    // y -> [0, height()] -> down
    auto crystalsImage = container->findChild<Widget*>("crystals_image");
    auto crystalsPoint = crystalsImage->mapToGlobal({0, 0});

    // x -> [0, 1] -> right
    // y -> [0, 1] -> down
    glm::vec2 scrNormalized {
        (float) (crystalsPoint.getX() + crystalsImage->getWidth() / 2) / width,
        (float) (crystalsPoint.getY() + crystalsImage->getHeight() / 2) / height
    };

    // x -> [-1, 1] -> right
    // y -> [-1, 1] -> up
    glm::vec2 scrCentered {
        2 * scrNormalized.x - 1,
        -(2 * scrNormalized.y - 1)
    };

    glm::vec2 endPoint = mapScreenToWorld(
        glm::perspective(
            Cameraman::FieldOfView,
            width / height,
            Cameraman::Near, Cameraman::Far),
        scrCentered,
        -Cameraman::GamePos.z);

    auto gameScene = parentGameUIScene()->parentGameScene();
    gameScene->getCrystalParticles().setEndPoint({endPoint, 0.0f});
}

class ScoreAnimation: public Widgets::ColorAnimation {
public:
    explicit ScoreAnimation(Label *score, GameLayer *layer)
        : m_score(score),
          m_layer(layer)
    {
        setInterpolator(AnimTools::fadeInOutInterp());
        setDuration(600);
    }

protected:
    void onStart() override {
        m_half = false;

        // TODO: color themes & move to ctor
        auto &color = getWidget()->getBackground().getColor();
        setFrom(Color::fromRgbF(color.redF(), color.greenF(), color.blueF(), 0.53f));
        setTo(Color::fromRgbF(color.redF(), color.greenF(), color.blueF(), 0.85f));
    }

    void onAnimate() override {
        Widgets::ColorAnimation::onAnimate();

        if (getPos() >= 0.5f) {
            if (!m_half) {
                auto score = *m_layer->parentGameUIScene()->parentGameScene()->getScore();
                m_score->setText(std::to_string(score));
                m_half = true;
            }
        }
    }

private:
    bool m_half = false;
    Label *m_score;
    GameLayer *m_layer;
};

// TODO: color theme!

void GameLayer::reset() {
    m_labelScore->setText("0");
    m_labelCrystals->setText("0");
}

void GameLayer::scoreChanged() {
    m_scoreContainer->addAnimation(std::make_unique<ScoreAnimation>(m_labelScore, this));
}

void GameLayer::crystalsChanged() {
    if (!m_crystalsContainer->getAnimations().empty()) {
        auto &anim = m_crystalsContainer->getAnimations().front();

        if (auto pos = anim->getPos(); pos > 0.5f) {
            anim->setPos(1.0f - pos);
        }
    } else {
        auto &color = m_crystalsContainer->getBackground().getColor();
        Color c1 = Color::fromRgbF(color.redF(), color.greenF(), color.blueF(), 0.53f);
        Color c2 = Color::fromRgbF(color.redF(), color.greenF(), color.blueF(), 0.85f);

        m_crystalsContainer->addAnimation(
            Widgets::Animation::create<Widgets::ColorAnimation>(c1, c2, AnimTools::fadeInOutInterp()));
    }

    auto crystals = *parentGameUIScene()->parentGameScene()->getCrystals();
    m_labelCrystals->setText(std::to_string(crystals));
}
} // UI
