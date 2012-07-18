#pragma once

#include "ParticleSystem/BaseParticleForce.h"

class ForceAttractor : public BaseParticleForce
{
    public:
        ForceAttractor(float _force, float _area);
        ~ForceAttractor();

        void update();
        ci::Vec2f getForce(BaseParticle * p);

        float force;
        float area;
        ci::Vec2f center;
};

