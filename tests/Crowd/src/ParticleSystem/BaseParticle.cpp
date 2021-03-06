#include "cinder/app/AppBasic.h"

#include "ParticleSystem/BaseParticle.h"

using namespace ci;

BaseParticle::BaseParticle()
{
    live = true;
	friction = 0.98;
	size = originalSize = 5;
	fixed = false;
	collisionDamp = 2;
	life = -1;
	intUserData = -1;
	time = 0;
}

BaseParticle::~BaseParticle()
{

}

void BaseParticle::setup( Vec2f p, Vec2f m, float _size, int _life, bool _fixed, float _friction, float _collisionDamp)
{
    position = p;
    originalPosition = p;
    movement = m;
    size = originalSize = _size;
	time = ci::app::getElapsedSeconds() * 4;
	life = _life;
	fixed = _fixed;
	friction = _friction;
	collisionDamp = _collisionDamp;
}

void BaseParticle::baseUpdate()
{
	if (life != -1 && ci::app::getElapsedSeconds() * 1000 - time > life)
		kill();
	else
		update();
}

void BaseParticle::setPosition(float x, float y)
{
    position.x = originalPosition.x = x;
    position.y = originalPosition.y = y;
}

void BaseParticle::kill()
{
    live = false;
}

float BaseParticle::getDirection()
{
    return sqrt(movement.x * movement.x + movement.y * movement.y);
}

float BaseParticle::getLife()
{
	float tmp = (ci::app::getElapsedSeconds() * 1000 - time) / (float)life;
	return math<float>::max( math<float>::min( tmp, 1 ), 0 );
}

void BaseParticle::collide(BaseParticle * p)
{
    if (p != this)
    {
        float dst = (size + p->size);

        if (fabsf(position.x - p->position.x) < dst && fabsf(position.y - p->position.y) < dst)
        {
            float d = ( position - p->position ).length();
            if (d < dst)
            {
                float dx = (p->position.x - position.x) / d;
                float dy = (p->position.y - position.y) / d;

                movement.x += dx * (d - dst) / collisionDamp;
                movement.y += dy * (d - dst) / collisionDamp;

                p->movement.x -= dx * (d - dst) / collisionDamp;
                p->movement.y -= dy * (d - dst) / collisionDamp;

				collisionEvent();
            }
        }
    }
}

void BaseParticle::moveTo(Vec2f tp, float motionDamp)
{
    /*float d = ofDist(position.x, position.y, tp.x, tp.y);
    float dx = (tp.x - position.x) / d;
    float dy = (tp.y - position.y) / d;
    */

    movement.x += (tp.x - position.x) / motionDamp;
    movement.y += (tp.y - position.y) / motionDamp;
}

void BaseParticle::moveAway(Vec2f tp, float dist, float motionDamp)
{
    float dx = (tp.x - position.x);
    float dy = (tp.y - position.y);

    if (fabsf(dx) < dist && fabsf(dy) < dist)
    {
		float d = ( position - tp ).length();

        if (d > 0 && d < dist)
        {
            dx /= d;
            dy /= d;

            float dd = (dist - d) / motionDamp;

            movement.x -= dx * dd;
            movement.y -= dy * dd;
        }
    }
}
