// DO NOT REMOVE THE FOLLOWING LINE
#if !defined(TESTER_P1) && !defined(TESTER_P2)
// DO NOT REMOVE THE ABOVE LINE
#include "reversi.h"
// DO NOT REMOVE THE FOLLOWING LINE
#endif
// DO NOT REMOVE THE ABOVE LINE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


//reversi.h contains some function prototypes. The rest are below.
bool checkLegalMove(char board[][26], int n, int row, int col, char colour);
bool checkHasMove(char board[][26], int n, char colour, bool* boardFull);
bool playerMove(char board[][26], int n, char colour);
int flip(char board[][26], int n, int row, int col, char colour);
int flipInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol);
void naiveMove(char board[][26], int n, char colour);
void currentScore(char board[][26], int n, char playerColour, char aiColour);
int makeMove(char board[26][26], int n, char turn, int *row, int *col);
int minimaxab(char board[][26], int n, char colour, bool max, int* row, int* col, int depth, int alpha, int beta, bool firstMove, bool opponentFrozen);
int evaluate(char board[][26], int n, char colour, char oppColour, bool gameOverKnown);


int makeMove(char board[26][26], int n, char turn, int *row, int *col) 
{
    //call minimaxab - depth parameter can be changed to search more or less moves into the future
    //firstMove is only true in this call
    minimaxab(board, n, turn, true, row, col, 6, INT_MIN, INT_MAX, true, false);

    return 0;
}

//bool max indicates whether to check for the max player or the min player - always true in initial makeMove call
int minimaxab(char board[][26], int n, char colour, bool max, int* row, int* col, int depth, int alpha, int beta, bool firstMove, bool opponentFrozen)
{
    char oppColour;
    if (colour == 'B')
    {
        oppColour = 'W';
    }
    else
    {
        oppColour = 'B';
    }
    
    if (depth == 0) 
    {
        if (max) return evaluate(board, n, colour, oppColour, false); //if not max, the "colour" variable currently passed is actually the opponent's colour
        else return evaluate(board, n, oppColour, colour, false); //we want to always evaluate from the "originally passed" colour's perspective
    }

    int score = 0;
    int change = 0;
    int bestrow = 0;
    int bestcol = 0;
    bool hasMove = false;
    
    char (*copy)[26] = malloc(n * 26 * sizeof(char)); //pointer to char [26], not char* copy[26] (array of 26 char pointers)
    memcpy(copy, board, n * 26 * sizeof(char));

    if (max)
    {
        for (int r = 0; r < n; r++)
        {
            for (int c = 0; c < n; c++)
            {
                change = flip(copy, n, r, c, colour);
                if (change)
                {
                    copy[r][c] = colour;
                    score = minimaxab(copy, n, oppColour, false, row, col, depth - 1, alpha, beta, false, false);
                    //function call passes the opponent's colour as parameter colour
                    //max = false when opponent's colour is currently the parameter colour
                    if (score > alpha)
                    {
                        alpha = score;
                        bestrow = r;
                        bestcol = c;
                    }
                    hasMove = true;
                    if (alpha >= beta) break;
                    memcpy(copy, board, n * 26 * sizeof(char));
                }
            if (alpha >= beta) break;
            }
        }
        if (!hasMove)
        {
            if (opponentFrozen)
            {
                return evaluate(board, n, colour, oppColour, true); //turn player and opponent have no moves - game over
            }
            else
            {
                score = minimaxab(copy, n, oppColour, false, row, col, depth, alpha, beta, false, true); //not doing depth - 1 to go same # of moves down
                if (score > alpha)
                    {
                        alpha = score;
                    }
            }
        }
        free(copy);
        if (firstMove)
        {
            *row = bestrow;
            *col = bestcol;
        }
        return alpha;

    }
    else
    {
        for (int r = 0; r < n; r++)
        {
            for (int c = 0; c < n; c++)
            {
                change = flip(copy, n, r, c, colour); //since max = false, "colour" is the opponent's colour and it is min player's turn
                if (change)
                {
                    copy[r][c] = colour;
                    score = minimaxab(copy, n, oppColour, true, row, col, depth - 1, alpha, beta, false, false); 
                    if (score < beta)
                    {
                        beta = score;
                        bestrow = r;
                        bestcol = c;
                    }
                    hasMove = true;
                    if (alpha >= beta) break; 
                    memcpy(copy, board, n * 26 * sizeof(char));
                }
            }
            if (alpha >= beta) break;
        }
        if (!hasMove)
        {
            if (opponentFrozen)
            {
                return evaluate(board, n, oppColour, colour, true);
            }
            else
            {
                score = minimaxab(copy, n, oppColour, true, row, col, depth, alpha, beta, false, true);
                if (score < beta)
                {
                    beta = score;
                }
            }
        }
        free(copy);
        if (firstMove)
        {
            *row = bestrow;
            *col = bestcol;
        }
        return beta;
    }
    return 0;
}

