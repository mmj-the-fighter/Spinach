#include <iostream>

#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <spn_core.h>
#include <spn_canvas.h>
#include <game.h>
#include <spn_rng.h>
#include <cq.h>

BlocksInLineGame::BlocksInLineGame()
{
	tileWidth = 32;
	tileHeight = 32;
	boardStartX = 32;
	boardStartY = 32;
	score = 0;
	gravityMultiplier = 1;
	tileQueue = new CircularQueue<TileSprite>();
}

BlocksInLineGame::~BlocksInLineGame()
{
	if (tileQueue) {
		delete tileQueue;
	}
}

void BlocksInLineGame::Init(spn::SpinachCore* sc) {
	spinachCore = sc;
	srand(static_cast<unsigned int>(time(nullptr)));
	spn::Canvas* canvas = sc->GetCanvas();
	canvas->SetClearColor(0, 0, 128);

	//ww = sc->GetCanvas()->GetWidth();
	//wh = sc->GetCanvas()->GetHeight();
	sc->SetWindowTitle("STJILZO Blocks In Line");
	FindAllPatterns();
	float x, y;
	float left = boardStartX;
	float top = boardStartY;
	float right = boardStartX + BOARDMAXCOLS * tileWidth;
	float bottom = boardStartY + (BOARDMAXROWS-1) * tileHeight;
	
	CenterAlignTextPos(canvas, gameOverText, x, y, left, top, right, bottom);
	gameOverTextX = static_cast<int>(x);
	gameOverTextY = static_cast<int>(y);

	CenterAlignTextPos(canvas, gamePausedText, x, y, left, top, right, bottom);
	gamePausedTextX = static_cast<int>(x);
	gamePausedTextY = static_cast<int>(y);
	OnRestart();
}

void BlocksInLineGame::OnRestart() {
	static int count = 0;
	desiredFps = 1;
	spinachCore->SetTargetFramesPerSecond(desiredFps);
	spinachCore->LockFps(true);
	score = 0;
	PrepareScore();
	gameState = GAME_RUNNING;
	gravity = tileHeight;
	ClearBoard();
	tileQueue->Clear();
	tileQueue->InQ(CreateNewTile());
	tileQueue->InQ(CreateNewTile());
	tileQueue->InQ(CreateNewTile());
	
	Spawn();
	if (currentTile.isCollided) {
		gameState = GAME_OVER;
	}
	if (count == 0) {
		++count;
		gameState = GAME_INTRO;
	}
}

void BlocksInLineGame::SetDebugEnabled(bool flag) {
	canDebug = flag;
}
TileSprite BlocksInLineGame::CreateNewTile(){
	TileSprite t;
	t.tetrominoeIndex = GetNextTetrominoIndex();
	t.color = colors[t.tetrominoeIndex];

	t.rotationIndex = spn::RandomGen::GetInstance().GenerateInRange(0, MAXROTATIONS - 1);
	t.pattern = tetrominoes[t.tetrominoeIndex][t.rotationIndex];
	//t.x = boardStartX + (BOARDMAXCOLS * tileWidth) / 2;
	t.x = spn::RandomGen::GetInstance().GenerateInRange(boardStartX, ((BOARDMAXCOLS-1) * tileWidth));
	t.x -= tileWidth;
	t.y = boardStartY - tileHeight * PATTERNMAXROWS;
	t.isCollided = false;
	return t;
}
bool BlocksInLineGame::Spawn()
{
	tileQueue->DelQ(currentTile);
	tileQueue->First(nextTile);
	tileQueue->Second(oneDownTile);
	tileQueue->InQ(CreateNewTile());
	PopulatePattern(nextTile.pattern, nextTilePattern);;
	PopulatePattern(oneDownTile.pattern, oneDownTilePattern);;
	PopulatePattern(currentTile.pattern, currentTilePattern);
	gravityMultiplier = 1;
	currentTile.isCollided = !CanRenderPatternGridWithoutOverlap(currentTile.x, currentTile.y, currentTilePattern);
	return currentTile.isCollided;
}

void BlocksInLineGame::PopulatePattern(unsigned int pattern, unsigned int patternGrid[PATTERNMAXROWS][PATTERNMAXCOLS])
{
	unsigned int pat = pattern;
	unsigned int mask = 0b1000000000000000;
	//inject current tile pattern to board based on the currentTilePosition
	for (int k = 0; k < 16; ++k) {
		int r = k / 4;
		int c = k % 4;
		if (pat & mask) {
			patternGrid[r][c] = 1;
		}
		else {
			patternGrid[r][c] = 0;
		}
		pat <<= 1;
	}
}

void BlocksInLineGame::ClearBoard()
{
	for (int i = 0; i < BOARDMAXROWS; i++) {
		for (int j = 0; j < BOARDMAXCOLS; j++) {
			board[i][j] = cleanboard[i][j];
		}
	}
}

