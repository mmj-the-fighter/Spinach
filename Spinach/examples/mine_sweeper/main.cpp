#include <iostream>
#include <algorithm>
#include <random>
#include <thread>
#include <vector>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include <ui_event.h>
#include <ui_event_translator.h>
#include <ui_manager.h>
#include <button.h>


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
bool lockInput = false;
spn::Image greyTile;
spn::Image flagTile;
spn::Image dangerTile;
float runningTime = 0;
int blastRow = -1;
int blastCol = -1;
spn::rmgui::Button* solveButton;
spn::rmgui::Button* nextButton;
spn::rmgui::UiManager* uim;

struct MinuteSecond {
	int totalSeconds;
	int minutes;
	int seconds;
};

enum class Opcode {
	FLAG_TILE,
	OPEN_TILE
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

void OpenEmptyAndPositiveDncTiles(int row, int col) {
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
					OpenEmptyAndPositiveDncTiles(row + dr, col + dc);
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
	flagTile.CreateFromPng("../examples/res_for_examples/flag.png");
	dangerTile.CreateFromPng("../examples/res_for_examples/danger.png");
	ms.totalSeconds = 0;
	runningTime = 0;
	blastRow = -1;
	blastCol = -1;
}

void MarkBoardRowCol(int r, int c, Opcode opcode) {
	if (r < 0 || r > MAXROWS - 1 || c < 0 || c > MAXCOLS - 1) {
		//std::cout << "out2\n";
		return;
	}

	//std::cout << r << " " << c << "\n";
	Tile& t = board[r][c];
	if (opcode == Opcode::OPEN_TILE) {
		//std::cout << "visited\n";
		if (t.dangerousNeighbourCount == 0) {
			lockInput = true;
			OpenEmptyAndPositiveDncTiles(r, c);
			lockInput = false;
		}
		t.isVisited = true;
	}
	else if (opcode == Opcode::FLAG_TILE && t.isVisited == false) {
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

void MarkBoard(int x, int y, Opcode opcode) {
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

	MarkBoardRowCol(r, c, opcode);
	
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
				if (isGameOver || isGameWon) {
					if (!t.isDangerous) {
						canvas->SetPrimaryColorUint(0xff0000);
						canvas->DrawRectangle(x + 2, y + 2, x + TILEW - 2, y + TILEH - 2);
						canvas->DrawLine(x + 2, y + 2, x + TILEW - 2, y + TILEH - 2);
						canvas->DrawLine(x + 2, y + TILEH - 2, x + TILEW - 2, y + 2);
					}
				}
			}
			else if (t.isVisited && t.isDangerous) {
			//else if (t.isDangerous) {
				//canvas->SetPrimaryColorUint(0xff0000);
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
		canvas->SetPrimaryColorUint(0xffffff);
		canvas->DrawCString("You Won!", GAMERESOLUTIONX / (float)(2), GAMERESOLUTIONY / (float)(2));
	}
}

struct RowCol {
	int r;
	int c;
};

void RevealBoard() {
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			if (t.isDangerous) {
				t.isFlagged = true;
			}
			else {
				t.isVisited = true;
			}
		}
	}
}

