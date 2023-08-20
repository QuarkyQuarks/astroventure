#include "LoadingLabel.h"

#include <algine/core/painter/Painter.h>
#include <algine/core/Engine.h>
#include <algine/core/TypeRegistry.h>

#include <glm/gtc/constants.hpp>

#include <locale>
#include <codecvt>

STATIC_INITIALIZER_IMPL(LoadingLabel) {
    alRegisterType(LoadingLabel);
}

void LoadingLabel::onMeasure(int &width, int &height) {
    Label::onMeasure(width, height);

    // to UTF-8 / UTF-16
    // we need this conversion since not in all languages
    // 1 symbol = 1 byte
    m_utfText = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(m_text);

    m_mainBlockWidth = m_fontMetrics.boundingRect({m_utfText.c_str(), m_utfText.size() - m_animatedCount}).getWidth();

    // see FontMetrics::boundingRect
    auto last = m_utfText[m_utfText.size() - m_animatedCount - 1];
    m_mainBlockWidth += m_fontMetrics.horizontalAdvance(last);
    m_mainBlockWidth -= m_fontMetrics.leftHorizontalBearing(last) + m_fontMetrics.width(last);
}

void LoadingLabel::onDraw(Painter &painter) {
    // Yes - if we will animate only dots (i.e. `.`),
    // we need utf string only for onMeasure()
    // But for example in japanese `。` is used instead of `.`,
    // so we need utf string for onDraw() too
    // auto &m_utfText = m_text;

    painter.setFont(getFont(), getFontSize());
    painter.setPaint(Paint(m_fontColor));

    painter.drawText({m_utfText.c_str(), m_utfText.size() - m_animatedCount}, {m_textX, m_textY});

    auto x = m_textX + (float) m_mainBlockWidth;

    for (int i = 0; i < m_animatedCount; ++i) {
        auto index = m_utfText.size() - m_animatedCount + i;

        painter.setOpacity(sinf((float) Engine::timeFromStart() / 300 - glm::pi<float>() / 6 * (float) i) * 0.5f + 0.5f);
        painter.drawText({m_utfText.c_str() + index, 1}, {x, m_textY});

        x += (float) m_fontMetrics.horizontalAdvance(m_utfText[index]);
    }
}
