#include "TicTacToeGame.h"
#include <iostream>
#include <assert.h>

bool TicTacToeGame::SetBoardState(int num_row, int num_col, GridState newState)
{
	if (num_col >= m_cols || num_row >= m_rows || num_row < 0 || num_col < 0)
		return false;
	m_board[m_cols * num_row + num_col] = newState;
	return true;
}

bool TicTacToeGame::GetBoardState(int num_row, int num_col, GridState& state)
{
	if (num_col >= m_cols || num_row >= m_rows || num_row < 0 || num_col < 0)
		return false;
	state = m_board[m_cols * num_row + num_col];
	return true;
}

int TicTacToeGame::TranslateIntoDiagonalIndex(int num_row, int num_col, bool isAnti)
{
	//assert(num_col < m_cols && num_row < m_rows && num_row >= 0 && num_col >= 0);
	if (num_col >= m_cols || num_row >= m_rows || num_row < 0 || num_col < 0)
		return 0;
	if (isAnti)
	{
		//0 is top-left corner and goes right and goes down. a 4x4 board
		// * 0 1 2 3
		// / / / / 4
		// / / / / 5
		// / / / / 6
		// / / / / *
		return num_row + num_col;

	}
	else
	{
		//0 is bottom-left corner. and goes up and goes right. a 4x4 board
		//3 4 5 6 *
		//2 \ \ \ \
		//1 \ \ \ \
		//0 \ \ \ \
		//* \ \ \ \ 

		return m_rows - 1 - num_row + num_col;
	}
}

bool TicTacToeGame::Initialize(int m, int n, int k)
{
	assert(m_gameState == NOT_INIT);
	if (m <= 0 || n <= 0 || k > m || k > n)
	{
		std::cout << "Invalid M, N, K value.\n";
		return false;
	}
	m_rows = n;
	m_cols = m;
	m_k = k;
	m_board = new GridState[m * n];
	for (int i = 0; i < m * n; ++i)
		m_board[i] = GridState::EMPTY;
	m_colScoreCache[0] = new int[m_cols];
	m_colScoreCache[1] = new int[m_cols];
	for (int i = 0; i < m_cols; ++i)
		m_colScoreCache[0][i] = m_colScoreCache[1][i] = 0;
	m_rowScoreCache[0] = new int[m_rows];
	m_rowScoreCache[1] = new int[m_rows];
	for (int i = 0; i < m_rows; ++i)
		m_rowScoreCache[0][i] = m_rowScoreCache[1][i] = 0;
	m_diaScoreCache[0] = new int[m_rows + m_cols - 1];
	m_diaScoreCache[1] = new int[m_rows + m_cols - 1];
	m_antidiaScoreCache[0] = new int[m_rows + m_cols - 1];
	m_antidiaScoreCache[1] = new int[m_rows + m_cols - 1];
	for (int i = 0; i < m_rows + m_cols - 1; ++i)
		m_diaScoreCache[0][i] = m_diaScoreCache[1][i] = m_antidiaScoreCache[0][i] = m_antidiaScoreCache[1][i] = 0;

	m_gameState = PLAYER_ONE_TURN;
	return true;
}

