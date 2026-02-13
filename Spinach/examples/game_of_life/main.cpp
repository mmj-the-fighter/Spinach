#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <ui_event.h>
#include <ui_event_translator.h>
#include <ui_manager.h>
#include <button.h>


constexpr int startx = 50;
constexpr int starty = 20;

constexpr int MAXRESX = 800;
constexpr int MAXRESY = 600;

constexpr int LIFEBOARDMAXX = 512;
constexpr int LIFEBOARDMAXY = 512;
constexpr int CELLSIZE = 16;

constexpr int maxCols = LIFEBOARDMAXX / CELLSIZE;
constexpr int maxRows = LIFEBOARDMAXY / CELLSIZE;
int board[maxRows][maxCols];
int boardnc[maxRows][maxCols];
bool stopFlag = false;

enum TileType {
	EMPTY=0,
	OCCUPIED=1
};


spn::rmgui::Button* startSimButton;
spn::rmgui::Button* stopSimButton;
spn::rmgui::Button* clearSimButton;
spn::rmgui::UiManager* uim;

void ClearBoard() {
	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			board[i][j] = EMPTY;
		}
	}
	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			boardnc[i][j] = EMPTY;
		}
	}
}

bool IsValidRowCol(int r, int c) {
	return (r >= 0 && r < maxRows) && (c >= 0 && c < maxCols);
}

bool IsInvalidRowCol(int r, int c) {
	return (r < 0 || r >= maxRows) || (c < 0 || c >= maxCols);
}

void CalculateNeighbourCounts() {

	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			boardnc[i][j] = EMPTY;
		}
	}

	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			for (int dr = -1; dr <= 1; dr++) {
				for (int dc = -1; dc <= 1; dc++) {
					if (IsValidRowCol(i + dr, j + dc)
						&& (dr != 0 || dc != 0)
						&& board[i + dr][j + dc] == OCCUPIED) {
						boardnc[i][j] += 1;
					}
				}
			}
		}
	}
}

void Populate() {
	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			switch (boardnc[i][j]) {
			case 0:
			case 1:
				board[i][j] = EMPTY;
				break;
			case 2:
				break;
			case 3:
				board[i][j] = OCCUPIED;
				break;
			default:
				board[i][j] = EMPTY;
				break;
			}
		}
	}
}
void StartSimulation() {
	stopFlag = false;
	std::thread t([=]() {
		std::cout << "Starting Simulation Thread\n";
		while (!stopFlag) {
			CalculateNeighbourCounts();
			Populate();
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
		std::cout << "Stoping Simulation Thread\n";
	});
	t.detach();
}

void StopSimulation() {
	stopFlag = true;
}


void InitUi() {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();
	startSimButton = uim->CreateWidget<Button>();
	startSimButton->SetPosition(80, MAXRESY-50);
	startSimButton->SetSize(128, 32);
	startSimButton->SetString("Start");
	startSimButton->SetCallback([=](int id) {
		//Filter(false);
		StartSimulation();
		});

	stopSimButton = uim->CreateWidget<Button>();
	stopSimButton->SetPosition(80+140, MAXRESY - 50);
	stopSimButton->SetSize(128, 32);
	stopSimButton->SetString("Stop");
	stopSimButton->SetCallback([=](int id) {
		//Filter(false);
		StopSimulation();
		});

	clearSimButton = uim->CreateWidget<Button>();
	clearSimButton->SetPosition(80 + 140 + 140, MAXRESY - 50);
	clearSimButton->SetSize(128, 32);
	clearSimButton->SetString("Clear");
	clearSimButton->SetCallback([=](int id) {
		//Filter(false);
		StopSimulation();
		ClearBoard();
		});
}


void InitApp() {
	ClearBoard();
}


void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->SaveColors();

	canvas->SetPrimaryColorUint(0xE6C71A);

	for (int i = 0; i < maxRows; i++) {
		for (int j = 0; j < maxCols; j++) {
			int left = startx + i * CELLSIZE;
			int top = starty + j * CELLSIZE;
			if (board[i][j] == OCCUPIED) {
				
				canvas->DrawFilledRectangle(left, top, left + CELLSIZE, top + CELLSIZE);
			}
			else {
				
				canvas->DrawRectangle(left, top, left + CELLSIZE, top + CELLSIZE);
			}
		}
	}
	canvas->RestoreColors();
	uim->Display(canvas);
}

void MarkBoard(int x, int y, bool occupy) {
	int minx = startx;
	int miny = starty;
	int maxy = starty + maxRows * CELLSIZE;
	int maxx = startx + maxCols * CELLSIZE;
	if (x < minx || x > maxx || y < miny || y > maxy) {
		//std::cout << "out1\n";
		return;
	}
	float spany = (maxy - miny);
	float dy = (y - miny);
	int r = (dy / spany) * maxRows;

	float spanx = (maxx - minx);
	float dx = (x - minx);
	int c = (dx / spanx) * maxCols;
	board[c][r] = occupy ? OCCUPIED : EMPTY;
}

void HandleInput(const SDL_Event* sdlEvent) {
	spn::rmgui::UiEvent uie;
	spn::rmgui::TranslateSdlEvent(sdlEvent, uie);
	uim->HandleUiEvent(uie);

	using namespace spn::rmgui;
	switch (uie.eventType) {
	case UiEventType::MouseUp:
		MarkBoard(uie.mouseX, uie.mouseY, (uie.mouseButton == MouseButton::Left));
		break;
	case UiEventType::MouseDrag:
		MarkBoard(uie.mouseX, uie.mouseY, true);
		break;
	case UiEventType::MouseMove:
		if (uie.mouseButton == MouseButton::Right) {
			MarkBoard(uie.mouseX, uie.mouseY, false);
		}
		break;
	}
}

int main(int argc, char* argv[])
{
	spn::SpinachCore sc(MAXRESX, MAXRESY, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	InitApp();
	InitUi();
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Conway's Game of Life");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.MainLoop();
	return 0;
}