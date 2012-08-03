#pragma once

#include "cinder/Filesystem.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "CPerson.h"
#include "ParticleSystem/ParticleSystem.h"

enum
{
	LIMB_HEAD,
	LIMB_TORSO,
	LIMB_ARM,
	LIMB_LEG
};

class Crowd
{
public:
	Crowd();
	~Crowd();

	void Init( const ci::fs::path &crowdFolder );
	void setup( const ci::fs::path &crowdFolder, ci::Vec2i size );
	void addPerson(CPerson cp);
	void update();
	ci::gl::Texture &getTexture() { return mFbo.getTexture(); }

	std::vector<ci::ImageSourceRef> getImages( ci::fs::path folder );
	ci::ImageSourceRef  getRandomImage( std::vector<ci::ImageSourceRef> *list );

	void energize( float e );
	void wave( float e );

protected:
	void _updateFbo();

protected:
	int stepX, stepY;
	int width, height;

	std::vector<CPerson> people;
	ParticleSystem psys;

	std::vector<ci::ImageSourceRef> heads;
	std::vector<ci::ImageSourceRef> torsos;
	std::vector<ci::ImageSourceRef> arms;
	std::vector<ci::ImageSourceRef> legs;

	ci::gl::Fbo mFbo;
};