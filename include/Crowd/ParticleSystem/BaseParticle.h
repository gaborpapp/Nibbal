#pragma once

#include "cinder/vector.h"

class BaseParticle
{
public:

	BaseParticle();
	~BaseParticle();

	void setup( ci::Vec2f p, ci::Vec2f m, float size, bool _fixed = false, float _friction = 0.98, float _collisionDamp = 2);
	void baseUpdate();

	virtual void update() {};
	virtual void draw();
	virtual void collisionEvent(){};
	virtual void otherCollisionEvent(){};

	virtual void kill();
	float getDirection();
	void collide(BaseParticle * p);
	void setPosition(float x, float y);

	void moveTo( ci::Vec2f targetPoint, float movementDamp);
	void moveAway( ci::Vec2f targetPoint, float dist, float motionDamp);
	void addPos( ci::Vec2f vec );

	bool live;
	bool fixed;
	float size;
	float originalSize;

	ci::Vec2f position;
	ci::Vec2f originalPosition;
	ci::Vec2f movement;
	float friction;
	float collisionDamp;

	int intUserData;
};
