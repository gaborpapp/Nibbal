#pragma once

#include "cinder/CinderMath.h"
#include "ParticleSystem/BaseParticle.h"

class SimpleParticle : public BaseParticle
{
    public:
        SimpleParticle(float x, float y)
        {
            position.set(x, y);
            friction = ci::Rand::randFloat( 0.7, 0.9);
            int a = 255;//(int)ci::Rand::randInt( 0, 255);
            r = g = b = a;
        };
        ~SimpleParticle(){};

        void update()
        {
            movement += ci::vec2f(ci::Rand::randFloat(), ci::Rand::randFloat() * 0.2);
        }

        void draw()
        {
            ci::Color( r, g, b);
            glLineWidth(2);
			Vec2f position2 = position;
			position2.x -= movement.x * 3;
			position2.y -= movement.y * 3;

			gl::drawLine( position, position2 );
            glLineWidth(1);
        };

        int r, g, b;
};
