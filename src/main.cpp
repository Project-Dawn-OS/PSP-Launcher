#include <Platform/Platform.h>
#include <GFX/RenderCore.h>
#include <Utilities/Timer.h>
#include <GFX/2D/Sprite.h>
using namespace Stardust;


auto main() -> int {
	Platform::initPlatform();

	unsigned int logoTex = GFX::g_TextureManager->loadTex("./assets/Logo.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	GFX::Render2D::Sprite* logoSprite = new GFX::Render2D::Sprite(logoTex);
	logoSprite->setPosition(240, 136);
	logoSprite->setScale(0.5f, 0.5f);

	glm::vec4 goalColor = {0x25, 0x27, 0x4D, 0xFF};
	goalColor = glm::normalize(goalColor);

	glm::vec4 currentColor = { 0, 0, 0, 1 };

	GFX::g_RenderCore->setDefault2DMode();
	Utilities::g_AppTimer.reset();

	while (true) {
		GFX::g_RenderCore->beginFrame();
		double dt = Utilities::g_AppTimer.deltaTime();

		if (Utilities::g_AppTimer.elapsed() - 2 < 3) {
			if (Utilities::g_AppTimer.elapsed() > 2) {
				glm::vec4 res = glm::mix(currentColor, goalColor, static_cast<float>(Utilities::g_AppTimer.elapsed() - 2) / 3.0f);
				GFX::g_RenderCore->setClearColor(res.r, res.g, res.b, res.a);
				logoSprite->setColor(1.0, 1.0, 1.0, 1.0 - (static_cast<float>(Utilities::g_AppTimer.elapsed() - 2) / 3.0f));
			}
		}

		GFX::g_RenderCore->clear();
		logoSprite->draw();


		GFX::g_RenderCore->endFrame();
		Platform::platformUpdate();
	}

	Platform::exitPlatform();
}