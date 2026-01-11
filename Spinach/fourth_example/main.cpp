#include <iostream>
#include <algorithm>
#include <random>
#include <thread>
#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_core.h"
#include "../spinach/common/spn_profiler.h"
#include <ui_event.h>
#include <ui_event_translator.h>


#define GAMERESOLUTIONX 800
#define GAMERESOLUTIONY 600
#define MAXROWS 19
#define MAXCOLS 11
#define MAXDANGEROUSTILES 21
#define TILEW 30
#define TILEH 30
int startX = 10;
int startY = 10;
bool isGameOver = false;
bool isGameWon = false;
spn::Image greyTile;
spn::Image flagTile;
spn::Image dangerTile;
float runningTime = 0;
int blastRow = -1;
int blastCol = -1;

struct MinuteSecond {
	int totalSeconds;
	int minutes;
	int seconds;
};

MinuteSecond ms;


struct Tile
{
	bool isVisited;
	bool isDangerous;
	bool isFlagged;
	int dangerousNeighbourCount;
};

Tile board[MAXROWS][MAXCOLS];

bool IsValidRowCol(int r, int c) {
	return (r >= 0 && r < MAXROWS) && (c >= 0 && c < MAXCOLS);
}

bool IsInvalidRowCol(int r, int c) {
	return (r < 0 || r >= MAXROWS) || (c < 0 || c >= MAXCOLS);
}

void CheckIfGameWon() {
	isGameWon = false;
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			if (t.isDangerous && !t.isFlagged) {
				isGameWon = false;
				return;
			}
		}
	}
	isGameWon = true;
}

void OpenPositiveDncTiles(int row, int col) {
	//std::cout << "dnc " << row << "\t" << col << "\n";
	if (IsInvalidRowCol(row, col)) {
		return;
	}
	Tile& t = board[row][col];
	if (t.isVisited || t.isDangerous) {
		return;
	}
	t.isVisited = true;
	if (t.dangerousNeighbourCount == 0) {
		for (int dr = -1; dr <= 1; dr++) {
			for (int dc = -1; dc <= 1; dc++) {
				if (!(dr == 0 && dc == 0) && IsValidRowCol(row + dr, col + dc)) {
					OpenPositiveDncTiles(row + dr, col + dc);
				}
			}
		}
	}
}

void Tick() {
	++ms.totalSeconds;
	ms.minutes = ms.totalSeconds / 60;
	ms.seconds = ms.totalSeconds % 60;
}

void InitBoard() {
	int cellcount = MAXROWS * MAXCOLS;
	int dboard[MAXROWS * MAXCOLS];
	int k;
	for (k = 0; k < MAXDANGEROUSTILES; k++) {
		dboard[k] = 1;
	}
	for (; k < cellcount; k++) {
		dboard[k] = 0;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(std::begin(dboard), std::end(dboard), gen);
	
	k = 0;
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			t.isVisited = false;
			t.isFlagged = false;
			t.isDangerous = (dboard[k++] == 1);
		}
	}

	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			if (t.isDangerous) {
				t.dangerousNeighbourCount = -1;
				continue;
			}
			int dnc = 0;
			for (int dr = -1; dr <= 1; dr++) {
				for (int dc = -1; dc <= 1; dc++) {
					if (dc == 0 && dr == 0) {
						continue;
					}
					if (IsValidRowCol(i + dr, j + dc) && board[i + dr][j + dc].isDangerous) {
						++dnc;
					}
				}
			}
			t.dangerousNeighbourCount = dnc;
		}
	}
	isGameOver = false;
	greyTile.CreateSolidColorBlockImage(TILEW-4, TILEH-4, 128, 128, 128, 1.0);
	flagTile.CreateFromPng("../res/flag.png");
	dangerTile.CreateFromPng("../res/danger.png");
	ms.totalSeconds = 0;
	runningTime = 0;
	blastRow = -1;
	blastCol = -1;
}

void MarkBoard(int x, int y, int opcode) {
	int minx = startX;
	int miny = startY;
	int maxy = startY + MAXROWS * TILEH;
	int maxx = startX + MAXCOLS * TILEW;
	if (x < minx || x > maxx || y < miny || y > maxy) {
		//std::cout << "out1\n";
		return;
	}
	float spany = (maxy - miny);
	float dy = (y - miny);
	int r = (dy / spany) * MAXROWS;

	float spanx = (maxx - minx);
	float dx = (x - minx);
	int c = (dx / spanx) * MAXCOLS;

	if (r < 0 || r > MAXROWS - 1 || c < 0 || c > MAXCOLS - 1) {
		//std::cout << "out2\n";
		return;
	}

	//std::cout << r << " " << c << "\n";
	Tile& t = board[r][c];
	if (opcode == 0) {
		//std::cout << "visited\n";
		if (t.dangerousNeighbourCount == 0) {
			OpenPositiveDncTiles(r, c);
		}
		t.isVisited = true;
	}
	else if (opcode == 1 && t.isVisited == false) {
		//std::cout << "flagged\n";
		t.isFlagged = !t.isFlagged;
	}

	if (t.isVisited && t.isDangerous) {
		isGameOver = true;
		blastRow = r;
		blastCol = c;
		for (int i = 0; i < MAXROWS; i++) {
			for (int j = 0; j < MAXCOLS; j++) {
				Tile& t = board[i][j];
				if (t.isDangerous) {
					t.isVisited = true;
				}
			}
		}
	}
}

