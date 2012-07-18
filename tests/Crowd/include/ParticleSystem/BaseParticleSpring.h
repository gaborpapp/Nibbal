#pragma once

#include "ParticleSystem/BaseParticle.h"

class BaseParticleSpring
{
    public:
        BaseParticleSpring();
        ~BaseParticleSpring();

        void setup(BaseParticle * p1, BaseParticle * p2, float springDamp);
        void update();
        void baseDraw();
        virtual void draw(){};

        BaseParticle * p1;
        BaseParticle * p2;

        float springDamp;
        float distance;
};

