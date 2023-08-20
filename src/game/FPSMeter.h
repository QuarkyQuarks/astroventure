#ifndef ASTROVENTURE_FPSMETER_H
#define ASTROVENTURE_FPSMETER_H

#include <tulz/observer/Subject.h>

class FPSMeter {
public:
    /**
     * Stores the average frame count and
     * the average time for 1 frame.
     */
    struct Data {
        float framesPerSecond;
        float timePerFrame;
    };

    using Subject = tulz::Subject<Data>;

public:
    explicit FPSMeter(int sampleLengthMs = 1000);

    void tick();

    Subject::Subscription_t addOnDataReadyListener(const Subject::Observer_t &listener);

private:
    Subject m_onDataReady;

private:
    int m_sampleLengthMs;
    long m_prevTime;
    size_t m_frameCount;
};

#endif //ASTROVENTURE_FPSMETER_H
