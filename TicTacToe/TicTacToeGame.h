#pragma once
#include <stack>
#include <unordered_set>

struct PlayerMove
{
	int RowNumber;
	int ColNumber;
	PlayerMove() :
		RowNumber(0), ColNumber(0)
	{
	}
	PlayerMove(int row, int col) :
		RowNumber(row), ColNumber(col)
	{}
	bool operator == (const PlayerMove& other) const { return RowNumber == other.RowNumber && ColNumber == other.ColNumber; }
};

//For using unoredered_set with custom struct
namespace std {
	template <>
	struct hash<PlayerMove>
	{
		std::size_t operator()(const PlayerMove& k) const
		{
			using std::size_t;
			using std::hash;

			// Compute individual hash values for first,
			// second and third and combine them using XOR
			// and bit shifting:

			return
				((hash<int>()(k.ColNumber) << 1) >> 1)
				^ (hash<int>()(k.RowNumber) << 1);
		}
	};
}
class TicTacToeGame
{
public:
	enum GameState {
		NOT_INIT,
		PLAYER_ONE_TURN,
		PLAYER_TWO_TURN,
		PLAYER_ONE_WIN,
		PLAYER_TWO_WIN,
		DRAW
	};
	
private:
	enum GridState {
		EMPTY,
		PLAYER_ONE,
		PLAYER_TWO,
	};

	

	std::stack<PlayerMove> playerMoves[2];

	GameState m_gameState;
	GridState* m_board;
	std::unordered_set<PlayerMove> m_winningMoves;
	int* m_rowScoreCache[2];
	int* m_colScoreCache[2];
	int* m_diaScoreCache[2];
	int* m_antidiaScoreCache[2];
	int m_rows;
	int m_cols;
	int m_k;
	int m_turns;

	//Helpers
	bool SetBoardState(int num_row, int num_col, GridState newState);
	bool GetBoardState(int num_row, int num_col, GridState& state);
	int TranslateIntoDiagonalIndex(int num_row, int num_col, bool isAnti = false);

	//Check if the game should end
	bool ShouldGameEnd(int playerIndex, const PlayerMove& lastMove);
	bool UndoMove(int playerIndex);
	bool CheckForInput(const PlayerMove& playerMove, bool & shouldUndo);

public:
	
	TicTacToeGame() :
		m_gameState(NOT_INIT),
		m_board(nullptr),
		m_turns(0),
		m_rows(0),
		m_cols(0),
		m_k(0)
	{
		m_rowScoreCache[0] = m_rowScoreCache[1] = nullptr;
		m_colScoreCache[0] = m_colScoreCache[1] = nullptr;
		m_diaScoreCache[0] = m_diaScoreCache[1] = nullptr;
		m_antidiaScoreCache[0] = m_antidiaScoreCache[1] = nullptr;
	}
	~TicTacToeGame() { CleanUp(); }
	bool Initialize(int m, int n, int k);
	

	bool Run();

	//Clean up memory
	void CleanUp();

	//Show the board to the screen
	void ShowBoard();

	GameState GetGameState() { return m_gameState; }

	void Restart()
	{
		CleanUp();
		m_gameState = NOT_INIT;
	}
};

