// Comment By: Manoj M J
// C++ implementation derived from the my JavaScript Tic-Tac-Toe engine.
// AI tools were used for code translation and adaptation.

#ifndef TICTACTOEENGINE_H
#define TICTACTOEENGINE_H

enum PlayerType{
    HUMAN = 1,
    COMPUTER = 7,
    EMPTY = 0,
    INVALID = -1
};
enum GameResult {
    WON,
    LOST,
    DRAWN,
    ONGOING
};

//AI generated class
class KnuthRng
{
public:
    explicit KnuthRng(unsigned int seed = 1)
        : state(seed)
    {
    }

    void setSeed(int seed) {
        state = seed;
    }

    unsigned int Next()
    {
        state = state * 1664525u + 1013904223u;
        return state;
    }

    int NextInt(int maxExclusive)
    {
        return static_cast<int>(Next() % maxExclusive);
    }

private:
    unsigned int state;
};

class TicTacToeEngine
{
public:
	void Init(int seed) {
        rng.setSeed(seed);
        for (int i = 0; i < 9; i++) {
            board[i] = EMPTY;
        }
        board[9] = 0;
	}

	bool PlayHumanMove(int index) {
        if (index>=0 && index<9 && board[index] == EMPTY && board[9] < 9) {
            board[index] = HUMAN;
            ++board[9];
            return true;
        }
        else {
            return false;
        }
	}

	int PlayAiMove() {
        int index = FindTheBestAiMove();
        if (index != INVALID) {
            board[index] = COMPUTER;
            ++board[9];
            return index;
        }
        else {
            return INVALID;
        }
	}

    bool Toss() {
        return rng.NextInt(2) == 1;
    }

    GameResult GetGameResult() {
        if (CheckWin(COMPUTER)) {
            return LOST;
        }
        else if (CheckWin(HUMAN)) {
            return WON;
        }
        else if (board[9] == 9) {
            return DRAWN;
        }
        else {
            return ONGOING;
        }
    }

private:
   inline bool CheckWin(int player) {
        return (board[0] == player && board[1] == player && board[2] == player) ||
            (board[3] == player && board[4] == player && board[5] == player) ||
            (board[6] == player && board[7] == player && board[8] == player) ||
            (board[0] == player && board[3] == player && board[6] == player) ||
            (board[1] == player && board[4] == player && board[7] == player) ||
            (board[2] == player && board[5] == player && board[8] == player) ||
            (board[0] == player && board[4] == player && board[8] == player) ||
            (board[2] == player && board[4] == player && board[6] == player);
    }

    int MinimaxAlphaBeta(bool maximizingPlayer, int depth,int alpha, int beta) {

        if (CheckWin(COMPUTER)) {
            return WINNINGSCORE - depth;
        }
        else if (CheckWin(HUMAN)) {
            return -(WINNINGSCORE - depth);
        }
        
        if (board[9] == 9) { //board[9] is count of coins placed
            return 0;
        }

        if (maximizingPlayer) {
            int best = -TTT_INFINITY;

            //Generate each move for maximizing player(computer)
            for (int i = 0; i < 9; ++i) {
                if (board[i] == EMPTY) {
                    //simulate 
                    //play the valid move
                    board[i] = COMPUTER;
                    ++board[9]; //increment count
                    //check the score from the perspective of human player
                    int value = MinimaxAlphaBeta(false, depth + 1,alpha,beta);
                    if (value > best) {
                        best = value;
                    }
                    //undo move
                    board[i] = EMPTY;
                    --board[9]; 
                    if (best > alpha) {
                        alpha = best;
                    }
                    if (alpha >= beta)
                        break;

                }
            }
            return best;
        }
        else {
            int best = TTT_INFINITY;
            //Generate each move for minimizing player(human)
            for (int i = 0; i < 9; ++i) {
                if (board[i] == EMPTY) {
                    //simulate
                    //play the valid move
                    board[i] = HUMAN;
                    ++board[9]; 
                    //check the score from the perspective of computer player
                    int value = MinimaxAlphaBeta(true, depth + 1,alpha,beta);
                    if (value < best) {
                        best = value;
                    }
                    //undo move
                    board[i] = EMPTY; 
                    --board[9];
                    if (best < beta) {
                        beta = best;
                    }
                    if (beta <= alpha)
                        break;
                }
            }
            return best;
        }
    }
        
    int FindTheBestAiMove() {
        if (board[9] == 0) {
            if (rng.NextInt(7) == 0)
                return 4;
            else
                return RandomCorner();
        }
        int bestVal = -TTT_INFINITY;
        int bestIdx = INVALID;
        for (int i = 0; i < 9; ++i) {
            if (board[i] == EMPTY) {
                board[i] = COMPUTER;
                ++board[9];
                int val = MinimaxAlphaBeta(false, 0, -TTT_INFINITY, TTT_INFINITY);
                board[i] = EMPTY;
                --board[9];
                if (val > bestVal) {
                    bestVal = val;
                    bestIdx = i;
                }
            }
        }
        return bestIdx;
    }

    int RandomCorner() {
        int corners[] = { 0, 2, 6, 8 };
        return corners[rng.NextInt(4)];
    }

    const int WINNINGSCORE = 1000;
    const int TTT_INFINITY = 32768;
	int board[10];
    KnuthRng rng;
};


#endif
