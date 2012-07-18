#include "ParticleSystem/ForceAttractor.h"

using namespace ci;

ForceAttractor::ForceAttractor(float _force, float _area)
{
    force = _force;
    area = _area;
}

ForceAttractor::~ForceAttractor()
{

}

void ForceAttractor::update()
{

}

Vec2f ForceAttractor::getForce(BaseParticle * p)
{
    if (fabsf(center.x - p->position.x) < area && fabsf(center.y - p->position.y) < area)
    {
        float d = ( center - p->position ).length();

        if (d < area)
        {
            float dx = (p->position.x - center.x) / d;
            float dy = (p->position.y - center.y) / d;

            return Vec2f(dx * (area - d) * force, dy * (area - d) * force);
        }
    }
    return Vec2f();
}