//evaluates board position with heuristic
int evaluate(char board[][26], int n, char colour, char oppColour, bool gameOverKnown)
{
    int myCoins = 0;
    int oppCoins = 0;
    int myMobility = 0;
    int oppMobility = 0;
    int myCorners = 0;
    int oppCorners = 0;


    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (board[row][col] == 'U') 
            {
                if (checkLegalMove(board, n, row, col, colour))
                {
                    myMobility++;
                }
                if (checkLegalMove(board, n, row, col, oppColour))
                {
                    oppMobility++;
                }
            }
            else if (board[row][col] == colour)
            {
                myCoins++;
            }
            else if (board[row][col] == oppColour)
            {
                oppCoins++;
            }
        }
    }

    if (myMobility == 0 && oppMobility == 0) gameOverKnown = true; 
    if (gameOverKnown)
    {
        if (myCoins > oppCoins) 
        {
            return 10000000; 
        }
        else if (myCoins == oppCoins)
        {
            return 0;
        }
        else
        {
            return -10000000;
        }
    }

    if (board[0][0] == colour)
    {
        myCorners++;
    }
    else if (board[0][0] == oppColour)
    {
        oppCorners++;
    }
    
    if (board[0][n-1] == colour)
    {
        myCorners++;
    }
    else if (board[0][n-1] == oppColour)
    {
        oppCorners++;
    }

    
    if (board[n-1][0] == colour)
    {
        myCorners++;
    }
    else if (board[n-1][0] == oppColour)
    {
        oppCorners++;
    }

    if (board[n-1][n-1] == colour)
    {
        myCorners++;
    }
    else if (board[n-1][n-1] == oppColour)
    {
        oppCorners++;
    }

    int mobilityHeuristic = (int)((100.0 * (myMobility - oppMobility)) / (myMobility + oppMobility));
    int coinHeuristic = (int)((100.0 * (myCoins - oppCoins)) / (myCoins + oppCoins));

    int cornerHeuristic;
    if (myCorners + oppCorners != 0)
    {
        cornerHeuristic = (int)((100.0 * (myCorners - oppCorners)) / (myCorners + oppCorners));
    }
    else
    {
        cornerHeuristic = 0;
    }
    int heuristic = 25 * coinHeuristic + 5 * mobilityHeuristic + 50 * cornerHeuristic;

    return heuristic;
}

//*******************************************************
// Note: Please only put your main function below
// DO NOT REMOVE THE FOLLOWING LINE
#ifndef TESTER_P2
// DO NOT REMOVE THE ABOVE LINE

