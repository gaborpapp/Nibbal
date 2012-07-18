#pragma once

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

		void Init();
        void setup(int width, int height);
        void addPerson(CPerson cp);
        void update();
        void draw();

        std::vector<ci::ImageSourceRef> getImages( std::string dir );
        ci::ImageSourceRef  getRandomImage( std::vector<ci::ImageSourceRef> *list );
		ci::ImageSourceRef _loadImage( std::string strImageName );

        void energize( float e );
        void wave( float e );

        int stepX, stepY;
        int width, height;

        std::vector<CPerson> people;
        ParticleSystem psys;

        std::vector<ci::ImageSourceRef> heads;
        std::vector<ci::ImageSourceRef> torsos;
        std::vector<ci::ImageSourceRef> arms;
        std::vector<ci::ImageSourceRef> legs;
};
