#ifndef SPACE_EXPLORER_SPACECRAFT_H
#define SPACE_EXPLORER_SPACECRAFT_H

#include "game/base/SpaceModel.h"

using namespace algine;

class Spacecraft: public SpaceModel {
public:
    explicit Spacecraft(int id, Object *parent = defaultParent()); // TODO

    int getId() const;

private:
    int m_id;
};

#endif //SPACE_EXPLORER_SPACECRAFT_H
