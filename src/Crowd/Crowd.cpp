#include <vector>

#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "Crowd.h"
#include "CPerson.h"


using namespace ci;
using namespace ci::app;
using namespace std;

static const int TOP_MARGIN    = 370;
static const int BOTTOM_MARGIN = 90;
static const int LEFT_MARGIN   = 55;
static const int RIGHT_MARGIN  = 55;

Crowd::Crowd()
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

	float spread = 20;

	for (int y = TOP_MARGIN; y < height - BOTTOM_MARGIN; y += stepY)
	{
		for (int x = LEFT_MARGIN; x < width - RIGHT_MARGIN; x += stepX)
		{
			CPerson cp;
			cp.setup( &psys, Vec3f( x - width / 2.0f + Rand::randFloat() * spread, y + Rand::randFloat() * spread, 0.0f), getRandomImage(&heads), getRandomImage(&torsos), getRandomImage(&arms), getRandomImage(&legs));
			int light = (int)(y / (float)height * 200 + 10);
			cp.color.r = cp.color.g = cp.color.b = (float)light;

			addPerson(cp);
		}
	}
}

void Crowd::update()
{
	psys.update();

	for (int i = 0; i < (int)people.size(); i++)
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

	mFbo.bindFramebuffer();
	gl::setMatricesWindow( mFbo.getSize(), false );
	gl::setViewport( mFbo.getBounds() );

	gl::translate( mFbo.getSize().x / 2.0f, 0.0f, 0.0f );

	gl::clear( Color::black() );
	gl::color( Color::white() );

	psys.draw();

	for (int i = 0; i < (int)people.size(); i++)
	{
		people[i].draw();
	}

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
	for (int i = 0; i < (int)people.size(); i++)
	{
		people[i].energize(n);
	}
}

void Crowd::wave(float e)
{
	for (int i = 0; i < (int)people.size(); i++)
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

