#include <vector>

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "Crowd.h"
#include "CPerson.h"


using namespace ci;
using namespace ci::app;
using namespace std;

static const int TOP_MARGIN    = -120;
static const int BOTTOM_MARGIN = 250;
static const int LEFT_MARGIN   = 20;
static const int RIGHT_MARGIN  = 0;

static const int MAX_PEOPLE = 32;

Crowd::Crowd() :
	mNumVariants( MAX_PEOPLE ),
	mCrowdSeed( 589 )
{
}

void Crowd::Init( const fs::path &crowdFolder )
{
	psys.setBorderEvent( PARTICLE_BORDER_BOUNCE );
	psys.iteration = 5;

	float size = 10000;
	BaseParticleForce * gravity = new BaseParticleForce();
	gravity->setup( Vec2f( 0, 1), Rectd( -size / 2., -size / 2., size, size ));
	psys.add(gravity);

	heads  = getImages( crowdFolder / "head"  );
	torsos = getImages( crowdFolder / "torso" );
	arms   = getImages( crowdFolder / "arm"   );
	legs   = getImages( crowdFolder / "leg"   );

	stepY = 40;
	stepX = 70;
}

Crowd::~Crowd()
{

}

void Crowd::setup( const fs::path &crowdFolder, Vec2i size )
{
	Init( crowdFolder );

	width  = size.x;
	height = size.y;

	gl::Fbo::Format format;
	format.enableDepthBuffer( false );
	mFbo = gl::Fbo( size.x, size.y, format );


	const int w = 128;
	const int h = 192;
	for ( int i = 0; i < MAX_PEOPLE; ++i )
	{
		mPersonFbos.push_back( gl::Fbo( w, h, format ) );

		CPerson cp;
		cp.setup( &psys, Vec3f(  w / 2, h * .9 , 0 ),
				getRandomImage(&heads), getRandomImage(&torsos),
				getRandomImage(&arms), getRandomImage(&legs));
		cp.color.r = cp.color.g = cp.color.b = 1.f;
		addPerson(cp);
	}
	/*
	float spread = 20;

	for (int y = TOP_MARGIN; y < height - BOTTOM_MARGIN; y += stepY)
	{
		for (int x = LEFT_MARGIN; x < width - RIGHT_MARGIN; x += stepX)
		{
			CPerson cp;
			cp.setup( &psys, Vec3f( x - width / 2.0f + Rand::randFloat() * spread, y + Rand::randFloat() * spread, 0.0f ), getRandomImage(&heads), getRandomImage(&torsos), getRandomImage(&arms), getRandomImage(&legs));
			int light = (int)(y / (float)height * 200 + 10);
			cp.color.r = cp.color.g = cp.color.b = (float)light;

			addPerson(cp);
		}
	}
	*/
}

void Crowd::update()
{
	psys.update();

	for ( int i = 0; i < mNumVariants; i++ )
	{
		people[i].update();
	}

	_updateFbo();
}

void Crowd::_updateFbo()
{
	gl::enableAlphaBlending();

	Area viewport = gl::getViewport();
	gl::pushMatrices();

	for (int i = 0; i < mNumVariants; i++)
	{
		mPersonFbos[ i ].bindFramebuffer();
		gl::setMatricesWindow( mPersonFbos[ i ].getSize(), false );
		gl::setViewport( mPersonFbos[ i ].getBounds() );
		gl::clear( ColorA( 1, 0, 1, 0 ) );
		gl::color( Color::white() );

		people[ i ].draw();

		mPersonFbos[ i ].unbindFramebuffer();
	}


	mFbo.bindFramebuffer();
	gl::setMatricesWindow( mFbo.getSize(), false );
	gl::setViewport( mFbo.getBounds() );

	//gl::translate( mFbo.getSize().x / 2.0f, 0.0f, 0.0f );

	gl::clear( Color::black() );
	gl::color( Color::white() );

	psys.draw();

	Rand::randSeed( mCrowdSeed );
	float spread = 20;
	int fboW = mPersonFbos[ 0 ].getWidth();
	for (int y = TOP_MARGIN; y < height - BOTTOM_MARGIN; y += stepY)
	{
		float light = lmap< float>( y, TOP_MARGIN, height - BOTTOM_MARGIN, .2, 1 );
		gl::color( Color::gray( light ) );

		for (int x = LEFT_MARGIN; x < width - RIGHT_MARGIN; x += stepX)
		{
			float x2 = x + Rand::randFloat( -spread, spread );
			int r = Rand::randInt( mNumVariants );
			gl::pushModelView();
			gl::translate( x2, y );
			gl::draw( mPersonFbos[ r ].getTexture() );
			gl::popModelView();
			if ( x2 + fboW > width )
			{
				gl::pushModelView();
				gl::translate( x2 - width, y );
				gl::draw( mPersonFbos[ r ].getTexture() );
				gl::popModelView();
			}
		}
	}
	/*
	for (int i = 0; i < (int)people.size(); i++)
	{
		people[i].draw();
	}
	*/

	mFbo.unbindFramebuffer();

	gl::popMatrices();
	gl::setViewport( viewport );

	gl::disableAlphaBlending();
}

void Crowd::addPerson(CPerson cp)
{
	people.push_back(cp);
}

void Crowd::energize(float n)
{
	for ( int i = 0; i < mNumVariants; i++ )
	{
		people[i].energize(n);
	}
}

void Crowd::wave(float e)
{
	for ( int i = 0; i < mNumVariants; i++ )
	{
		people[i].energize(e, i * 3);
	}
}

std::vector<ci::ImageSourceRef> Crowd::getImages( fs::path folder )
{
	vector<ImageSourceRef> list;

	for( fs::directory_iterator it( folder ); it != fs::directory_iterator(); ++it )
	{
		if( fs::is_regular_file( *it ))
		{
			ImageSourceRef im = loadImage( folder.string() + "/" + it->path().filename().string());
			list.push_back( im );
		}
	}

	return list;
}

ImageSourceRef  Crowd::getRandomImage(vector<ImageSourceRef> * list)
{
	if (list->size() == 0) return ImageSourceRef();
	int n = Rand::randInt( 0, list->size());
	ImageSourceRef  img = (*(list->begin() + n));
	return img;
}