bool TicTacToeGame::ShouldGameEnd(int playerIndex, const PlayerMove& lastMove)
{
	//reference https://stackoverflow.com/questions/4198955/how-to-find-the-winner-of-a-tic-tac-toe-game-of-any-size/34478665
	//update scores
	{
		int currentScore = 0;
		GridState playerState = playerIndex == 0 ? PLAYER_ONE : PLAYER_TWO;
		//row
		{
			int rowNumber = lastMove.RowNumber;
			//Get neighbor score
			GridState neighborRowState;
			int nextRowScore = 0, preRowScore = 0;
			if (GetBoardState(rowNumber + 1, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
				nextRowScore = m_rowScoreCache[playerIndex][rowNumber + 1];
			if (GetBoardState(rowNumber - 1, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
				preRowScore = m_rowScoreCache[playerIndex][rowNumber - 1];

			currentScore = preRowScore + nextRowScore + 1;

			m_rowScoreCache[playerIndex][rowNumber] = currentScore;
			if (currentScore >= m_k)
			{
				m_gameState = playerIndex == 0 ? PLAYER_ONE_WIN : PLAYER_TWO_WIN;
				//determine winning moves
				m_winningMoves.insert(lastMove);
				while (GetBoardState(++rowNumber, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, lastMove.ColNumber));
				rowNumber = lastMove.RowNumber;
				while (GetBoardState(--rowNumber, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, lastMove.ColNumber));
				return true;
			}
			//updates nearby scores
			while (GetBoardState(++rowNumber, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
				m_rowScoreCache[playerIndex][rowNumber] = currentScore;
			rowNumber = lastMove.RowNumber;
			while (GetBoardState(--rowNumber, lastMove.ColNumber, neighborRowState) && neighborRowState == playerState)
				m_rowScoreCache[playerIndex][rowNumber] = currentScore;
		}


		//col
		{
			int colNumber = lastMove.ColNumber;
			GridState neighborColState;
			int nextColScore = 0, preColScore = 0;
			if (GetBoardState(lastMove.RowNumber, colNumber + 1, neighborColState) && neighborColState == playerState)
				nextColScore = m_colScoreCache[playerIndex][colNumber + 1];
			if (GetBoardState(lastMove.RowNumber, colNumber - 1, neighborColState) && neighborColState == playerState)
				preColScore = m_colScoreCache[playerIndex][colNumber - 1];
			currentScore = preColScore + nextColScore + 1;
			m_colScoreCache[playerIndex][colNumber] = currentScore;
			if (currentScore >= m_k)
			{
				m_gameState = playerIndex == 0 ? PLAYER_ONE_WIN : PLAYER_TWO_WIN;
				//determine winning moves
				m_winningMoves.insert(lastMove);
				while (GetBoardState(lastMove.RowNumber, ++colNumber, neighborColState) && neighborColState == playerState)
					m_winningMoves.insert(PlayerMove(lastMove.RowNumber, colNumber));
				colNumber = lastMove.ColNumber;
				while (GetBoardState(lastMove.RowNumber, --colNumber, neighborColState) && neighborColState == playerState)
					m_winningMoves.insert(PlayerMove(lastMove.RowNumber, colNumber));
				return true;
			}
			//update nearby scores
			while (GetBoardState(lastMove.RowNumber, ++colNumber, neighborColState) && neighborColState == playerState)
				m_colScoreCache[playerIndex][colNumber] = currentScore;
			colNumber = lastMove.ColNumber;
			while (GetBoardState(lastMove.RowNumber, --colNumber, neighborColState) && neighborColState == playerState)
				m_colScoreCache[playerIndex][colNumber] = currentScore;
		}

		//anti diagonal
		{
			int colNumber = lastMove.ColNumber;
			int rowNumber = lastMove.RowNumber;
			GridState neighborDiaState;
			int nextScore = 0, prevScore = 0;
			if(GetBoardState(rowNumber + 1, colNumber - 1, neighborDiaState) && neighborDiaState == playerState)
			   nextScore = m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber + 1, colNumber - 1, true)];
			if (GetBoardState(rowNumber - 1, colNumber + 1, neighborDiaState) && neighborDiaState == playerState)
			   prevScore = m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber - 1, colNumber + 1, true)];
			int currentScore = prevScore + nextScore + 1;
			m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber, true)] = currentScore;

			if (currentScore >= m_k)
			{
				m_gameState = playerIndex == 0 ? PLAYER_ONE_WIN : PLAYER_TWO_WIN;
				//determine winning moves
				m_winningMoves.insert(lastMove);
				while (GetBoardState(++rowNumber, --colNumber, neighborDiaState) && neighborDiaState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, colNumber));
				rowNumber = lastMove.RowNumber;
				colNumber = lastMove.ColNumber;
				while (GetBoardState(--rowNumber, ++colNumber, neighborDiaState) && neighborDiaState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, colNumber));
				return true;
			}
			//update nearby scores
			while (GetBoardState(++rowNumber, --colNumber, neighborDiaState) && neighborDiaState == playerState)
				m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber, true)] = currentScore;
			rowNumber = lastMove.RowNumber;
			colNumber = lastMove.ColNumber;
			while (GetBoardState(--rowNumber, ++colNumber, neighborDiaState) && neighborDiaState == playerState)
				m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber, true)] = currentScore;
		}

		//diagonal
		{
			int colNumber = lastMove.ColNumber;
			int rowNumber = lastMove.RowNumber;
			GridState neighborDiaState;
			int nextScore = 0, prevScore = 0;
			if (GetBoardState(rowNumber + 1, colNumber + 1, neighborDiaState) && neighborDiaState == playerState)
				nextScore = m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber + 1, colNumber + 1)];
			if (GetBoardState(rowNumber - 1, colNumber - 1, neighborDiaState) && neighborDiaState == playerState)
				prevScore = m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber - 1, colNumber - 1)];
			int currentScore = prevScore + nextScore + 1;
			m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber)] = currentScore;

			if (currentScore >= m_k)
			{
				m_gameState = playerIndex == 0 ? PLAYER_ONE_WIN : PLAYER_TWO_WIN;
				//determine winning moves
				m_winningMoves.insert(lastMove);
				while (GetBoardState(++rowNumber, ++colNumber, neighborDiaState) && neighborDiaState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, colNumber));
				rowNumber = lastMove.RowNumber;
				colNumber = lastMove.ColNumber;
				while (GetBoardState(--rowNumber, --colNumber, neighborDiaState) && neighborDiaState == playerState)
					m_winningMoves.insert(PlayerMove(rowNumber, colNumber));
				return true;
			}
			//update nearby scores
			while (GetBoardState(++rowNumber, ++colNumber, neighborDiaState) && neighborDiaState == playerState)
				m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber)] = currentScore;
			rowNumber = lastMove.RowNumber;
			colNumber = lastMove.ColNumber;
			while (GetBoardState(--rowNumber, --colNumber, neighborDiaState) && neighborDiaState == playerState)
				m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(rowNumber, colNumber)] = currentScore;
		}
	}
	//Check for draw
	if (m_turns == m_rows * m_cols)
	{
		m_gameState = DRAW;
		return true;
	}
	return false;
}

