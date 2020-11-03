#pragma once
#include <GFX/2D/Sprite.h>

using namespace Stardust;

struct Particle {
	float x;
	float y;
	float vx;
	float vy;
};

class ParticleSystem {
public:
	ParticleSystem(int numberOfParticles, std::string texture);
	~ParticleSystem();

	void update(double dt);
	void draw();
private:
	Particle* particles;
	GFX::Render2D::Sprite* sprt;
	int num;
	unsigned int particleTex;
};