#include "ParticleSystem/BaseParticleForce.h"

using namespace ci;

BaseParticleForce::BaseParticleForce()
{

}

BaseParticleForce::~BaseParticleForce()
{

}

void BaseParticleForce::setup(Vec2f _force, Rectd _bbox)
{
	force = _force;
	bbox = _bbox;
}

void BaseParticleForce::update()
{

}

void BaseParticleForce::apply(BaseParticle * p)
{
	if (p->position.x >= bbox.x1 && p->position.x <= bbox.x2 && p->position.y >= bbox.y1 && p->position.y <= bbox.y2 )
	{
		p->movement += getForce(p);
	}
}

Vec2f BaseParticleForce::getForce(BaseParticle * p)
{
	return force;
}
