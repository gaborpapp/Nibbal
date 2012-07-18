#pragma once

#include "ParticleSystem/BaseParticle.h"

class ParticleEmpty : public BaseParticle
{
    public:
        ParticleEmpty(float x, float y, float friction, bool fixed);
        ~ParticleEmpty();

        void draw();

};
