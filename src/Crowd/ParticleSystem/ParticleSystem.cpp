#include "cinder/gl/gl.h"
#include "ParticleSystem/ParticleSystem.h"

using namespace ci;
using namespace std;


ParticleSystem::ParticleSystem()
{
	hasBbox = hasCollision = false;
	maxVelocity = 20;
	iteration = 2;
}

ParticleSystem::~ParticleSystem()
{
	clear();
}

void ParticleSystem::add(BaseParticle * particle)
{
	particles.push_back(particle);
}

void ParticleSystem::add(BaseParticleForce * force)
{
	forces.push_back(force);
}

void ParticleSystem::add(BaseParticleSpring * spring)
{
	springs.push_back(spring);
}

void ParticleSystem::clear()
{
	for (vector<BaseParticle*>::iterator p = particles.begin(); p != particles.end(); p++)
	{
		delete (*p);
	}
	particles.clear();
}

void ParticleSystem::setBoundingBox( Rectd _bbox)
{
	hasBbox = true;
	bbox = _bbox;
}

void ParticleSystem::setBorderEvent(int BORDER_EVENT)
{
	borderEvent = BORDER_EVENT;
}

void ParticleSystem::update()
{
	for (int iter = 0; iter < iteration; iter++)
	{
		for (vector<BaseParticleSpring*>::iterator i = springs.begin(); i != springs.end(); i++)
		{
			(*i)->update();
		}

		for (vector<BaseParticleForce*>::iterator f = forces.begin(); f != forces.end(); f++)
		{
			(*f)->update();
		}

		for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end();)
		{
			if ((*i)->live)
			{
				(*i)->baseUpdate();
				if (!(*i)->fixed)
				{
					for (vector<BaseParticleForce*>::iterator f = forces.begin(); f != forces.end(); f++)
					{
						(*f)->apply(*i);
					}

					(*i)->position += (*i)->movement;

					clampVelocity(*i);
					(*i)->movement *= (*i)->friction;
				}

				i++;
			}
			else
			{
				deleteSpring(*i);
				delete (*i);
				i = particles.erase(i);
			}
		}

		if (hasCollision) collision();
		if (hasBbox) borders();
	}
}

void ParticleSystem::clampVelocity(BaseParticle * p)
{
	p->movement.x = math<float>::min( math<float>::max( p->movement.x, -maxVelocity ), maxVelocity );
	p->movement.y = math<float>::min( math<float>::max( p->movement.y, -maxVelocity ), maxVelocity );
}

void ParticleSystem::deleteSpring(BaseParticle * p)
{
	for (vector<BaseParticleSpring*>::iterator i = springs.begin(); i != springs.end();)
	{
		if ((*i)->p1 == p || (*i)->p2 == p)
		{
			delete (*i);
			i = springs.erase(i);
		}
		else i++;
	}
}

BaseParticle * ParticleSystem::getParticle(float p)
{
	if (particles.size() == 0) return NULL;
	p = math<float>::min( math<float>::max( p, 0 ), 1 );
	int n = (int)(p * (particles.size() - 1));
	vector<BaseParticle*>::iterator i = particles.begin() + n;
	return (*i);
}

BaseParticle * ParticleSystem::getParticleAt(int n)
{
	if (particles.size() == 0) return NULL;
	n = math<int>::min( math<int>::max( n, 0 ), particles.size() - 1 );
	vector<BaseParticle*>::iterator i = particles.begin() + n;
	return (*i);
}

int ParticleSystem::getNumOfParticles()
{
	return particles.size();
}

void ParticleSystem::borders()
{
	if (borderEvent == PARTICLE_BORDER_DIE)
	{
		for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end();i++)
		{
			if ((*i)->position.x < bbox.x1 - (*i)->size ||
				(*i)->position.x > bbox.x2 + (*i)->size ||
				(*i)->position.y < bbox.y1 - (*i)->size ||
				(*i)->position.y > bbox.y2 + (*i)->size)
			{
				(*i)->kill();
			}
		}
	}
	else if (borderEvent == PARTICLE_BORDER_BOUNCE)
	{
		for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end();i++)
		{
			if ((*i)->position.x < (float)bbox.x1 - (*i)->size)
			{
				(*i)->position.x = (float)bbox.x1 - (*i)->size;
				(*i)->movement.x *= -1;
			}
			else if ((*i)->position.x > (float)bbox.x2 + (*i)->size)
			{
				(*i)->position.x = (float)bbox.x2 + (*i)->size;
				(*i)->movement.x *= -1;
			}
			if ((*i)->position.y < (float)bbox.y1 - (*i)->size)
			{
				(*i)->position.y = (float)bbox.y1 - (*i)->size;
				(*i)->movement.y *= -1;
			}
			else if ((*i)->position.y > (float)bbox.y2 + (*i)->size)
			{
				(*i)->position.y = (float)bbox.y2 + (*i)->size;
				(*i)->movement.y *= -1;
			}
		}
	}
	else if (borderEvent == PARTICLE_BORDER_JUMP_TO_OPPOSITE)
	{
		for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end();i++)
		{
			if ((*i)->position.x < (float)bbox.x1 - (*i)->size)
			{
				(*i)->position.x = (float)bbox.x2 + (*i)->size;
			}
			else if ((*i)->position.x > (float)bbox.x2 + (*i)->size)
			{
				(*i)->position.x = (float)bbox.x1 - (*i)->size;
			}
			else if ((*i)->position.y < (float)bbox.y1 - (*i)->size)
			{
				(*i)->position.y = (float)bbox.y2 + (*i)->size;
			}
			else if ((*i)->position.y > (float)bbox.y2 + (*i)->size)
			{
				(*i)->position.y = (float)bbox.y1 - (*i)->size;
			}
		}
	}

}

void ParticleSystem::collision()
{
	for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end();i++)
	{
		for (vector<BaseParticle*>::iterator j = particles.begin(); j != particles.end();j++)
		{
			(*i)->collide(*j);
		}
	}
}

void ParticleSystem::draw()
{
// 	gl::color( ColorA( 0.0f, 0.0f, 1.0f, 1.0f ));
// 
// 	for (vector<BaseParticleSpring*>::iterator i = springs.begin(); i != springs.end(); i++)
// 	{
// 		(*i)->baseDraw();
// 	}
// 
// 	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ));
// 
// 	for (vector<BaseParticle*>::iterator i = particles.begin(); i != particles.end(); i++)
// 	{
// 		(*i)->draw();
// 	}
}
