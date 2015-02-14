#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "vvrphysicsdll.h"

namespace vvr {
namespace phys {

/**
 * An abstract class for rendering and updating
 */
class VVRPhysics_API IRenderable {
public:
    virtual ~IRenderable(){}

    /**
     * Can be rendered
     */
    virtual void draw() const = 0;

    /**
     * Can be updated
     */
    virtual void update(float t, float dt) = 0;
};

}} // end namespace vvr::phys

#endif
