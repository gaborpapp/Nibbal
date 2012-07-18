#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/Rand.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "Crowd.h"
#include "CPerson.h"


using namespace ci;
using namespace ci::app;
using namespace std;

Crowd::Crowd()
{
}

void Crowd::Init()
{
    psys.setBorderEvent( PARTICLE_BORDER_BOUNCE );
    psys.iteration = 5;

    float size = 10000;
    BaseParticleForce * gravity = new BaseParticleForce();
    gravity->setup( Vec2f( 0, 1), Rectd( -size / 2., -size / 2., size, size ));
    psys.add(gravity);

    heads  = getImages("head");
    torsos = getImages("torso");
    arms   = getImages("arm");
    legs   = getImages("leg");

    stepY = 40;
    stepX = 70;
}

Crowd::~Crowd()
{

}

void Crowd::setup(int w, int h)
{
	Init();

    width = w;
    height = h;

    float spread = 20;

    for (int y = 0; y < h; y += stepY)
    {
        for (int x = 0; x < w; x += stepX)
        {
            CPerson cp;
            cp.setup( &psys, Vec3f( x - w / 2. + Rand::randFloat() * spread, y + Rand::randFloat() * spread, 0), getRandomImage(&heads), getRandomImage(&torsos), getRandomImage(&arms), getRandomImage(&legs));
            int light = (int)(y / (float)h * 200 + 10);
            cp.color.r = cp.color.g = cp.color.b = light;

            addPerson(cp);
        }
    }
}

void Crowd::update()
{
    psys.update();

    for (int i = 0; i < people.size(); i++)
    {
        people[i].update();
    }
}

void Crowd::draw()
{
    for (int i = 0; i < people.size(); i++)
    {
        people[i].draw();
    }
}

void Crowd::addPerson(CPerson cp)
{
    people.push_back(cp);
}

void Crowd::energize(float n)
{
    //printf("ENERGIZE...");
    for (int i = 0; i < people.size(); i++)
    {
        people[i].energize(n);
    }
}

void Crowd::wave(float e)
{
    int n =  0;
    for (int y = 0; y < height; y += stepY)
    {
        for (int x = 0; x < width; x += stepX)
        {
            people[n].energize(e, x * 3);
            n++;
        }
    }
}

std::vector<ci::ImageSourceRef> Crowd::getImages( std::string dirName )
{
	vector<ImageSourceRef> list;

#if defined( CINDER_MAC )
	fs::path p( getResourcePath() / "assets"  / dirName );
#else
	fs::path p( getAppPath() / "../../assets" / dirName );
#endif

	for( fs::directory_iterator it( p ); it != fs::directory_iterator(); ++it )
	{
		if( fs::is_regular_file( *it ))
		{
			ImageSourceRef im = loadImage( p.string() + "/" + it->path().filename().string());
			list.push_back( im );
		}
	}

	return list;
}

ImageSourceRef  Crowd::getRandomImage(vector<ImageSourceRef> * list)
{
	if (list->size() == 0) return NULL;
	int n = Rand::randInt( 0, list->size());
	ImageSourceRef  img = (*(list->begin() + n));
	return img;
}


// ci::ImageSourceRef Crowd::_loadImage( std::string strImageName )
// {
// 	if( ! strImageName.empty())
// 	{
// 		try
// 		{
// 			fs::path imagePath( getAssetPath( strImageName ));
// 			return loadImage( imagePath );
// 		}
// 		catch( ... )
// 		{
// 			console() << "Unable to load the image: " << strImageName << std::endl;
// 		}
// 	}
// 
// 	return ImageSourceRef();
// }
