#include "Particles.h"

#include <GFX/RenderCore.h>

ParticleSystem::ParticleSystem(int numberOfParticles, std::string texture): num(numberOfParticles){
	particles = new Particle[numberOfParticles];
	particleTex = GFX::g_TextureManager->loadTex(texture, GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);

	sprt = new GFX::Render2D::Sprite(particleTex);
	sprt->setScale(0.33f, 0.33f);
	sprt->setColor(1.0f, 1.0f, 1.0f, 0.3f);

	for (int i = 0; i < numberOfParticles; i++) {
		particles[i].x = rand() % 480;
		particles[i].y = rand() % 272;
		particles[i].vy = rand() % 21 - 5;
		particles[i].vx = rand() % 21 - 5;
	}
}

ParticleSystem::~ParticleSystem()
{
	delete particles;
	delete sprt;
	GFX::g_TextureManager->deleteTex(particleTex);
}

void ParticleSystem::update(double dt)
{
	for (int i = 0; i < num; i++) {
		particles[i].x += particles[i].vx * dt;
		particles[i].y += particles[i].vy * dt;

		if (particles[i].x > 480 || particles[i].x < 0) {
			particles[i].vx = -particles[i].vx;
			particles[i].x += particles[i].vx * dt;
		}

		if (particles[i].y > 272 || particles[i].y < 0) {
			particles[i].vy = -particles[i].vy;
			particles[i].y += particles[i].vy * dt;
		}
	}
}

void ParticleSystem::draw()
{
	for (int i = 0; i < num; i++) {
		sprt->setPosition(particles[i].x, particles[i].y);
		sprt->draw();
	}
}
