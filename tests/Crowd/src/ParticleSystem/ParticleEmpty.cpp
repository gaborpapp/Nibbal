#include "ParticleSystem/ParticleEmpty.h"

ParticleEmpty::ParticleEmpty(float x, float y, float _friction, bool _fixed)
{
    setPosition(x, y);
    fixed = _fixed;
    friction = _friction;
}

ParticleEmpty::~ParticleEmpty()
{

}

void ParticleEmpty::draw()
{
    /*
    ofFill();
    ofSetColor(255, 255, 255, 100);
    ofCircle(position.x, position.y, 1);
    */
}
