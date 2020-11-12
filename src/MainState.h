#pragma once
#include <GFX/UI/TextRenderer.h>
#include <GFX/2D/Sprite.h>
#include <vector>

using namespace Stardust;

struct ProgramInfo {
	std::string title;
	std::string path; // Used for categorization purposes
	bool hasICON;
	bool hasPIC;
	GFX::Render2D::Sprite* iconSprite, *picSprite;
	unsigned int iconTex, picTex;
};

class MainState {
public:
	MainState();
	~MainState();

	void update();
	void draw();

private:
	GFX::UI::TextRenderer* textRenderer;
	GFX::Render2D::Sprite* blankSprite, *selectSprite, *dropSprite;
	unsigned int blankTex, selectTex, dropTex;
	int selX, selY;

	void scanDirectories();
	void validateDirectories();

	std::vector<std::string> directories;
	std::vector<ProgramInfo> validDirectories;
};