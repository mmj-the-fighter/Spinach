#ifndef BLOCKSGAME_H
#define BLOCKSGAME_H

#include <unordered_map>
#include <string>

#define MAXPATTERNS 7
#define MAXROTATIONS 4
#define PATTERNMAXROWS 4
#define PATTERNMAXCOLS 4
#define BOARDMAXROWS 16+1
#define BOARDMAXCOLS 12

namespace spn {
	class SpinachCore;
	class Canvas;
}
template <typename T>
class CircularQueue;

struct TileSprite {
	int x;
	int y;
	int tetrominoeIndex;
	int rotationIndex;
	unsigned int pattern;
	unsigned int color;
	bool isCollided;
};

enum GameState {
	GAME_PAUSED,
	GAME_OVER,
	GAME_RUNNING,
};


class BlocksInLineGame
{
public:
	BlocksInLineGame();
	~BlocksInLineGame();
	TileSprite CreateNewTile();
	void Init(spn::SpinachCore* sc);
	void OnMoveLeft();
	void OnMoveRight();
	void OnRotateCCW();
	void OnRotateCW();
	void OnFallFast();
	void OnPause();
	void OnUndo();
	void OnRestart();
	void UpdateAndRender(spn::Canvas* canvas);
	void SetDebugEnabled(bool flag);
	void ClearBoard();

private:
	void Render(spn::Canvas* canvas);
	void Update(float dt);
	bool Spawn();
	void FindAllPatterns();
	void PopulatePattern(unsigned int pattern, unsigned int patternGrid[PATTERNMAXROWS][PATTERNMAXCOLS]);
	int GetPatternUintFromPatternGrid(unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	void TransferPatternGridToBoard(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	bool CanRenderPatternGridWithoutOverlapAndGoingOutOfBounds(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	bool CanRenderPatternGridWithoutOverlap(int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	void RenderPatternGrid(spn::Canvas* canvas, int x, int y, unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	void EliminateBoardRow(int rowIndex);
	bool ClearAllTilesInLine();
	bool FallDown();
	void PrepareScore();
	bool canDebug = false;
	TileSprite currentTile;
	bool scoreChanged = false;
	CircularQueue<TileSprite>* queue;
	//unsigned int tetrominoes[MAXPATTERNS][MAXROTATIONS];

	//patterns suggested by AI
	unsigned short tetrominoes[MAXPATTERNS][MAXROTATIONS] =
	{
		// O (square)
		{
			0b0000011001100000,
			0b0000011001100000,
			0b0000011001100000,
			0b0000011001100000
		},

		// I
		{
			0b0010001000100010, // vertical
			0b0000000011110000, // horizontal
			0b0010001000100010,
			0b0000000011110000
		},

		// T
		{
			0b0000011100100000,
			0b0000001001100010,
			0b0000001001110000,
			0b0000001000110010
		},

		// J
		{
			0b0000011101000000,
			0b0000001100100010,
			0b0000000101110000,
			0b0000001000100110
		},

		// L
		{
			0b0000011100010000,
			0b0000001000100011,
			0b0000010001110000,
			0b0000001100100010
		},

		// Z
		{
			0b0000011000110000,
			0b0000001001100100,
			0b0000011000110000,
			0b0000001001100100
		},

		// S
		{
			0b0000001101100000,
			0b0000001000110001,
			0b0000001101100000,
			0b0000001000110001
		}
	};

	spn::SpinachCore* spinachCore;
	char patternChar[7] = { 'O','I','T','J','L','Z','S' };

	//colors suggested by AI// ------------------------------------------------------------
// OpenAI Generated Code - Free to use, modify, distribute
// ------------------------------------------------------------

	unsigned int colors[MAXPATTERNS] = {
		0xCC3333, // warm red
		0xFF6633, // orange
		0xFF9933, // amber
		0xCC6633, // burnt orange
		0xFFCC33, // warm yellow
		0xCC3366, // warm magenta
		0x993333  // deep brown-red
	};

	//unsigned int boardGridColor = 0x333333; // dark neutral gray
	unsigned int textColor = 0xffffff; // warm olive gray
	unsigned int boardGridColor = 0x666633; // warm olive gray
	unsigned int previewGridColor = 0x666666; // lighter for separation
	unsigned int board[BOARDMAXROWS][BOARDMAXCOLS];
	unsigned int cleanboard[BOARDMAXROWS][BOARDMAXCOLS] = 
	{
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},

		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},

		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},

		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 0,0,0,0},
		{1,1,1,1, 1,1,1,1, 1,1,1,1}//dummyrow
	};
	void RenderPreviewBoard(spn::Canvas* canvas, const char* title, 
		TileSprite tile, int startX, int startY, 
		unsigned int pat[PATTERNMAXROWS][PATTERNMAXCOLS]);
	unsigned int currentTilePattern[PATTERNMAXROWS][PATTERNMAXCOLS];
	unsigned int nextTilePattern[PATTERNMAXROWS][PATTERNMAXCOLS];
	unsigned int oneDownTilePattern[PATTERNMAXROWS][PATTERNMAXCOLS];
	int previewX;
	int previewY;
	TileSprite nextTile;
	TileSprite oneDownTile;
	float lastFrameTime = 0;
	int tileWidth;
	int tileHeight;
	int boardStartX;
	int boardStartY;
	int desiredFps;
	int gravity;
	int gravityMultiplier;
	int score;
	char scoreText[256];
	char* previewName;
	const char* gameOverText = "GAME OVER";
	const char* gamePausedText = "PAUSED";
	GameState gameState;

	//AI generated better tetrominoe index generator
	int bag[7];
	int bagIndex = 7; // force initial refill
	void RefillBag();
	int GetNextTetrominoIndex();
};
#endif
