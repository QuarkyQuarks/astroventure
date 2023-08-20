#include "FPSMeter.h"

#include <algine/core/Engine.h>

using namespace algine;

FPSMeter::FPSMeter(int sampleLengthMs)
    : m_sampleLengthMs(sampleLengthMs),
      m_prevTime(),
      m_frameCount() {}

void FPSMeter::tick() {
    auto currentTime = Engine::timeFromStart();

    m_frameCount++;

    if (currentTime - m_prevTime >= m_sampleLengthMs) {
        float frameTime = (float) (currentTime - m_prevTime) / (float) m_frameCount;

        m_onDataReady.notify(Data {
            .framesPerSecond = (1.0f / frameTime) * 1000.0f,
            .timePerFrame = frameTime * 1000.0f
        });

        m_frameCount = 0;
        m_prevTime = currentTime;
    }
}

FPSMeter::Subject::Subscription_t FPSMeter::addOnDataReadyListener(const Subject::Observer_t &listener) {
    return m_onDataReady.subscribe(listener);
}