void BlocksInLineGame::RenderPatternGrid(spn::Canvas* canvas, int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]) {
	int row = (y - boardStartY) / tileHeight;
	int col = (x - boardStartX) / tileWidth;

	for (int i = 0; i < PATTERNMAXROWS; i++) {
		for (int j = 0; j < PATTERNMAXCOLS; j++) {
			int p = pat[i][j];
			if (p == 0) {//pattern cell is empty
				continue;
			}
			int er = row + i;
			int ec = col + j;
			if (er >= 0 &&
				er < BOARDMAXROWS &&
				ec >= 0 &&
				ec < BOARDMAXCOLS) 
			{
				int y = boardStartY + er * tileHeight;
				int x = boardStartX + ec * tileWidth;
				canvas->SetPrimaryColorUint(boardGridColor);
				canvas->DrawRectangle(x, y, x + tileWidth, y + tileHeight);
				canvas->SetPrimaryColorUint(currentTile.color);
				canvas->DrawFilledRectangle(x + 4, y + 4, x + tileWidth - 4, y + tileHeight - 4);
			}
		}
	}
}
void BlocksInLineGame::TransferPatternGridToBoard(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS])
{
	int row = (y - boardStartY) / tileHeight;
	int col = (x - boardStartX) / tileWidth;
	for (int i = 0; i < PATTERNMAXROWS; i++) {
		for (int j = 0; j < PATTERNMAXCOLS; j++) {
			int er = row + i;
			int ec = col + j;
			int p = pat[i][j];
			if (p!=0 && er >= 0 && er < BOARDMAXROWS && ec >= 0 && ec < BOARDMAXCOLS) {
				board[er][ec] = currentTile.color;
			}
		}
	}
}
bool BlocksInLineGame::CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]) {
	int row = (y - boardStartY) / tileHeight;
	int col = (x - boardStartX) / tileWidth;
	
	for (int i = 0; i < PATTERNMAXROWS; i++) {
		for (int j = 0; j < PATTERNMAXCOLS; j++) {
			int p = pat[i][j];
			if (p == 0) {//pattern cell is empty
				continue;
			}
			int er = row + i;
			int ec = col + j;
			if (er >= 0 &&
				er < BOARDMAXROWS &&
				ec >= 0 &&
				ec < BOARDMAXCOLS) {
				if (board[er][ec] != 0) {
					return false;
				}
			}
			else {
				return false;
			}
		}
	}
	return true;
}

void BlocksInLineGame::RenderPreviewBoard(
	spn::Canvas* canvas,
	const char* title, 
	TileSprite tile, 
	int startX, int startY, 
	unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]) {
	int th = 3*tileHeight / 4;
	int tw = 3*tileWidth / 4;

	for (int row = 0, col = 0, i = 0; i < PATTERNMAXROWS; i++) {
		for (int j = 0; j < PATTERNMAXCOLS; j++) {
			int p = pat[i][j];
			int er = row + i;
			int ec = col + j;
			int y = startY + er * th;
			int x = startX + ec * tw;

			if (p != 0)
			{
				canvas->SetPrimaryColorUint(previewGridColor);
				canvas->DrawRectangle(x, y, x + tw, y + th);
				canvas->SetPrimaryColorUint(tile.color);
				canvas->DrawFilledRectangle(x + 2, y + 2, x + tw - 2, y + th - 2);
			}
			else {
				canvas->SetPrimaryColorUint(previewGridColor);
				canvas->DrawRectangle(x, y, x + tw, y + th);
			}
		}
	}
	canvas->SetPrimaryColorUint(textColor);
	char str[256] = "";
	sprintf(str, "%s: %c", title, patternChar[tile.tetrominoeIndex]);
	canvas->DrawCString(str, startX, startY - 24);
}


bool BlocksInLineGame::CanRenderPatternGridWithoutOverlap(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS])
{
	int row = (y - boardStartY) / tileHeight;
	int col = (x - boardStartX) / tileWidth;

	for (int i = 0; i < PATTERNMAXROWS; i++) {
		for (int j = 0; j < PATTERNMAXCOLS; j++) {
			int p = pat[i][j];
			int er = row + i;
			int ec = col + j;
			if (p != 0 && er >= 0 &&
				er < BOARDMAXROWS &&
				ec >= 0 &&
				ec < BOARDMAXCOLS) {
				if (board[er][ec] != 0) {
					return false;
				}
			}
		}
	}
	return true;
}

void BlocksInLineGame::FindAllPatterns()
{
}

