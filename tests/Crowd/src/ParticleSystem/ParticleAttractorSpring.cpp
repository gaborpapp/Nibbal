#include "cinder/app/App.h"
#include "ParticleSystem/ParticleAttractorSpring.h"

using namespace ci;

ParticleAttractorSpring::ParticleAttractorSpring()
{

}

ParticleAttractorSpring::~ParticleAttractorSpring()
{

}

void ParticleAttractorSpring::draw()
{

    float d = ( p1->position - p2->position ).length();
    float w = 2;
    if (d > distance)
    {
        float p = distance / d;
        w = (1 - p) * 4;
    }

    //ofSetColor(0, 0, 0, 200);
    int b = 255;
	gl::color( ColorA( b, b, b, 200 ));

    //glLineWidth(w + 1);
    //ofEnableSmoothing();
    gl::drawLine( p1->position, p2->position );
    //ofDisableSmoothing();
    glLineWidth(1);
}

