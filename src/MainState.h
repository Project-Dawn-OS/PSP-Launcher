#pragma once
#include <GFX/UI/TextRenderer.h>
#include <GFX/2D/Sprite.h>
#include <vector>

using namespace Stardust;

class MainState {
public:
	MainState();
	~MainState();

	void update();
	void draw();

private:
	GFX::UI::TextRenderer* textRenderer;
	GFX::Render2D::Sprite* blankSprite, *selectSprite;
	unsigned int blankTex, selectTex;
	int selX, selY;

	void scanDirectories();
	void validateDirectories();

	std::vector<std::string> directories;
	std::vector<std::string> validDirectories;
};