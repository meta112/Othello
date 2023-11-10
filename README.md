# Othello Bot README
## Introduction
This program implements the two-player strategy board game Othello using C. It allows the user to play against an AI controlled opponent. Traditionally, Othello is played on an 8x8 board, although the program allows you to play on a board of any even dimension under 26. Players take turns placing pieces to flip the colour of the opponent's pieces, and whoever controls more pieces at the end wins.

## Playing the game
- After running the program, follow the onscreen instructions; enter the board dimension as a number, then enter which colour to have the computer play (case-sensitive).
- To play a move, enter the lower-case letter of the row and column of the square you wish to place a piece on, with no spaces in between. For example, typing "ah" on the standard 8x8 board would place a piece on the top-right corner.
- The bot can make its moves automatically; it uses a minimax algorithm with alpha-beta pruning to search the game tree and a heuristic function evaluating positions based on corner pieces, mobility, and total score. In testing, the program was generally found to move within 1 second at most.
- The game ends when the board is full, neither player has an available move, or the player makes an illegal move.
