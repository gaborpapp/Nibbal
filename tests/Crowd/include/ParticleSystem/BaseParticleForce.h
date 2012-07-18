#pragma once

#include "cinder/Rect.h"
#include "cinder/Vector.h"

#include "ParticleSystem/BaseParticle.h"

class BaseParticleForce
{
    public:
        BaseParticleForce();
        ~BaseParticleForce();

        void setup( ci::Vec2f force, ci::Rectd bbox);
        virtual void update();

        void apply(BaseParticle * p);

        virtual ci::Vec2f getForce(BaseParticle * p);

        ci::Vec2f force;
        ci::Rectd bbox;

};
