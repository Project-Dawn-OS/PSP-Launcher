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

	dropTex = GFX::g_TextureManager->loadTex("./assets/drop.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	dropSprite = new GFX::Render2D::Sprite(dropTex);

	selectTex = GFX::g_TextureManager->loadTex("./assets/selector.png", GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
	selectSprite = new GFX::Render2D::Sprite(selectTex);
	selX = selY = 0;

	directories.clear();
	validDirectories.clear();
	scanDirectories();
	validateDirectories();

	scrlY = 0;
}

MainState::~MainState()
{
}

extern "C" {
	int RunEboot(const char* args);
}

void MainState::update()
{
	if (Utilities::KeyPressed(PSP_CTRL_UP)) {
		selY--;
		if (selY < 0) {
			if (scrlY > 0) {
				scrlY--;
			}

			selY = 0;
		}
	}

	if (Utilities::KeyPressed(PSP_CTRL_DOWN)) {
		selY++;
		if (selY > 4) {
			if (scrlY < (validDirectories.size() - 25) / 5 + 1) {
				scrlY++;
			}
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

	if (Utilities::KeyPressed(PSP_CTRL_CROSS)) {
		int selIDX = selX + (selY+scrlY) * 5;

		if (selIDX < validDirectories.size()) {
			RunEboot(validDirectories[selIDX].path.c_str());
		}
	}
}

void MainState::draw() {
	int selIDX = selX + (selY+scrlY) * 5;
	if (selIDX < validDirectories.size()) {
		if (validDirectories[selIDX].hasPIC) {
			validDirectories[selIDX].picSprite->draw();
		}
	}

	textRenderer->setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_LEFT, 0.0f, 0xFF000000 });
	textRenderer->draw("PSP-Launcher", { 0, 16 });

	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			int idx = x + (y+scrlY) * 5;

			if (idx < validDirectories.size()) {
				//Try to draw an icon

				if (validDirectories[idx].hasICON) {
					dropSprite->setPosition(x * 76 + 40, y * 44 + 48);
					dropSprite->draw();

					validDirectories[idx].iconSprite->setPosition(x * 76 + 40, y * 44 + 48);
					validDirectories[idx].iconSprite->draw();
				}
				else {
					blankSprite->setPosition(x * 76 + 40, y * 44 + 48);
					blankSprite->draw();
				}

				if (idx == selIDX) {
					textRenderer->setStyle({ 255, 255, 255, 255, 1.0f, TEXT_RENDERER_CENTER, TEXT_RENDERER_RIGHT, 0.0f, 0xFF000000 });
					textRenderer->draw(validDirectories[idx].title, { 480, 16 });
				}
			}
			else {
				//Draw Nothing
			}
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

struct PBPHeader {
	uint8_t signature[4];
	uint16_t version[2];
	uint32_t offset[8];
};

struct SFOHeader {
	uint32_t magic;
	uint32_t version;
	uint32_t keyofs;
	uint32_t valofs;
	uint32_t count;
};

struct SFOEntry {
	uint16_t nameOffset;
	uint8_t alignment;
	uint8_t type;
	uint32_t valueSize;
	uint32_t totalSize;
	uint32_t dataOffset;
};

void MainState::validateDirectories()
{
	for (auto& s : directories) {
		std::fstream f("ms0:/PSP/GAME/" + s + "/EBOOT.PBP");

		if (f.is_open()) {
			ProgramInfo prog;
			prog.picTex = 0;
			prog.picSprite = NULL;
			prog.hasPIC = false;

			prog.iconTex = 0;
			prog.iconSprite = NULL;
			prog.hasICON = false;

			prog.title = "(No Title)";
			
			prog.path = "ms0:/PSP/GAME/" + s + "/EBOOT.PBP";
			
			f.close();

			FILE* file = fopen(prog.path.c_str(), "rw");

			PBPHeader header;
			fread(&header, sizeof(PBPHeader), 1, file);

			if (header.signature[0] == '\0' && header.signature[1] == 'P' && header.signature[2] == 'B' && header.signature[3] == 'P') {
				//Valid EBOOT.PBP file
				//We don't care about the version - let's look at the offset table
				//Table order:
				// 0  "PARAM.SFO" - Do care
				// 1  "ICON0.PNG" - Do care
				// 2  "ICON1.PMF" - Don't care
				// 3  "PIC0.PNG" - Don't care
				// 4  "PIC1.PNG" - Do care
				// 5  "SND0.AT3" - Don't care
				// 6  "DATA.PSP" - Don't care
				// 7  "DATA.PSAR" - Don't care

				int paramSize = header.offset[1] - header.offset[0];
				if (paramSize < 0) {
					//This can't happen - PARAM.SFO MUST be included!
					continue;
				}

				SFOHeader sfoH;
				fread(&sfoH, sizeof(SFOHeader), 1, file);

				if (sfoH.magic != 0x46535000) {
					//Error!
					continue;
				}

				SFOEntry* entries = new SFOEntry[sfoH.count];
				fread(entries, sizeof(SFOEntry), sfoH.count, file);

				for (int i = 0; i < sfoH.count; i++) {
					fseek(file, entries[i].nameOffset + sfoH.keyofs + sizeof(PBPHeader), SEEK_SET);

					int size = 0;
					if (i + 1 < sfoH.count) {
						//We're good
						size = entries[i + 1].nameOffset - entries[i].nameOffset;
					}
					else {
						size = sfoH.valofs - (entries[i].nameOffset + sfoH.keyofs);
					}

					char buf[32] = { 0 };
					fread(buf, 1, size, file);

					std::string str = std::string(buf);
					if (str == "TITLE") {
						fseek(file, entries[i].dataOffset + sfoH.valofs + sizeof(PBPHeader), SEEK_SET);
						char buf2[32] = { 0 };

						fread(buf2, 1, entries[i].valueSize, file);
						prog.title = std::string(buf2);
					}
				}
				delete[] entries;

				int icon0Size = header.offset[2] - header.offset[1];
				if (icon0Size > 0) {
					prog.hasICON = true;

					uint8_t* imgBuffer = new uint8_t[icon0Size];
					fseek(file, header.offset[1], SEEK_SET);
					fread(imgBuffer, 1, icon0Size, file);

					prog.iconTex = GFX::g_TextureManager->loadTexFromRAM(imgBuffer, icon0Size, GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
					prog.iconSprite = new GFX::Render2D::Sprite(prog.iconTex);
					prog.iconSprite->setScale(0.5f, 0.5f);

					delete[] imgBuffer;
				}
				else {
					prog.hasICON = false;
				}

				int pic1Size = header.offset[5] - header.offset[4];

				if (pic1Size > 0) {
					prog.hasPIC = true;

					uint8_t* imgBuffer = new uint8_t[pic1Size];
					fseek(file, header.offset[4], SEEK_SET);
					fread(imgBuffer, 1, pic1Size, file);

					prog.picTex = GFX::g_TextureManager->loadTexFromRAM(imgBuffer, pic1Size, GFX_FILTER_LINEAR, GFX_FILTER_LINEAR, true);
					prog.picSprite = new GFX::Render2D::Sprite(prog.picTex);
					prog.picSprite->setPosition(240, 136);

					delete[] imgBuffer;
				}
				else {
					prog.hasPIC = false;
				}

				Utilities::app_Logger->log("PBP: " + prog.path);
				Utilities::app_Logger->log("Title: " + std::to_string(prog.title));
				Utilities::app_Logger->log("HAS ICON?: " + std::to_string(prog.hasICON));
				Utilities::app_Logger->log("HAS PIC?: " + std::to_string(prog.hasPIC));
				validDirectories.push_back(prog);
			}

			fclose(file);

		}
		f.close();
	}
}
