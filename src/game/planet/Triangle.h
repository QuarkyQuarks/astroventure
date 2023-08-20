#ifndef ASTROVENTURE_TRIANGLE_H
#define ASTROVENTURE_TRIANGLE_H

class Triangle {
public:
    Triangle() = default;
    inline Triangle(int v1, int v2, int v3)
        : v1(v1), v2(v2), v3(v3) {}

public:
    int v1;
    int v2;
    int v3;
};

#endif //ASTROVENTURE_TRIANGLE_H
