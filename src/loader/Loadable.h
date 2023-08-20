#ifndef ASTROVENTURE_LOADABLE_H
#define ASTROVENTURE_LOADABLE_H

#include "LoaderConfig.h"

/**
 * Represents Loadable class, i.e. class
 * that will be loaded before game starts.
 * @note in order to be able to use Loadable
 * as dependency to another Loadable it must
 * be *loadable* (it must have loader).
 */
class Loadable {
public:
    virtual LoaderConfig resourceLoaderConfig() = 0;
};

#endif //ASTROVENTURE_LOADABLE_H