bool TicTacToeGame::UndoMove(int playerIndex)
{
	assert(playerIndex == 0 || playerIndex == 1);
	if (!playerMoves[playerIndex].empty())
	{
		PlayerMove moveToUndo = playerMoves[playerIndex].top();
		//set board state
		if (!SetBoardState(moveToUndo.RowNumber, moveToUndo.ColNumber, EMPTY))
			return false;
		//update cached scores
		m_colScoreCache[playerIndex][moveToUndo.ColNumber] = 0;
		m_rowScoreCache[playerIndex][moveToUndo.RowNumber] = 0;
		m_diaScoreCache[playerIndex][TranslateIntoDiagonalIndex(moveToUndo.RowNumber, moveToUndo.ColNumber, false)] = 0;
		m_antidiaScoreCache[playerIndex][TranslateIntoDiagonalIndex(moveToUndo.RowNumber, moveToUndo.ColNumber, true)] = 0;
		//pop the move
		playerMoves[playerIndex].pop();
	}
	else
		return false;
	return true;
}

bool TicTacToeGame::CheckForInput(const PlayerMove& playerMove, bool& shouldUndo)
{
	if (playerMove.RowNumber < 0 || playerMove.ColNumber < 0)
	{
		shouldUndo = true;
		return true;
	}
	GridState currentGridState;
	if (GetBoardState(playerMove.RowNumber, playerMove.ColNumber, currentGridState))
	{
		if (currentGridState != EMPTY)
		{
			std::cout << "Slot (" << playerMove.RowNumber + 1 << ',' << playerMove.ColNumber + 1 << ") already taken.\n";
			return false;
		}
	}
	else
	{
		std::cout << "Invalid Row / Column number.\n";
		return false;
	}

	return true;
}

