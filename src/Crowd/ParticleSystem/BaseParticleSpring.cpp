#include "cinder/gl/gl.h"
#include "ParticleSystem/BaseParticleSpring.h"

using namespace ci;

BaseParticleSpring::BaseParticleSpring()
{
	p1 = NULL;
	p2 = NULL;
}

BaseParticleSpring::~BaseParticleSpring()
{

}

void BaseParticleSpring::setup(BaseParticle * _p1, BaseParticle * _p2, float _springDamp )
{
	p1 = _p1;
	p2 = _p2;
	if (p1 != NULL && p2 != NULL)
	{
		springDamp = _springDamp;
		distance = ( p1->position - p2->position ).length();
	}
}
void BaseParticleSpring::update()
{
	if (p1 == NULL || p2 == NULL) return;

	float d = ( p1->position - p2->position ).length();

	if (d == 0) return;

	float dx = (p2->position.x - p1->position.x) / d;
	float dy = (p2->position.y - p1->position.y) / d;

	if (!p1->fixed)
	{
		p1->movement.x -= dx * (distance - d) / springDamp;
		p1->movement.y -= dy * (distance - d) / springDamp;
	}
	if (!p2->fixed)
	{
		p2->movement.x += dx * (distance - d) / springDamp;
		p2->movement.y += dy * (distance - d) / springDamp;
	}

}

void BaseParticleSpring::baseDraw()
{
	if (p1 != NULL && p2 != NULL) draw();
}

void BaseParticleSpring::draw()
{
//	gl::drawLine( p1->position, p2->position );
}

Vec2f BaseParticleSpring::normalize()
{
	if( p1->fixed && p2->fixed )
		return Vec2f( 0, 0 );

	BaseParticle *particleBase = p1;
	BaseParticle *particleNorm = p2;

	if( p2->fixed )
	{
		particleBase = p2;
		particleNorm = p1;
	}

	Vec2f direction = particleNorm->position - particleBase->position;
	float d = direction.length();

	if( d != distance )
	{
		direction.normalize();
		Vec2f posNorm = particleNorm->position;
		particleNorm->position = particleBase->position + direction * distance;

		direction = particleNorm->position - particleBase->position;
		d = direction.length();

		return Vec2f( posNorm - particleNorm->position );
	}

	return Vec2f( 0, 0 );
}