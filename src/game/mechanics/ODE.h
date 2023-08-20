#ifndef ASTROVENTURE_ODE_H
#define ASTROVENTURE_ODE_H

class ODE {
public:
    static inline float linear(float p, float h) {
        float k1 = p;
        float k2 = p * k1 * (h / 2);
        float k3 = p * k2 * (h / 2);
        float k4 = p * k3 * h;
        return rk4(k1, k2, k3, k4, h);
    }

    template<auto ptr, typename T>
    static float gravity(const T &v, float h) {
        float val = v.*ptr;
        float k1 = -massTimesG * powf(length(v), -3) * val;
        float k2 = -massTimesG * powf(length(v + T(k1 * h / 2)), -3) * (val + k1 * h / 2);
        float k3 = -massTimesG * powf(length(v + T(k2 * h / 2)), -3) * (val + k2 * h / 2);
        float k4 = -massTimesG * powf(length(v + T(k3 * h)), -3) * (val + k3 * h);
        return rk4(k1, k2, k3, k4, h);
    }

private:
    static inline float rk4(float k1, float k2, float k3, float k4, float h) {
        return 1.0f / 6 * h * (k1 + 2 * k2 + 2 * k3 + k4);
    }

private:
    constexpr static float massTimesG = 0.15f;
};

#endif //ASTROVENTURE_ODE_H

