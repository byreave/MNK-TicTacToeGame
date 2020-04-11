#include "TicTacToeGame.h"
#include <iostream>

void ShowGameResult(TicTacToeGame::GameState whoWon)
{
	switch (whoWon)
	{
	case TicTacToeGame::PLAYER_ONE_WIN:
		std::cout << "Player One Wins!\n";
		break;
	case TicTacToeGame::PLAYER_TWO_WIN:
		std::cout << "Player Two Wins!\n";
		break;
	case TicTacToeGame::DRAW:
		std::cout << "Draw!\n";
		break;
	default:
		break;
	}
}

bool CheckForRestart()
{
	std::cout << "Do you want to have another go? (1 : yes, 0: no.)\n";
	int restart;
	std::cin >> restart;
	return restart == 1;
}

int main()
{
	TicTacToeGame theGame;
	int m = 3, n = 3, k = 3;
	while (theGame.GetGameState() == TicTacToeGame::NOT_INIT)
	{
		do {
			std::cout << "Welcome to the TicTacToe game. \n"
				<< "Please enter the m, n, k you want to play with.\n";
			std::cin >> m >> n >> k;
			std::cin.clear();
		} while (!theGame.Initialize(m, n, k));

		while (theGame.Run());

		ShowGameResult(theGame.GetGameState());
		theGame.ShowBoard();

		if (CheckForRestart())
			theGame.Restart();
		else
			theGame.CleanUp();
	}

	std::cout << "Thanks for playing!\n";

	return 0;
}