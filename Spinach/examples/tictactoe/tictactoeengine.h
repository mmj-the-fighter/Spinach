// Comment By: Manoj M J
// C++ implementation derived from the my JavaScript Tic-Tac-Toe engine.
// AI tools were used for code translation and adaptation.

#ifndef TICTACTOEENGINE_H
#define TICTACTOEENGINE_H
#include <spn_profiler.h>

enum PlayerType{
    HUMAN = 1,
    COMPUTER = 2,
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

    void Restart() {
        for (int i = 0; i < 9; i++) {
            board[i] = EMPTY;
        }
        board[9] = 0;
    }

	void Init(int seed) {
        rng.setSeed(seed);
        ComputeCanonicalHashForAllBoardConfigs();
	}

    void DeInit() {
        spn::Profiler::GetInstance().Print();
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

   //A note on dihedral group symmetry based optimization(This idea is from a discussion with AI)
   //The symmetric board positions when evaluated with a minimax algorithm 
   // wil have equal values. This property can be used to reduce the 
   // number of moves to be evaluated and it will speed up the minimax search.
   // At init: all possible board configurations are inspected and all the symmetrical 
   // moves are generated and their board configurations are converted to hash codes.
   // The minimum hash code among them is taken as the canonical hash code. This code is assigned 
   // to each of the group.
   // A board configuration value array of maximum size of all the hashcodes is declared 
   // and initialized with negative values.
   // At board evaluation time, the whenever the AI is inspecting 
   // the winnning score of computer move, it converts the board to hashcode
   // and checks if the board configuration value array has a valid value for this and uses 
   // the standard memoization technique to speed up the search.
   //
   // The hash code can have only one continuous value between 0 and a maximum value.
   // Because we use continous values in the closed interval[0, 2] to represent 
   // empty, x input and o input and we encode the board configuration as a 9 digit 
   // base 3 number and 222222222base3 = 19683
   // Due to this reason, instead of a hash map, we can use 
   // the build configuration value array to store 
   // the results corresponding to a canonical hashcode by just using the 
   // canonical hash code as the index to the array.

   int ComputeHashcode(int board[10]) {
       int poweredValue = 1;
       int sum = 0;
       for (int i = 0; i < 9; i++) {
           sum += poweredValue * board[i];
     
           poweredValue *= 3;
       }
       return sum;
   }

   void DecodeHashcode(int hashCode, int resultBoard[10]) {
       int dividend = hashCode;
       for (int i = 0; i < 9; i++) {
           int remainder = dividend % 3;
           resultBoard[i] = remainder;
           dividend = dividend / 3;
       }
   }

   void ComputeCanonicalHashForAllBoardConfigs() {
       PROFILE_REST_OF_THE_CURRENT_BLOCK(0);

       //this array definition is ai generated
       int dihedralGroupPermutationTable[8][9] =
       {
           // Identity
           {
               0,1,2,
               3,4,5,
               6,7,8
           }, 

           // Rotate 90 degrees clockwise
           {
               6,3,0,
               7,4,1,
               8,5,2
           }, 
           
           // Rotate 180 degrees clockwise
           {
               8,7,6,
               5,4,3,
               2,1,0
           }, 
           
           // Rotate 270 degrees clockwise
           {
               2,5,8,
               1,4,7,
               0,3,6
           }, 
           
           // Reflect vertically about central vertical axis
           {
               2,1,0,
               5,4,3,
               8,7,6
           }, 
           
           // Reflect horizontally about central horizontal axis
           {
               6,7,8,
               3,4,5,
               0,1,2
           }, 
           
           // Reflect across main diagonal 048
           {
               0,3,6,
               1,4,7,
               2,5,8
           }, 
           
           // Reflect across anti diagonal 246
           {
               8,5,2,
               7,4,1,
               6,3,0
           }  
       };

       for (int i = 0; i < HASHCODESSIZE; i++) {
           boardConfigValues[i] = HASHNOTSET;
       }

       for (int i = 0; i < HASHCODESSIZE; i++) {
           canonicalHashCodes[i] = HASHNOTSET;
       }
       canonicalHashCodes[0] = 0;
       int trialBoard[10]; 

       //for each hashcode
       for (int i = 1; i < HASHCODESSIZE; i++) {
           if (canonicalHashCodes[i] == HASHNOTSET) {
               DecodeHashcode(i, trialBoard);
               int minHash = HASHCODESSIZE+1;
               int generatedHashCodeArray[8];
               for (int j = 0; j < 8; j++) {
                   //transform the booard
                   int transformedBoard[10];
                   for (int k = 0; k < 9; k++) {
                       //int index = dihedralGroupPermutationTable[j][k];
                       //transformedBoard[k] = trialBoard[index];
                       transformedBoard[k] = 
                           trialBoard[dihedralGroupPermutationTable[j][k] ];
                   }
                   int currentHash = ComputeHashcode(transformedBoard);
                   if (currentHash < minHash) {
                       minHash = currentHash;
                   }
                   generatedHashCodeArray[j] = currentHash;
               }
               for (int j = 0; j < 8; j++) {
                   //int index = generatedHashCodeArray[j];
                   //canonicalHashCodes[index] = minHash;
                   canonicalHashCodes[ generatedHashCodeArray[j] ] = minHash;
               }
           }
       }
   }

   int MinimaxAlphaBeta(bool maximizingPlayer, int depth,int alpha, int beta) {
       int hash = ComputeHashcode(board);
       int minhash = canonicalHashCodes[hash];

       if (boardConfigValues[minhash] != HASHNOTSET) {
           return boardConfigValues[minhash];
       }
        if (CheckWin(COMPUTER)) {
            if (boardConfigValues[minhash] == HASHNOTSET) {
                boardConfigValues[minhash] = WINNINGSCORE - depth;
            }

            return WINNINGSCORE - depth;
        }
        else if (CheckWin(HUMAN)) {
            if (boardConfigValues[minhash] == HASHNOTSET) {
                boardConfigValues[minhash] = -(WINNINGSCORE - depth);
            }
            return -(WINNINGSCORE - depth);
        }
        
        if (board[9] == 9) { //board[9] is count of coins placed
            if (boardConfigValues[minhash] == HASHNOTSET) {
                boardConfigValues[minhash] = 0;
            }
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
            if (boardConfigValues[minhash] == HASHNOTSET) {
                boardConfigValues[minhash] = best;
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
            if (boardConfigValues[minhash] == HASHNOTSET) {
                boardConfigValues[minhash] = best;
            }
            return best;
        }
    }
        
   int FindTheBestAiMove() {
       static int tag = 0;
       ++tag;
       PROFILE_REST_OF_THE_CURRENT_BLOCK(tag);
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

    static constexpr int WINNINGSCORE = 1000;
    static constexpr int TTT_INFINITY = 32768;
    static constexpr int HASHNOTSET = -TTT_INFINITY-5000;
    static constexpr int HASHCODESSIZE = 19683;
	
    int board[10];
    // board[0..8] = cells
    // board[9] = number of occupied cells
    
    int canonicalHashCodes[HASHCODESSIZE];
    int boardConfigValues[HASHCODESSIZE];
    KnuthRng rng;
};


#endif