int BlocksInLineGame::GetPatternUintFromPatternGrid(unsigned int patGrid[PATTERNMAXROWS][PATTERNMAXCOLS])
{
	int shiftamount = 12;
	int pattern = 0;
	for (int i = 0; i < PATTERNMAXROWS; ++i) {
		unsigned int rowpattern = 0xF;
		unsigned int rowmask = 0x1;
		for (int j = PATTERNMAXCOLS - 1; j >= 0; --j) {
			if (patGrid[i][j] == 0) {
				rowpattern ^= rowmask;
			}
			rowmask <<= 1;
		}
		pattern |= rowpattern << shiftamount;
		shiftamount -= 4;
	}
	return pattern;
}

void BlocksInLineGame::OnMoveLeft()
{
	if (gameState != GAME_RUNNING) {
		return;
	}
	if (CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(currentTile.x - tileWidth, currentTile.y, currentTilePattern)) {
		currentTile.x -= tileWidth;
	}
}

void BlocksInLineGame::OnMoveRight()
{
	if (gameState != GAME_RUNNING) {
		return;
	}
	if (CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(currentTile.x + tileWidth, currentTile.y, currentTilePattern)) {
		currentTile.x += tileWidth;
	}
}

void BlocksInLineGame::OnRotateCCW()
{
	if (gameState == GAME_INTRO) {
		OnRestart();
		return;
	}
	if (gameState != GAME_RUNNING) {
		return;
	}
	int trindex = currentTile.rotationIndex;

	++trindex;
	trindex %= MAXROTATIONS;
	unsigned int temp[PATTERNMAXROWS][PATTERNMAXCOLS];
	PopulatePattern(tetrominoes[currentTile.tetrominoeIndex][trindex], temp);
	if (CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(currentTile.x, currentTile.y, temp)) {
		currentTile.rotationIndex = trindex;
		for (int i = 0; i < PATTERNMAXROWS; i++) {
			for (int j = 0; j < PATTERNMAXCOLS; j++) {
				currentTilePattern[i][j] = temp[i][j];
			}
		}
	}
}

void BlocksInLineGame::OnRotateCW()
{
	if (gameState == GAME_INTRO) {
		OnRestart();
		return;
	}
	if (gameState != GAME_RUNNING) {
		return;
	}
	int trindex = currentTile.rotationIndex;
	--trindex;
	if (trindex < 0) {
		trindex = MAXROTATIONS - 1;
	}
	
	unsigned int temp[PATTERNMAXROWS][PATTERNMAXCOLS];
	PopulatePattern(tetrominoes[currentTile.tetrominoeIndex][trindex], temp);
	if (CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(currentTile.x, currentTile.y, temp)) {
		currentTile.rotationIndex = trindex;
		for (int i = 0; i < PATTERNMAXROWS; i++) {
			for (int j = 0; j < PATTERNMAXCOLS; j++) {
				currentTilePattern[i][j] = temp[i][j];
			}
		}
	}
}

void BlocksInLineGame::OnPause()
{
	if (gameState == GAME_RUNNING ) {
		gameState = GAME_PAUSED;
	}
	else if (gameState == GAME_PAUSED) {
		gameState = GAME_RUNNING;
	}
}

void BlocksInLineGame::OnFallFast()
{
	if (gameState != GAME_RUNNING) {
		return;
	}
	float dt = spinachCore->GetCanvas()->GetLastFrameTime();
	gravityMultiplier += 0.075*tileHeight * dt;
}


//OpenAI Generated Code
void BlocksInLineGame::RefillBag() {
	for (int i = 0; i < 7; ++i)
		bag[i] = i;

	// Fisher-Yates shuffle
	for (int i = 6; i > 0; --i) {
		int j = spn::RandomGen::GetInstance().GenerateInRange(0, i);
		std::swap(bag[i], bag[j]);
	}

	bagIndex = 0;
}

int BlocksInLineGame::GetNextTetrominoIndex() {
	if (bagIndex >= 7)
		RefillBag();

	return bag[bagIndex++];
}
bool BlocksInLineGame::FallDown() {
	int iterations = gravityMultiplier;
	bool collided = false;
	for(int i = 0; i < iterations; i++) {
		if (CanRenderPatternGridWithoutOverlap(currentTile.x, currentTile.y + tileHeight, currentTilePattern)) {
			currentTile.y += tileHeight;
		}
		else {
			collided = true;
			break;
		}
	}
	currentTile.isCollided = collided;
	return !collided;
}

void BlocksInLineGame::EliminateBoardRow(int rowIndex) {
	for (int j = 0; j < BOARDMAXCOLS; ++j) {
		for (int i = rowIndex; i >= 0; --i) {
			if (i == 0)
				board[i][j] = 0;
			else
				board[i][j] = board[i - 1][j];
		}
	}
}

