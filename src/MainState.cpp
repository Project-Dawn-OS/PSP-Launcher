#include "MainState.h"
#include <Utilities/Input.h>
#include <Utilities/Logger.h>

MainState::MainState()
{
	textRenderer = new GFX::UI::TextRenderer();
	textRenderer->init("./assets/noto/ltn8.pgf");
	textRenderer->setStyle({255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_CENTER, 0.0f, 0xFF000000});

	blankTex = GFX::g_TextureManager->loadTex("./assets/blank.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	blankSprite = new GFX::Render2D::Sprite(blankTex);

	selectTex = GFX::g_TextureManager->loadTex("./assets/selector.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	selectSprite = new GFX::Render2D::Sprite(selectTex);
	selX = selY = 0;

	directories.clear();
	validDirectories.clear();
	scanDirectories();
	validateDirectories();
}

MainState::~MainState()
{
}

void MainState::update()
{
	if (Utilities::KeyPressed(PSP_CTRL_UP)) {
		selY--;
		if (selY < 0) {
			selY = 0;
		}
	}

	if (Utilities::KeyPressed(PSP_CTRL_DOWN)) {
		selY++;
		if (selY > 4) {
			selY = 4;
		}
	}

	if (Utilities::KeyPressed(PSP_CTRL_LEFT)) {
		selX--;
		if (selX < 0) {
			selX = 0;
		}
	}

	if (Utilities::KeyPressed(PSP_CTRL_RIGHT)) {
		selX++;
		if (selX > 4) {
			selX = 4;
		}
	}
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

	selectSprite->setPosition(40 + selX*76, 48 + selY*44);
	selectSprite->draw();

	textRenderer->setStyle({ 255, 255, 255, 255, 0.67f, TEXT_RENDERER_CENTER, TEXT_RENDERER_RIGHT, 0.0f, 0xFF000000 });
	textRenderer->draw("Experimental!", { 480, 260 });
}

#include <dirent.h>
void MainState::scanDirectories(){
	struct dirent* dp;
	DIR* dfd;

	dfd = opendir("ms0:/PSP/GAME/");
	if (dfd != NULL) {
		while ((dp = readdir(dfd)) != NULL) {
			std::string name = dp->d_name;

			if (name.find_first_of('.') == name.npos) {
				if (name != "." && name != "..") {
					directories.push_back(name);
				}
			}
		}
	}
}

void MainState::validateDirectories()
{
	for (auto& s : directories) {
		std::fstream f("ms0:/PSP/GAME/" + s + "/EBOOT.PBP");
		if (f.is_open()) {
			Utilities::app_Logger->log(s);
			validDirectories.push_back(s);
		}
		f.close();
	}
}