bool TicTacToeGame::Run()
{
	assert(m_gameState == PLAYER_ONE_TURN || m_gameState == PLAYER_TWO_TURN);
	int currentTurnPlayerIndex = m_gameState == PLAYER_ONE_TURN ? 0 : 1;

	ShowBoard();
	std::cout << "Player " << currentTurnPlayerIndex + 1 << " Enter your move: \n";
	std::cout << "Enter 0 or '-' to revert Player " << (currentTurnPlayerIndex + 1) % 2 + 1<< " Last Move.\n";
	PlayerMove moveThisTurn;
	bool shouldUndoMove = false;
	do {
		std::cout << "Row Number (start from 1): ";
		std::cin >> moveThisTurn.RowNumber;
		moveThisTurn.RowNumber--;

		std::cout << "Column Number (start from 1): ";
		std::cin >> moveThisTurn.ColNumber;
		moveThisTurn.ColNumber--;
		std::cin.clear();
	} while (!CheckForInput(moveThisTurn, shouldUndoMove));

	if (shouldUndoMove)
	{
		if (UndoMove((currentTurnPlayerIndex + 1) % 2))
		{
			std::cout << "Undo successfully.\n";
			m_turns--;
			//roll back turns
			m_gameState = m_gameState == PLAYER_ONE_TURN ? PLAYER_TWO_TURN : PLAYER_ONE_TURN;
		}
		else
			std::cout << "No moves to undo.\n";

		return true;
	}

	if (!SetBoardState(moveThisTurn.RowNumber, moveThisTurn.ColNumber, currentTurnPlayerIndex == 0 ? PLAYER_ONE : PLAYER_TWO))
	{
		std::cout << "Error when setting board state.\n";
		return false;
	}

	playerMoves[currentTurnPlayerIndex].push(moveThisTurn);

	m_turns++;
	if (ShouldGameEnd(currentTurnPlayerIndex, moveThisTurn))
	{
		std::cout << "Game Ended.\n";
		return false;
	}

	m_gameState = currentTurnPlayerIndex == 0 ? PLAYER_TWO_TURN : PLAYER_ONE_TURN;
	return true;
}

void TicTacToeGame::CleanUp()
{
	if (m_board)
	{
		delete[] m_board;
		m_board = nullptr;
	}

	if (m_rowScoreCache[0])
	{
		delete[] m_rowScoreCache[0];
		m_rowScoreCache[0] = nullptr;
	}

	if (m_rowScoreCache[1])
	{
		delete[] m_rowScoreCache[1];
		m_rowScoreCache[1] = nullptr;
	}

	if (m_colScoreCache[0])
	{
		delete[] m_colScoreCache[0];
		m_colScoreCache[0] = nullptr;
	}

	if (m_colScoreCache[1])
	{
		delete[] m_colScoreCache[1];
		m_colScoreCache[1] = nullptr;
	}
	if (m_diaScoreCache[0])
	{
		delete[] m_diaScoreCache[0];
		m_diaScoreCache[0] = nullptr;
	}

	if (m_diaScoreCache[1])
	{
		delete[] m_diaScoreCache[1];
		m_diaScoreCache[1] = nullptr;
	}

	if (m_antidiaScoreCache[0])
	{
		delete[] m_antidiaScoreCache[0];
		m_antidiaScoreCache[0] = nullptr;
	}

	if (m_antidiaScoreCache[1])
	{
		delete[] m_antidiaScoreCache[1];
		m_antidiaScoreCache[1] = nullptr;
	}

	playerMoves[0] = playerMoves[1] = {};

	m_winningMoves.clear();
}

void TicTacToeGame::ShowBoard()
{
	std::cout << std::endl;
	for (int rows = 0; rows < m_rows; ++rows)
	{
		for (int cols = 0; cols < m_cols; ++cols)
		{
			GridState thisState = m_board[rows * m_cols + cols];
			if (m_gameState == PLAYER_ONE_WIN)
			{
				if (thisState == EMPTY)
					std::cout << "* ";
				else if (thisState == PLAYER_TWO)
					std::cout << "X ";
				else
				{
					PlayerMove thisMove(rows, cols);
					if (m_winningMoves.find(thisMove) != m_winningMoves.end())
						std::cout << "@ ";
					else
						std::cout << "O ";
				}
				//std::cout << (thisState == EMPTY ? '*' : (thisState == PLAYER_ONE ? 'O' : 'X')) << ' ';
			}
			else if (m_gameState == PLAYER_TWO_WIN)
			{
				if (thisState == EMPTY)
					std::cout << "* ";
				else if (thisState == PLAYER_ONE)
					std::cout << "O ";
				else
				{
					PlayerMove thisMove(rows, cols);
					if (m_winningMoves.find(thisMove) != m_winningMoves.end())
						std::cout << "# ";
					else
						std::cout << "X ";
				}
			}
			else
				std::cout << (thisState == EMPTY ? '*' : (thisState == PLAYER_ONE ? 'O' : 'X')) << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
