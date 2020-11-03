#include "MainState.h"

MainState::MainState()
{
	textRenderer = new GFX::UI::TextRenderer();
	textRenderer->init("./assets/noto/ltn8.pgf");
	textRenderer->setStyle({255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0xFF000000});

	blankTex = GFX::g_TextureManager->loadTex("./assets/blank.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	blankSprite = new GFX::Render2D::Sprite(blankTex);

	selectTex = GFX::g_TextureManager->loadTex("./assets/selector.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	selectSprite = new GFX::Render2D::Sprite(selectTex);
}

MainState::~MainState()
{
}

void MainState::update()
{
}

void MainState::draw()
{
	textRenderer->setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_LEFT, 0.0f, 0xFF000000 });
	textRenderer->draw("PSP-Launcher", { 0, 16 });

	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {

			blankSprite->setPosition(x*76 + 40, y*44 + 48);
			blankSprite->draw();
		}
	}

	selectSprite->setPosition(40, 48);
	selectSprite->draw();

	textRenderer->setStyle({ 255, 255, 255, 255, 0.67f, TEXT_RENDERER_CENTER, TEXT_RENDERER_RIGHT, 0.0f, 0xFF000000 });
	textRenderer->draw("Experimental!", { 480, 260 });
}
