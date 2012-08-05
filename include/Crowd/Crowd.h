#pragma once

#include <vector>

#include "cinder/Filesystem.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"
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

	int getNumVariants() { return mNumVariants; }
	void setNumVariants( int variants ) { mNumVariants = variants; }
	void setCrowdSeed( uint32_t seed ) { mCrowdSeed = seed; }

protected:
	void _updateFbo();

	int stepX, stepY;
	int width, height;

	std::vector<CPerson> people;
	ParticleSystem psys;

	std::vector<ci::ImageSourceRef> heads;
	std::vector<ci::ImageSourceRef> torsos;
	std::vector<ci::ImageSourceRef> arms;
	std::vector<ci::ImageSourceRef> legs;

	ci::gl::Fbo mFbo;

	int mNumVariants;
	uint32_t mCrowdSeed;
	ci::Rand mRand;

	std::vector< ci::gl::Fbo > mPersonFbos;
};