bool BlocksInLineGame::ClearAllTilesInLine() {
	int count = 0;
	for (int k = BOARDMAXROWS - 2; k > 0; --k) {
		int j=0;
		while (board[k][j] != 0) {
			++j;
		}
		if (j >= BOARDMAXCOLS) {
			EliminateBoardRow(k);
			++count;
		}
	}
	score += count;
	if (count > 0) {
		PrepareScore();
	}
	return count > 0;
}

void BlocksInLineGame::Render(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->SaveColors();
	

	for (int i = 0; i < BOARDMAXROWS-1; ++i) {
		for (int j = 0; j < BOARDMAXCOLS; ++j) {
			int y = boardStartY + i * tileHeight;
			int x = boardStartX + j * tileWidth;
			unsigned int color = board[i][j];
			if (color != 0) {
				canvas->SetPrimaryColorUint(boardGridColor);
				canvas->DrawRectangle(x, y, x + tileWidth, y + tileHeight);
				canvas->SetPrimaryColorUint(color);
				canvas->DrawFilledRectangle(x + 4, y + 4, x + tileWidth - 4, y + tileHeight - 4);
			}
			else {
				canvas->SetPrimaryColorUint(boardGridColor);
				canvas->DrawRectangle(x, y, x + tileWidth, y + tileHeight);
			}
		}
	}
	
	RenderPatternGrid(canvas, currentTile.x, currentTile.y, currentTilePattern);
	int yy = tileHeight * 4;
	int off = tileHeight * 4 + 20;
	RenderPreviewBoard(canvas, "Running", currentTile, 500,yy, currentTilePattern);
	RenderPreviewBoard(canvas, "Next", nextTile, 500,yy+off, nextTilePattern);
	RenderPreviewBoard(canvas, "One-Down", oneDownTile, 500,yy+2*off, oneDownTilePattern);

	canvas->RestoreColors();
	canvas->DrawRectangle(
		boardStartX,
		boardStartY,
		boardStartX + (BOARDMAXCOLS * tileWidth),
		boardStartY + ((BOARDMAXROWS-1) * tileHeight)
	);

}

void BlocksInLineGame::Update(float dt) {
	FallDown();
	if (currentTile.isCollided) {
		TransferPatternGridToBoard(currentTile.x, currentTile.y, currentTilePattern);
		while (ClearAllTilesInLine());
		for (int i = 0; i < BOARDMAXCOLS; i++) {
			if (board[0][i] != 0) {
				gameState = GAME_OVER;
				break;
			}
		}
		if (gameState != GAME_OVER) {
			Spawn();
			if (currentTile.isCollided) {
				gameState = GAME_OVER;
			}
		}
	}
}

void BlocksInLineGame::CenterAlignTextPos(spn::Canvas* canvas, const char* text, float& tx, float& ty, float left, float top, float right, float bottom)
{
	float txtwt, txtht;
	canvas->GetCStringDisplaySize(text, txtwt, txtht);
	float bw = right - left;
	float bh = bottom - top;
	tx = left + (bw - txtwt) * 0.5;
	ty = top + (bh - txtht) * 0.5;
}

void BlocksInLineGame::PrepareScore() {
	sprintf(scoreText,"Clearances: %d", score);
}

void BlocksInLineGame::UpdateAndRender(spn::Canvas* canvas) {
	switch (gameState) 
	{
	case GAME_INTRO:
		//Render(canvas);
		canvas->Clear();
		canvas->DrawCString("Controls:", 100, 50);
		canvas->DrawCString("Move left - LEFT ARROW ", 100, 50+24);
		canvas->DrawCString("Move right - RIGHT ARROW ", 100, 50+2*24);
		canvas->DrawCString("Rotate clockwise - SPACE", 100, 50+3*24);
		canvas->DrawCString("Rotate counter clockwise - CTRL+SPACE", 100, 50+4*24);
		canvas->DrawCString("Restart - R", 100, 50+5*24);
		canvas->DrawCString("Pause - P", 100, 50+6*24);
		canvas->DrawCString("Press SPACE to start", 100, 50+9*24);
		break;
	case GAME_RUNNING:
		Update(canvas->GetLastFrameTime());
		Render(canvas);
		canvas->SetPrimaryColorUint(textColor);
		canvas->DrawCString(scoreText, 500, 40);
		break;
	case GAME_PAUSED:
		Render(canvas);
		canvas->SetPrimaryColorUint(textColor);
		canvas->DrawCString(gamePausedText, gamePausedTextX, gamePausedTextY);
		canvas->DrawCString(scoreText, 500, 40);
		break;
	case GAME_OVER:
		Render(canvas);
		canvas->SetPrimaryColorUint(textColor);
		canvas->DrawCString(gameOverText, gameOverTextX, gameOverTextY);
		canvas->DrawCString(scoreText, 500, 40);
		break;
	}
	canvas->SetPrimaryColorUint(textColor);
}