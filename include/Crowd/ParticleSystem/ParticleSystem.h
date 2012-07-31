#pragma once

#include "ParticleSystem/BaseParticle.h"
#include "ParticleSystem/BaseParticleForce.h"
#include "ParticleSystem/BaseParticleSpring.h"

enum
{
	PARTICLE_BORDER_DIE,
	PARTICLE_BORDER_BOUNCE,
	PARTICLE_BORDER_JUMP_TO_OPPOSITE
};

class ParticleSystem
{
public:

	ParticleSystem();
	~ParticleSystem();

	void update();
	void draw();
	void clear();

	void borders();
	void collision();

	void setBoundingBox( ci::Rectd bbox);
	void setBorderEvent(int BORDER_EVENT);

	void add(BaseParticle * particle);
	void add(BaseParticleForce * force);
	void add(BaseParticleSpring * spring);

	BaseParticle * getParticle(float p);
	BaseParticle * getParticleAt(int n);

	int getNumOfParticles();

	void clampVelocity(BaseParticle * p);
	void deleteSpring(BaseParticle * p);

	std::vector<BaseParticle*> particles;
	std::vector<BaseParticleForce*> forces;
	std::vector<BaseParticleSpring*> springs;

	ci::Rectd bbox;
	bool hasBbox;
	bool hasCollision;
	int borderEvent;

	float maxVelocity;

	int iteration;
};