void DisplayTime(spn::Canvas* canvas) {
	char buffer[256];
	sprintf(buffer, "%2d:%2d", ms.minutes, ms.seconds);
	canvas->SetPrimaryColorUint(0xffff00);
	canvas->DrawCString(buffer, GAMERESOLUTIONX / (float)(2), (GAMERESOLUTIONY / (float)(2))+40);
}

void DisplayBoard(spn::Canvas* canvas) {
	char str[2];
	int padding = 8;
	str[1] = '\0';

	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			int y = startY + i * TILEH;
			int x = startX + j * TILEW;
			canvas->SetPrimaryColorUint(0xffff00);
			canvas->DrawRectangle(x, y, x + TILEW, y + TILEH);

			const Tile& t = board[i][j];
			if (!t.isVisited) {
				//greyTile
				canvas->DrawImage(&greyTile, x+2, y+2);
			}
			if (t.isFlagged) {
				canvas->SetPrimaryColorUint(0xfffff);
				//canvas->DrawCString("F", x + padding, y + padding);
				canvas->DrawImage(&flagTile, x, y);
			}
			else if (t.isVisited && t.isDangerous) {
			//else if (t.isDangerous) {
				canvas->SetPrimaryColorUint(0xff0000);
				//canvas->DrawCString("M", x + padding, y + padding);
				canvas->DrawImage(&dangerTile, x, y);
			}
			else if(t.isVisited && t.dangerousNeighbourCount > 0){
			//else if(t.dangerousNeighbourCount > 0){
				str[0] = '0' + t.dangerousNeighbourCount;
				canvas->SetPrimaryColorUint(0xffffff);
				canvas->DrawCString(str, x + padding, y + padding);
			}
			
		}
	}

	if (isGameOver) {
		int by = startY + blastRow * TILEH;
		int bx = startX + blastCol * TILEW;
		canvas->SetPrimaryColorUint(0xff0000);
		canvas->DrawRectangle(bx+2, by+2, bx + TILEW-2, by + TILEH-2);
		canvas->DrawRectangle(bx + 4, by + 4, bx + TILEW - 4, by + TILEH - 4);
		canvas->DrawRectangle(bx + 6, by + 6, bx + TILEW - 6, by + TILEH - 6);
		canvas->DrawCString("Game Over!", GAMERESOLUTIONX/(float)(2), GAMERESOLUTIONY / (float)(2));
	}
	else if (isGameWon) {
		canvas->SetPrimaryColorUint(0x0000ff);
		canvas->DrawCString("You Won!", GAMERESOLUTIONX / (float)(2), GAMERESOLUTIONY / (float)(2));
	}
}

void UpdateAndRender(spn::Canvas* canvas) {
	runningTime += canvas->GetLastFrameTime();
	if (!isGameWon && !isGameOver && runningTime >= 1.0f) {
		Tick();
		runningTime = 0;
	}
	CheckIfGameWon();
	canvas->Clear();
	//canvas->DrawCString("the quick brown fox 1234567890", 245, 40);
	DisplayBoard(canvas);
	DisplayTime(canvas);
}

void HandleInput(const SDL_Event* sdlEvent) {

	UiEvent uie;
	TranslateSdlEvent(sdlEvent, uie);

	switch (uie.eventType) {
	case UiEventType::MouseUp:
		if (uie.mouseButton == MouseButton::Left) {
			if (!isGameWon && !isGameOver) {
				MarkBoard(uie.mouseX, uie.mouseY, 0);
			}
		}
		else {
			if (!isGameWon && !isGameOver) {
				MarkBoard(uie.mouseX, uie.mouseY, 1);
			}
		}
		break;
	case UiEventType::ActionKeyDown:
		//std::cout << "action\n";
		if (uie.keyCode == KeyCode::Right) {
			InitBoard();
		}
		break;
	}
	//std::cout << "*";
}

// note
// res folder and build_root folder are inside the project folder
// build_root is where solution is built using cmake
// there for the correct resources folder for me is "../res/"
// note that i have included build_root in .gitignore


int main(int argc, char* argv[])
{
	spn::SpinachCore sc(GAMERESOLUTIONX, GAMERESOLUTIONY, "../res/");
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	InitBoard();
	
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("YAMSC - Yet Another Mine Sweeper Clone");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.MainLoop();
	spn::Profiler::GetInstance().Print();
	return 0;
}