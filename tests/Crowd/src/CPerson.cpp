#include "cinder\app\AppBasic.h"
#include "cinder\Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/cindermath.h"

#include "CPerson.h"

using namespace ci;
using namespace std;

CPerson::CPerson()
{

}

CPerson::~CPerson()
{

}

void CPerson::setup(ParticleSystem * _psys, Vec3f center, ImageSourceRef _head, ImageSourceRef _torso, ImageSourceRef _arm, ImageSourceRef _leg)
{
    psys = _psys;

	headImg = _head;
    torsoImg = _torso;
    armImg = _arm;
    legImg = _leg;

    energy = 1;

    pulseSpeedX = Rand::randFloat( 0.0006f, 0.003f );
    pulseSizeX = Rand::randFloat( 60.f, 150.f);

    pulseSpeedY = Rand::randFloat( 0.0006f, 0.003f);
    pulseSizeY = Rand::randFloat( 60.f, 150.f);

    phaseX = Rand::randFloat( 0.f, 10.f );
    phaseY = Rand::randFloat( 0.f, 10.f );

    float size = Rand::randFloat( 0.4f, 0.6f );
    float s_torso = 100 * size * Rand::randFloat( 0.7f, 1.3f );
    float s_head = 30 * size * Rand::randFloat( 0.9f, 1.1f );
    float s_shoulder = 40 * size * Rand::randFloat( 0.7f, 1.3f );
    float s_shoulder_y = 10 * size * Rand::randFloat( 1.f, 1.f );
    float s_arm = 60 * size * Rand::randFloat( 0.7f, 1.3f );
    float s_hip = 30 * size * Rand::randFloat( 0.7f, 1.3f );
    float s_leg = 60 * size * Rand::randFloat( 0.7f, 1.3f );

    float friction = 0.8f;

    oHead = Vec3f(center.x, center.y - s_torso - s_head, 0);
    tHeadY = oHead.y;

    torso = new BaseParticle();
        torso->setup(Vec2f(center.x, center.y), Vec2f(0, 0), 1, -1, false, friction);
    neck = new BaseParticle();
        neck->setup(Vec2f(center.x, center.y - s_torso), Vec2f(0, 0), 1, -1, false, friction);
    head = new BaseParticle();
        head->setup(Vec2f(center.x, center.y - s_torso - s_head), Vec2f(0, 0), 1, -1, true, friction);

    l_shoulder = new BaseParticle();
        l_shoulder->setup(Vec2f(center.x - s_shoulder, center.y - s_torso + s_shoulder_y), Vec2f(0, 0), 1, -1, false, friction);
    l_elbow = new BaseParticle();
        l_elbow->setup(Vec2f(center.x - s_shoulder - s_arm, center.y - s_torso), Vec2f(0, 0), 1, -1, false, friction);
    l_hand = new BaseParticle();
        l_hand->setup(Vec2f(center.x - s_shoulder - s_arm * 2, center.y - s_torso), Vec2f(0, 0), 1, -1, false, friction);

    r_shoulder = new BaseParticle();
        r_shoulder->setup(Vec2f(center.x + s_shoulder, center.y - s_torso + s_shoulder_y), Vec2f(0, 0), 1, -1, false, friction);
    r_elbow = new BaseParticle();
        r_elbow->setup(Vec2f(center.x + s_shoulder + s_arm, center.y - s_torso), Vec2f(0, 0), 1, -1, false, friction);
    r_hand = new BaseParticle();
        r_hand->setup(Vec2f(center.x + s_shoulder + s_arm * 2, center.y - s_torso), Vec2f(0, 0), 1, -1, false, friction);

    l_hip  = new BaseParticle();
        l_hip->setup(Vec2f(center.x - s_hip, center.y), Vec2f(0, 0), 1, -1, false, friction);
    l_knee  = new BaseParticle();
        l_knee->setup(Vec2f(center.x - s_hip, center.y + s_leg), Vec2f(0, 0), 1, -1, false, friction);
    l_foot = new BaseParticle();
        l_foot->setup(Vec2f(center.x - s_hip, center.y + s_leg * 2), Vec2f(0, 0), 1, -1, true, friction);

    r_hip  = new BaseParticle();
        r_hip->setup(Vec2f(center.x + s_hip, center.y), Vec2f(0, 0), 1, -1, false, friction);
    r_knee  = new BaseParticle();
        r_knee->setup(Vec2f(center.x + s_hip, center.y + s_leg), Vec2f(0, 0), 1, -1, false, friction);
    r_foot = new BaseParticle();
        r_foot->setup(Vec2f(center.x + s_hip, center.y + s_leg * 2), Vec2f(0, 0), 1, -1, true, friction);

    psys->add(torso);
    psys->add(neck);
    psys->add(head);
    psys->add(l_shoulder);
    psys->add(l_elbow);
    psys->add(l_hand);
    psys->add(r_shoulder);
    psys->add(r_elbow);
    psys->add(r_hand);
    psys->add(l_hip);
    psys->add(l_knee);
    psys->add(l_foot);
    psys->add(r_hip);
    psys->add(r_knee);
    psys->add(r_foot);

    float damp = 2;

    BaseParticleSpring * spring;
    ////////////////////////////////////
    spring = new BaseParticleSpring();
        spring->setup(torso, neck, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(neck, head, damp);
    psys->add(spring);

    ////////////////////////////////////
    spring = new BaseParticleSpring();
        spring->setup(neck, l_shoulder, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_shoulder, l_elbow, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_elbow, l_hand, damp);
    psys->add(spring);

    ////////////////////////////////////
    spring = new BaseParticleSpring();
        spring->setup(neck, r_shoulder, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(r_shoulder, r_elbow, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(r_elbow, r_hand, damp);
    psys->add(spring);

    ////////////////////////////////////
    spring = new BaseParticleSpring();
        spring->setup(torso, l_hip, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_hip, l_knee, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_knee, l_foot, damp);
    psys->add(spring);

    ////////////////////////////////////
    spring = new BaseParticleSpring();
        spring->setup(torso, r_hip, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(r_hip, r_knee, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(r_knee, r_foot, damp);
    psys->add(spring);

    //////////////////////////////////////
    // merevito kotesek
    spring = new BaseParticleSpring();
        spring->setup(head, l_shoulder, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(head, r_shoulder, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(r_shoulder, l_shoulder, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_hip, r_hip, damp);
    psys->add(spring);

    spring = new BaseParticleSpring();
        spring->setup(l_hand, r_hand, 150);
    psys->add(spring);


    //////////////////////
    handDamp = 50;

    handTarget = new BaseParticle();
    handTarget->setup(Vec2f(head->position.x, head->position.y), Vec2f(0, 0), 1, -1, true);
    psys->add(handTarget);

    l_handSpring = new BaseParticleSpring();
    l_handSpring->setup(handTarget, l_hand, handDamp);
    psys->add(l_handSpring);
    r_handSpring = new BaseParticleSpring();
    r_handSpring->setup(handTarget, r_hand, handDamp);
    psys->add(r_handSpring);

    impulseStarted = -1;

    /*color.r =(int)ofRandom(0, 255);
    color.g =(int)ofRandom(0, 255);
    color.b =(int)ofRandom(0, 255);

    torsoColor.setDefaultColorMode(OF_COLOR_HSV);
    armColor.setDefaultColorMode(OF_COLOR_HSV);
    legColor.setDefaultColorMode(OF_COLOR_HSV);

    torsoColor = ofxColor(ofRandom(0, 20), 255, 255);
    armColor = ofxColor(ofRandom(40, 150), 255, 255);
    legColor = ofxColor(ofRandom(150, 200), 255, 255);
    */

}

void CPerson::update()
{
	float time = ci::app::getElapsedSeconds() * 1000;

    if (impulseStarted != -1)
    {
		if( time - impulseStarted > delay)
        {
            impulseStarted = -1;
            energy = tEnergy;
        }
    }
    energy += (1 - energy) / 50.f;
    handTarget->position.x = head->position.x - cos( time * pulseSpeedX + phaseX) * pulseSizeX * 0.2;
    handTarget->position.y = head->position.y - (sin( time * (pulseSpeedY) + phaseY) * 0.5 + 0.5) * pulseSizeY * energy;
    head->position.x = oHead.x + sin( time * pulseSpeedX + phaseX) * 5;

    if (energy > 1.2)
    {
        if (Rand::randInt(0, 100) < 10) tHeadY = oHead.y - energy * Rand::randInt(20, 50);
    }
    else if (Rand::randInt(0, 100) < 5) tHeadY = oHead.y;
    head->position.y += (tHeadY - head->position.y) / 12.;
    //printf("%f\n", energy);
}

void CPerson::draw()
{
    //glLineWidth(10);
    Color( color.r, color.g, color.b);

    //ofSetColor(255, 255, 255);
    drawLimb(neck, head, headImg);

    //ofSetColor(armColor.r, armColor.g, armColor.b);
    drawLimb(l_shoulder, l_elbow, armImg);
    drawLimb(l_elbow, l_hand, armImg);

    drawLimb(r_shoulder, r_elbow, armImg);
    drawLimb(r_elbow, r_hand, armImg);

    //ofSetColor(torsoColor.r, torsoColor.g, torsoColor.b);
    drawLimb(torso, l_hip, legImg);
    drawLimb(torso, r_hip, legImg);

    //ofSetColor(legColor.r, legColor.g, legColor.b);
    drawLimb(l_hip, l_knee, legImg);
    drawLimb(l_knee, l_foot, legImg);
    drawLimb(r_hip, r_knee, legImg);
    drawLimb(r_knee, r_foot, legImg);

    //ofSetColor(torsoColor.r, torsoColor.g, torsoColor.b);
    drawLimb(torso, neck, torsoImg);
    //drawLimb(neck, l_shoulder, armImg);
    //drawLimb(neck, r_shoulder, armImg);

    //glLineWidth(1);
/*
    ofSetColor(0, 255, 255);
    ofFill();
    ofCircle(handTarget->position.x, handTarget->position.y, 3);
    drawLimb(handTarget, l_hand);
    drawLimb(handTarget, r_hand);
    */
}

void CPerson::drawLimb(BaseParticle * a, BaseParticle * b, gl::Texture img )
{
    //ofLine(a->position.x, a->position.y, b->position.x, b->position.y);

    if (img == NULL) return;

    float w = img.getWidth();
    float h = img.getHeight();
    float ratio = img.getHeight() / (float)img.getWidth();
    float length = ( a->position - b->position ).length() * 1.2;

     glPushMatrix();
         glTranslatef(a->position.x, a->position.y, 0);
         glRotatef(toDegrees(atan2(b->position.y - a->position.y, b->position.x - a->position.x)), 0, 0, 1);

	gl::draw( img, Rectf( 0, -length * ratio / 2., length, +length * ratio / 2. ));

    glPopMatrix();
}

void CPerson::energize(float e, int _delay)
{
    delay = _delay;
    tEnergy = e;
    impulseStarted =  ci::app::getElapsedSeconds() * 1000;
}
