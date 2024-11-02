#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define P1_SYMBOL 'x'
#define P2_SYMBOL 'o'
#define EMPTY_TILE ' '

void startGame();
void resetGame();
void rowDeco(int row);
void renderGame();
void turnInfo();
void getInputs();
void markTile(int subGame, int subGameTile);
void fillSubGame(int subGame, char symbol);
bool verifyGame(char game[], char player);
void errorMessage(char msg[]);
void gameResult();

// game
char game[9][9];
char completedGames[9];
// game variables
int subGame = -1;
int subGameTile = 0;
int player = 1;
char playerIcon = P1_SYMBOL;
// victory
bool winSubGame = false;
bool winGame = false;
int subGamesFinished = 0;
int results = 0;
// after game
bool isPlaying = true;
char option;

int main() {
  while (isPlaying) {
    startGame();

    // play more?
    do {
      printf("Do you want to play more? (y/n) ");
      scanf("%c", &option);
      fflush(stdin);
      tolower(option);

      if (option != 'n' && option != 'y') errorMessage("type \'y\' for YES and \'n\' for NO");
    } while (option != 'n' && option != 'y');

    if (option == 'n') isPlaying = false;
  }

  return 0;
}

void startGame() {
  resetGame();

  while (!winGame) {
    renderGame();
    turnInfo();

    getInputs();

    markTile(subGame, subGameTile);

    if (subGamesFinished >= 3) winGame = verifyGame(completedGames, playerIcon);

    if (subGamesFinished == 9) {
      results = 0;
      break;
    }

    if (winGame) {
      results = player;
    } else {
      // condition of marked tile
      subGame = completedGames[subGameTile] == EMPTY_TILE ? subGameTile : -1;

      player = player == 1 ? 2 : 1;
      playerIcon = player == 1 ? P1_SYMBOL : P2_SYMBOL;
    }
  }

  // after playing
  renderGame();
  gameResult();
}

void resetGame() {
  subGame = -1;

  player = 1;
  playerIcon = P1_SYMBOL;

  winSubGame = false;
  winGame = false;
  subGamesFinished = 0;

  for (int i = 0; i < 9; i++) {
    fillSubGame(i, EMPTY_TILE);
    completedGames[i] = EMPTY_TILE;
  }
}

void rowDeco(int row) {
  for (int i = 0; i < 3; i++) {
    int subGameInRow = (i % 3) + (row / 3) * 3;

    printf(
      subGameInRow == subGame ||
      subGame == -1 &&
      completedGames[subGameInRow] == EMPTY_TILE
        ? "o---o---o---o"
        : "+---+---+---+"
    );

    printf(" "); // space between sub-games
  }
  printf("\n");
}

void renderGame() {
  printf("\e[1;1H\e[2J");

  printf("=========== SUPER TIC-TAC-TOE ===========\n");

  for (int row = 0; row < 9; row++) {
    rowDeco(row);

    printf("|");

    for (int column = 0; column < 9; column++) {
      printf(" %c ", game[(column / 3) + (row / 3) * 3][(column % 3) + (row % 3) * 3]);
      // column / 3 - get game column
      //    row / 3 - get game row
      // column % 3 - get sub-game column
      //    row % 3 - get sub-game row

      printf("|"); // division border

      if (column == 2 || column == 5)
        printf(" |"); // right/left border sub-game
    }

    printf("\n");

    if (row == 2 || row == 5) {
      rowDeco(row); // bottom border sub-game
    }
  }
  rowDeco(8); // bottom border
}

void turnInfo() {
  printf("------------- Player %i turn -------------\n", player);
  if (subGame != -1) printf("Playing on: %i\n", subGame + 1);
}

void getInputs() {
  do {
    int row = 0;
    int column = 0;

    // get sub-game
    while (subGame < 0 || subGame > 8 || completedGames[subGame] != EMPTY_TILE) {
      printf("Enter the sub-game (1~9): ");
      scanf("%i", &subGame);
      fflush(stdin);
      subGame--;

      if (subGame < 0 || subGame > 8) errorMessage("sub-game out of range");
      else if (completedGames[subGame] != EMPTY_TILE) errorMessage("sub-game already completed");
      else {
        renderGame();
        turnInfo();
      }
    }

    // get row
    do {
      printf("Enter the row (1~3): ");
      scanf("%i", &row); 
      fflush(stdin);
      row--;

      if (row < 0 || row > 2) errorMessage("row out of range");
    } while (row < 0 || row > 2);

    // get column
    do {
      printf("Enter the column (1~3): ");
      scanf("%i", &column); 
      fflush(stdin);
      column--;

      if (column < 0 || column > 2) errorMessage("column out of range");
    } while (column < 0 || column > 2);

    // if tile was already chosen
    subGameTile = row * 3 + column;
    if (game[subGame][subGameTile] != EMPTY_TILE) errorMessage("tile already chosen");
  } while (game[subGame][subGameTile] != EMPTY_TILE);
}

void markTile(int subGame, int subGameTile) {
  // mark tile
  game[subGame][subGameTile] = playerIcon;

  // verify if a game was completed
  winSubGame = verifyGame(game[subGame], playerIcon);

  // mark if some game is completed
  if (winSubGame) {
    completedGames[subGame] = playerIcon;
    subGamesFinished++;
    winSubGame = false;

    fillSubGame(subGame, playerIcon);
  } else {
    completedGames[subGame] = EMPTY_TILE;
  }
}

void fillSubGame(int subGame, char symbol) {
  for (int i = 0; i < 9; i++) game[subGame][i] = symbol;
}

bool verifyGame(char game[], char player) {
  int winning[8][3] = {
    { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, // horizontal
    { 0, 3, 6 }, { 1, 4, 7 }, { 2, 5, 8 }, // vertical
    { 0, 4, 8 }, { 2, 4, 6 }               // diagonal
  };

  for (int i = 0; i < 8; i++) {
    if (
      game[winning[i][0]] == player &&
      game[winning[i][1]] == player &&
      game[winning[i][2]] == player
    ) {
      return true;
    }
  }

  return false;
}

void errorMessage(char msg[]) {
  printf("[ERROR] - %s\n", msg);
}

void gameResult() {
  char msg[14];
       if (results == 0) strcpy(msg, "    Draw!    ");
  else if (results == 1) strcpy(msg, "Player 1 win!");
  else if (results == 2) strcpy(msg, "Player 2 win!");

  printf("#########################################\n");
  printf("#             GAME RESULTS!             #\n");
  printf("#             %s             #\n", msg);
  printf("#########################################\n");
}
