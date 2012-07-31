#pragma once

#include "cinder/gl/Texture.h"
#include "ParticleSystem/ParticleSystem.h"

class CPerson
{
public:
	CPerson();
	~CPerson();

	void setup(ParticleSystem * psys, ci::Vec3f center, ci::ImageSourceRef  _head, ci::ImageSourceRef  _torso, ci::ImageSourceRef  _arm, ci::ImageSourceRef  _leg);
	void update();
	void draw();
	void drawLimb(BaseParticle * a, BaseParticle * b, ci::gl::Texture img );
	void energize(float e, int delay = 0);
	void normalize();

	ParticleSystem * psys;

	float energy;

	BaseParticle * torso, * neck, * head;
	BaseParticle * l_shoulder, * r_shoulder, * l_elbow, * l_hand, * r_elbow, * r_hand;
	BaseParticle * l_hip, * l_knee, * l_foot, * r_hip, * r_knee, * r_foot;

	BaseParticle * handTarget;
	BaseParticleSpring * l_handSpring, * r_handSpring;
	float handDamp;

	BaseParticleSpring * neckSpring;

	float pulseSpeedX;
	float pulseSizeX;
	float pulseSpeedY;
	float pulseSizeY;
	float phaseX, phaseY;

	ci::Vec3f oHead;
	float tHeadY;

	int impulseStarted;
	int delay;
	float tEnergy;

	ci::Color color;

	ci::gl::Texture headImg,  torsoImg,  armImg,  legImg;
	ci::Color torsoColor, armColor, legColor;

	std::vector<BaseParticleSpring*> springs;

};