bool SolveBoardIteration() {
	bool isBoardDataChanged = false;
	std::vector<RowCol> unvisitedNeighbours;
	std::vector<RowCol> unflaggedAndUnvistedNeighbours;
	unvisitedNeighbours.reserve(10);
	unflaggedAndUnvistedNeighbours.reserve(10);
	//if dnc == number of unvisited neighbours: flag the neighbours
	//if dnc == number of flagged neighbours: open the unvisited neighbours
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			if (t.isVisited == false) continue;
			unvisitedNeighbours.clear();
			int uvnc = 0;
			for (int dr = -1; dr <= 1; dr++) {
				for (int dc = -1; dc <= 1; dc++) {
					if (IsInvalidRowCol(i + dr, j + dc) || (dc == 0 && dr == 0)) {
						continue;
					}
					if (board[i + dr][j + dc].isVisited == false) {
						++uvnc;
						unvisitedNeighbours.push_back({ i + dr,j + dc });
					}
				}
			}
			//std::cout << t.dangerousNeighbourCount << " " << uvnc << " / " << fnc << "\n";
			if (t.dangerousNeighbourCount == uvnc) {
				for (const auto& rc : unvisitedNeighbours) {
					if (!board[rc.r][rc.c].isFlagged) {
						board[rc.r][rc.c].isFlagged = true;
						isBoardDataChanged = true;
					}
				}
			}
		}
	}

	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			Tile& t = board[i][j];
			if (t.isVisited == false) continue;
			unflaggedAndUnvistedNeighbours.clear();
			int fnc = 0;
			for (int dr = -1; dr <= 1; dr++) {
				for (int dc = -1; dc <= 1; dc++) {
					if (IsInvalidRowCol(i + dr, j + dc) || (dc == 0 && dr == 0)) {
						continue;
					}

					if (board[i + dr][j + dc].isFlagged == true) {
						++fnc;
					}
					if (board[i + dr][j + dc].isVisited == false &&
						board[i + dr][j + dc].isFlagged == false
						) {
						unflaggedAndUnvistedNeighbours.push_back({ i + dr,j + dc });
					}
				}
			}
			//std::cout << t.dangerousNeighbourCount << " " << uvnc << " / " << fnc << "\n";
			if (t.dangerousNeighbourCount == fnc) {
				for (const auto& rc : unflaggedAndUnvistedNeighbours) {
					if (!board[rc.r][rc.c].isVisited) {
						MarkBoardRowCol(rc.r, rc.c, Opcode::OPEN_TILE);
						isBoardDataChanged = true;
					}
				}
			}
		}
	}
	return isBoardDataChanged;
}

void SolveBoard() {
	if (isGameOver || isGameWon) {
		std::cout << "Game over!\n";
		std::cout << "Revealing Board..\n";
		RevealBoard();
		return;
	}
	std::cout << "Finding deterministic solution...\n";
	while (SolveBoardIteration()) {
		;
	}
	CheckIfGameWon();
	if (!isGameWon) {
		std::cout << "Cannot solve from this data...\n";
	}
	else {
		std::cout << "Solved.\n";
	}
}


void InitUi() {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();
	solveButton = uim->CreateWidget<Button>();
	solveButton->SetPosition(GAMERESOLUTIONX - 130, GAMERESOLUTIONY / 3);
	solveButton->SetSize(128, 32);
	solveButton->SetString("Pick Them");
	solveButton->SetCallback([=](int id) {
		SolveBoard();
		});

	nextButton = uim->CreateWidget<Button>();
	nextButton->SetPosition(GAMERESOLUTIONX-130, GAMERESOLUTIONY - 34);
	nextButton->SetSize(128, 32);
	nextButton->SetString("Next");
	nextButton->SetCallback([=](int id) {
		InitBoard();
		});
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
	uim->Display(canvas);
}

void HandleInput(const SDL_Event* sdlEvent) {
	using namespace spn::rmgui;
	if (lockInput) {
		return;
	}

	UiEvent uie;
	TranslateSdlEvent(sdlEvent, uie);
	bool hasUiConsumedEvent = uim->HandleUiEvent(uie);
	if (hasUiConsumedEvent) {
		return;
	}

	switch (uie.eventType) {
	case UiEventType::MouseUp:
		if (uie.mouseButton == MouseButton::Left) {
			if (!isGameWon && !isGameOver) {
				MarkBoard(uie.mouseX, uie.mouseY, Opcode::OPEN_TILE);
			}
		}
		else {
			if (!isGameWon && !isGameOver) {
				MarkBoard(uie.mouseX, uie.mouseY, Opcode::FLAG_TILE);
			}
		}
		break;
	case UiEventType::ActionKeyUp:
		//std::cout << "action\n";
		if (uie.keyCode == KeyCode::Right) {
			InitBoard();
		}
		else if (uie.keyCode == KeyCode::Down) {
			SolveBoard();
		}
		break;
	}
	//std::cout << "*";
}


int main(int argc, char* argv[])
{
	spn::SpinachCore sc(GAMERESOLUTIONX, GAMERESOLUTIONY, "../res/"); //Note: 3rd argument is path rel. to build folder
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	InitBoard();
	InitUi();
	
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