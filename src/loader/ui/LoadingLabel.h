#ifndef ASTROVENTURE_LOADINGLABEL_H
#define ASTROVENTURE_LOADINGLABEL_H

#include <algine/core/widgets/Label.h>

using namespace algine;

class LoadingLabel: public Label {
    STATIC_INITIALIZER_DECL

protected:
    void onMeasure(int &width, int &height) override;
    void onDraw(Painter &painter) override;

private:
    std::u16string m_utfText;
    int m_mainBlockWidth {};
    int m_animatedCount {3};
};

#endif //ASTROVENTURE_LOADINGLABEL_H