int main(void) {
    // Write your own main function here
    int dim;
    char aiColour;
    char playerColour;
    bool isPlayerTurn;
    bool playerHasMove = true;
    bool aiHasMove = true;
    bool boardFull = false;

    int airow = 0; //makeMove updates these variables with the position for the AI to play a piece in
    int aicol = 0;

    printf("Enter the board dimension: ");
    scanf("%d", &dim);
    printf("Computer plays (B/W): ");
    scanf(" %c", &aiColour);
    char board[dim][26];
    for (int r = 0; r < dim; r++)
    {
        for (int c = 0; c < dim; c++)
        {
            board[r][c] = 'U';
        }
    }
    board[dim / 2][dim / 2 - 1] = 'B';
    board[dim / 2 - 1][dim / 2] = 'B';
    board[dim / 2][dim / 2] = 'W';
    board[dim / 2 - 1][dim / 2 - 1] = 'W';
    printBoard(board, dim);

    if (aiColour == 'W') 
    {
        playerColour = 'B';
        isPlayerTurn = true;
    }
        
    else if (aiColour == 'B') 
    {
        playerColour = 'W';
        isPlayerTurn = false;
    }

    while (1)
    {
        if (isPlayerTurn) 
        {
            playerHasMove = checkHasMove(board, dim, playerColour, &boardFull);
            if (boardFull) break;
            if (playerHasMove)
            {
                if (!playerMove(board, dim, playerColour)) //player makes a move - if illegal move made, player loses
                {
                printf("%c player wins.\n", aiColour);
                return 0;
                }
            }
            else
            {
                printf("%c player has no valid move.\n", playerColour);
            }
            isPlayerTurn = false;

        }

        if (!playerHasMove && !aiHasMove) break;

        if (!isPlayerTurn)
        {
            aiHasMove = checkHasMove(board, dim, aiColour, &boardFull);
            if (boardFull) break;
            if (aiHasMove)
            {
                makeMove(board, dim, aiColour, &airow, &aicol);
                flip(board, dim, airow, aicol, aiColour);
                board[airow][aicol] = aiColour;
                printf("Computer places %c at %c%c.\n", aiColour, 'a' + airow, 'a' + aicol); 
                printBoard(board, dim);
            }
            else
            {
                printf("%c player has no valid move.\n", aiColour);
            }
            isPlayerTurn = true;
        }
        if (!playerHasMove && !aiHasMove) break;
    }

    currentScore(board, dim, playerColour, aiColour);

    return 0;
}

// DO NOT REMOVE THE FOLLOWING LINE
#endif
// DO NOT REMOVE THE ABOVE LINE
//*******************************************************




void printBoard(char board[][26], int n)
{
  printf("  ");
    for (int i = 0; i < n; i++)
    {
        printf("%c", 'a' + i);
    }
    printf("\n");
    for (int r = 0; r < n; r++)
    {
        printf("%c ", 'a' + r);
        for (int c = 0; c < n; c++)
        {
            printf("%c", board[r][c]);
        }
        printf("\n");
    }
}

bool positionInBounds(int n, int row, int col)
{
  return (row >= 0 && row < n && col >= 0 && col < n);
}

bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol)
{
    //deltarow and deltacol determine direction to check in
    //check that entered rows and cols are in bounds before indexing board
    if (!positionInBounds(n, row, col) || board[row][col] != 'U') 
    {
        return false;
    }

    if (colour == 'W')
    {
        row += deltaRow;
        col += deltaCol;
        if (positionInBounds(n, row, col) && board[row][col] == 'B')
        {
            while (1)
            {
                row += deltaRow;
                col += deltaCol;
                if (positionInBounds(n, row, col))
                {
                    if (board[row][col] == 'W')
                    {
                        return true;
                    }
                    else if (board[row][col] == 'U')
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }

    else if (colour == 'B')
    {
        row += deltaRow;
        col += deltaCol;
        if (positionInBounds(n, row, col) && board[row][col] == 'W')
        {
            while (1)
            {
                row += deltaRow;
                col += deltaCol;
                if (positionInBounds(n, row, col))
                {
                    if (board[row][col] == 'B')
                    {
                        return true;
                    }
                    else if (board[row][col] == 'U')
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    return 0; //to avoid compiler warning of reaching the end without a return; should not occur through the ifs/elses
}

bool checkLegalMove(char board[][26], int n, int row, int col, char colour)
{
    if (checkLegalInDirection(board, n, row, col, colour, -1, -1)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, -1, 0)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, -1, 1)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, 0, -1)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, 0, 1)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, 1, -1)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, 1, 0)) return true;
    if (checkLegalInDirection(board, n, row, col, colour, 1, 1)) return true;
    return false;
}

bool checkHasMove(char board[][26], int n, char colour, bool* boardFull)
{
  bool noU = true;
  for (int row = 0; row < n; row++)
  {
    for (int col = 0; col < n; col++)
    {
      if (checkLegalMove(board, n, row, col, colour)) return true;
      if (board[row][col] == 'U') noU = false;
    }
  }

  //returns to the main function whether or not the board is full
  //if it is, we break to avoid checking if the other player has moves, because they will not
  *boardFull = noU;
  return false;
}

bool playerMove(char board[][26], int n, char colour) 
{
    char row, col;
    printf("Enter move for colour %c (RowCol): ", colour);
    scanf(" %c%c", &row, &col);
    if (flip(board, n, row - 'a', col - 'a', colour)) //if number of flipped pieces is nonzero, continue with move as it was valid
    {
        board[row - 'a'][col - 'a'] = colour; //flip() only changes pieces that will be flipped; this line places the piece
        printBoard(board, n);
        return true;
    }
    else //if number of flipped pieces is zero, move was invalid
    {
        printf("Invalid move.\n");
        return false;
    }
}

int flip(char board[][26], int n, int row, int col, char colour) 
//flips pieces that get flipped in all directions and returns the number of pieces that would be flipped
//if the move is illegal no pieces are flipped so returns 0
{
    return flipInDirection(board, n, row, col, colour, -1, -1)
    + flipInDirection(board, n, row, col, colour, -1, 0)
    + flipInDirection(board, n, row, col, colour, -1, 1)
    + flipInDirection(board, n, row, col, colour, 0, -1)
    + flipInDirection(board, n, row, col, colour, 0, 1)
    + flipInDirection(board, n, row, col, colour, 1, -1)
    + flipInDirection(board, n, row, col, colour, 1, 0)
    + flipInDirection(board, n, row, col, colour, 1, 1);
}

int flipInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol)
{
    int numFlipped = 0;
    if (colour == 'W')
    {
        if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol))
        {
            row += deltaRow;
            col += deltaCol;
            while (board[row][col] == 'B')
            {
                board[row][col] = 'W';
                numFlipped++;
                row += deltaRow;
                col += deltaCol;
            }
        }
    }
    else if (colour == 'B')
    {
        if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol))
        {
            row += deltaRow;
            col += deltaCol;
            while (board[row][col] == 'W')
            {
                board[row][col] = 'B';
                numFlipped++;
                row += deltaRow;
                col += deltaCol;
            }
        }
    }
  return numFlipped;
}

void currentScore(char board[][26], int n, char playerColour, char aiColour)
{
  int playerScore = 0;
  int aiScore = 0;
  for (int row = 0; row < n; row++)
  {
    for (int col = 0; col < n; col++)
    {
      if (board[row][col] == playerColour)
      {
        playerScore++;
      }
      else if (board[row][col] == aiColour)
      {
        aiScore++;
      }
    }
  }
  if (playerScore > aiScore)
  {
    printf("%c player wins.\n", playerColour);
    return;
  }
  else if (playerScore < aiScore)
  {
    printf("%c player wins.\n", aiColour);
    return;
  }
  else if (playerScore == aiScore)
  {
    printf("Draw!\n");
    return;
  }
}


//naive algorithm for computer to select a move
//a greedy algorithm that chooses the move flipping the most pieces
void naiveMove(char board[][26], int n, char colour)
{
    int scoreBoard[n][n]; //matrix storing score values of each move
    int maxScore = 0;
    int maxRow = 0;
    int maxCol = 0;

    //test out moves on a copy of the board
    char tempCopy[n][26];
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
        //reset board
        for (int row = 0; row < n; row++)
        {
            for (int col = 0; col < n; col++)
            {
            tempCopy[row][col] = board[row][col];
            }
        }
        scoreBoard[r][c] = flip(tempCopy, n, r, c, colour);
        if (scoreBoard[r][c] > maxScore)
        {
            maxScore = scoreBoard[r][c];
            maxRow = r;
            maxCol = c;
        }
        }
    }

    flip(board, n, maxRow, maxCol, colour);
    board[maxRow][maxCol] = colour;
    printf("Computer places %c at %c%c.\n", colour, 'a' + maxRow, 'a' + maxCol);
    printBoard(board, n);

    return;
}
