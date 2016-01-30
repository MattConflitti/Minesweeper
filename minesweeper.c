/************************************************************************
* minesweeper.c																											 *
*																																		 *
* Author(s): Matt Conflitti																					 *
***********************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE_MIN 5
#define BOARD_SIZE_MAX 15
#define PCT_MINES_MIN 10
#define PCT_MINES_MAX 70

typedef struct {
	bool is_mine;
	int mines;
	bool visible;
	bool marked;
} Cell;

typedef enum {WON, LOST, INPROGRESS} Status;

/************************************************************************
* YOU MUST NOT DEFINE ANY GLOBAL VARIABLES (i.e., OUTSIDE FUNCTIONS).  *
* COMMUNICATION BETWEEN FUNCTIONS MUST HAPPEN ONLY VIA PARAMETERS.     *                           *
************************************************************************/

/************************************************************************
* Function declarations/prototypes																			*
************************************************************************/
void displayMenu();

int getBoardSize();

int getPercentMines();

int getMaxMines(int size, Cell board[][size]);

void initBoard(int size, Cell board[][size]);

void placeMinesOnBoard(int size, Cell board[][size], int nbrMines);

void fillInMineCountForNonMineCells(int size, Cell board[][size]);

int nbrOfMines(int size, Cell board[][size]);

int getNbrNeighborMines(int row, int col, int size, Cell board[][size]);

void displayBoard(int size, Cell board[][size], bool displayMines);

Status selectCell(int row, int col, int size, Cell board[][size], bool flag);

int nbrVisibleCells(int size, Cell board[][size]);

void setImmediateNeighborCellsVisible(int row, int col, int size, Cell board[][size]);

void setAllNeighborCellsVisible(int row, int col, int size, Cell board[][size]);


/************************************************************************
* Main driver of the program. Uses the functions defined above.		*
************************************************************************/
int main()
{
	int row, col, size, nbrMines;
	char command;
	bool displayMines;
	Status gameState;


	while (true) {
		gameState = INPROGRESS;
		displayMines = false;


		srand((unsigned int)time(NULL));

		printf("\n!!!!!WELCOME TO THE MINESWEEPER GAME!!!!!\n\n");
		size = getBoardSize();

		// declare 2D array of cells
		Cell board[size][size];

		initBoard(size,board);

		// determine number of mine to place on the board
		nbrMines = (int) (size * size * (getPercentMines()/100.0));

		// place mines randomly on the board
		placeMinesOnBoard(size,board,nbrMines);

		// For each non-mine cell, sets the neighboring mine count
		fillInMineCountForNonMineCells(size,board);

		displayBoard(size,board,displayMines);

		while(true) {
			printf("Enter command (m/M for command menu): ");
			scanf("%c",&command);
			getchar();

			switch (command) {
				case 'm': case 'M':
				displayMenu();
				break;

				case 'c': case 'C':
				do {
					printf("Enter row and column of cell: ");
					scanf("%d%d",&row,&col);
					getchar();
					if (row < 1 || row > size || col < 1 || col > size) {
						printf("Invalid row or column values. Try again.\n");
					}
				} while (row < 1 || row > size || col < 1 || col > size);
				row--;
				col--;
				gameState = selectCell(row,col,size,board,false);
				displayBoard(size,board,displayMines);
				break;

				case 'f': case 'F':
				do {
					printf("Enter row and column of cell: ");
					scanf("%d%d",&row,&col);
					getchar();
					if (row < 1 || row > size || col < 1 || col > size) {
						printf("Invalid row or column values. Try again.\n");
					}
				} while (row < 1 || row > size || col < 1 || col > size);
				row--;
				col--;
				gameState = selectCell(row,col,size,board,true);
				displayBoard(size,board,displayMines);
				break;

				case 's': case 'S':
				displayMines = true;
				displayBoard(size,board,displayMines);
				break;

				case 'h': case 'H':
				displayMines = false;
				displayBoard(size,board,displayMines);
				break;

				case 'b': case 'B':
				displayBoard(size,board,displayMines);
				break;

				case 'q': case 'Q':
				printf("Bye.\n");
				return 0;

				default:
				printf("Invalid command. Try again.\n");
				break;
			}
			// display appropriate message if the game is over
			if (gameState == WON) {
				printf("You found all the mines. Congratulations.\n");
				break;
			} else if (gameState == LOST) {
				printf("Oops. Sorry, you landed on a mine.\n");
				break;
			}
		}
		while(gameState != INPROGRESS) {
			printf("Play again? (y/Y or n/N)");
			scanf("%c",&command);
			getchar();

			switch (command) {
				case 'y': case 'Y':
				gameState = INPROGRESS;
				break;

				case 'n': case 'N':
				printf("Bye.\n");
				return 0;

				default:
				break;
			}
		}

	}
	return 0;
}

/************************************************************************
* Displays list of commands allowed at the prompt						*
************************************************************************/
void displayMenu()
{
	printf("List of available commands:\n");
	printf("   Show Mines: s/S\n");
	printf("   Hide Mines: h/H\n");
	printf("   Select Cell: c/C\n");
	printf("   Flag/Unflag Cell: f/F\n");
	printf("   Display Board: b/B\n");
	printf("   Display Menu: m/M\n");
	printf("   Quit: q/Q\n\n");
}

/************************************************************************
* Initializes the fields of each cell on the board as follows:			*
* 		is_mine field to false											*
* 		mines field to 0												*
* 		visible field to false											*
************************************************************************/
void initBoard(int size, Cell board[][size])
{
	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			board[i][j].is_mine = false;
			board[i][j].mines = 0;
			board[i][j].visible = false;
			board[i][j].marked = false;
		}
	}
}

/************************************************************************
* Places the specified number of mines randomly on the board			*
************************************************************************/
void placeMinesOnBoard(int size, Cell board[][size], int nbrMines)
{
	int num;
	int total = nbrMines;
	while(total > 0) {
		for(int i=0; i<size; i++) {
			num = rand() % size;
			for(int j=0;j<size; j++) {
				if(total == 0) {
					break;
				}

				if(j == num && !board[i][j].is_mine) {
					board[i][j].is_mine = true;
					total--;
				}
			}
		}

	}


}

/************************************************************************
* For each non-mine cell on the board, set the "mines" field to the	*
number of mines in the immediate neighborhood.						*
************************************************************************/
void fillInMineCountForNonMineCells(int size, Cell board[][size])
{
	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			if(!board[i][j].is_mine) {
				if(i==0) {
					//in top row, if mine is directly below
					if(board[i+1][j].is_mine) {
						board[i][j].mines++;
					}

					//diagonals
					if(j>0 && j<=size-2) {
						if(board[i+1][j-1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i+1][j+1].is_mine) {
							board[i][j].mines++;
						}
					}
				} else if(i==size-1) {
					//in bottom row, if mine is directly above
					if(board[i-1][j].is_mine) {
						board[i][j].mines++;
					}
					//diagonals
					if(j>0 && j<=size-2) {
						if(board[i-1][j-1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i-1][j+1].is_mine) {
							board[i][j].mines++;
						}
					}

				} else {
					//any other row, if mine is directly above or below
					if(board[i+1][j].is_mine || board[i-1][j].is_mine) {
						board[i][j].mines++;
					}
					//diagonals from any middle row
					if(j>0 && j<=size-2) {
						if(board[i+1][j-1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i+1][j+1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i-1][j-1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i-1][j+1].is_mine) {
							board[i][j].mines++;
						}
					}
				}

				if(j==0) {
					//in leftmost rcol, if mine is directly to right
					if(board[i][j+1].is_mine) {
						board[i][j].mines++;
					}
					//diagonals
					if(i>0 && i<=size-2) {
						if(board[i-1][j+1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i+1][j+1].is_mine) {
							board[i][j].mines++;
						}
					}

				} else if(j==size-1) {
					//in rightmost col, if mine is directly to left
					if(board[i][j-1].is_mine) {
						board[i][j].mines++;
					}
					//diagonals
					if(i>0 && i<=size-2) {
						if(board[i-1][j-1].is_mine) {
							board[i][j].mines++;
						}
						if(board[i+1][j-1].is_mine) {
							board[i][j].mines++;
						}
					}
				} else {
					//any other row, if mine is directly left or right
					if(board[i][j+1].is_mine) {
						board[i][j].mines++;
					}
					if(board[i][j-1].is_mine) {
						board[i][j].mines++;
					}
				}
				//topleft corner
				if(i==0 && j==0) {
					if(board[i+1][j+1].is_mine) {
						board[i][j].mines++;
					}

				}

				//topright corner
				if(i==0 && j==size-1) {
					if(board[i+1][j-1].is_mine) {
						board[i][j].mines++;
					}
				}

				//bottomleft corner
				if(i==size-1 && j==0) {
					if(board[i-1][j+1].is_mine) {
						board[i][j].mines++;
					}
				}

				//bottomright corner
				if(i==size-1 && j==size-1) {
					if(board[i-1][j-1].is_mine) {
						board[i][j].mines++;
					}
				}

			}
		}
	}
}

/************************************************************************
* Counts and returns the number of mines on the board					*
************************************************************************/
int nbrOfMines(int size, Cell board[][size])
{
	int count = 0;

	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			if(board[i][j].is_mine) {
				count++;
			}
		}
	}

	return count;
}

/************************************************************************
* Returns the number of mines in the immediate neighborhood of a cell	*
* at location (row,col) on the board.									*
************************************************************************/
int getNbrNeighborMines(int row, int col, int size, Cell board[][size])
{

	return board[row][col].mines;

	// int count = 0;
	//
	// // TO DO
	//
	// return count;
}

/************************************************************************
* Displays the board. If a cell is not currently visible and has a		*
* mine, show the mine if the displayMines is true. Used for debugging	*
* and testing purposes.												*
************************************************************************/
void displayBoard(int size, Cell board[][size], bool displayMines)
{
	printf("\n");

	for(int i=0; i<=size; i++) {
		if(i==0) {
			printf("\t");
			printf("\e[44m  \e[0m");
		} else {
			printf("\e[37;44m%2d\e[0m", i);
		}
	}
	printf("\n");

	for(int i=0; i<size; i++) {
		printf("\t");
		printf("\e[37;44m%2d\e[0m", i+1);


		for(int j=0;j<size; j++) {
			if(board[i][j].visible && !board[i][j].is_mine) {
				if(board[i][j].mines == (getMaxMines(size,board)) || board[i][j].mines > 4) {
					printf("\e[31;42m%2d\e[0m", getNbrNeighborMines(i, j, size, board));
				} else {
					printf("\e[30;42m%2d\e[0m", getNbrNeighborMines(i, j, size, board));
				}

			}else if(board[i][j].marked) {
				printf("\e[31;47m X\e[0m");
			}

			else if(board[i][j].is_mine && (displayMines || board[i][j].visible)) {
				printf("\e[31;47m *\e[0m");

			}
			else {
				printf("\e[30;47m ?\e[0m");
			}

		}
		printf("\n");
	}
	printf("\n");
}

/************************************************************************
* Prompts the user for board size, reads and validates the input		*
* entered, and returns the integer if it is within valid range.		*
* repeats this in a loop until the user enters a valid value.			*
************************************************************************/
int getBoardSize()
{
	int size = 0;

	while(true) {
		printf("Enter a board size between %i and %i:", BOARD_SIZE_MIN, BOARD_SIZE_MAX);
		scanf("%i", &size);
		getchar();
		if(size >= BOARD_SIZE_MIN && size <= BOARD_SIZE_MAX) {
			return size;
		}
	}
}

/************************************************************************
* Get max number mines on any of the cells.									*
************************************************************************/
int getMaxMines(int size, Cell board[][size]) {
	int max = 0;

	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			if(board[i][j].mines > max) {
				max = board[i][j].mines;
			}
		}
	}
	return max;
}

/************************************************************************
* Prompts the user for percentage of mines to place on the board,		*
* reads and validates the input entered, and returns the integer if it	*
* is within valid range. repeats this in a loop until the user enters	*
* a valid value for board size.										*
************************************************************************/
int getPercentMines()
{
	int percent = 0;

	while(true) {
		printf("Enter a percent of mines from %i to %i:", PCT_MINES_MIN,PCT_MINES_MAX);
		scanf("%i", &percent);
		getchar();
		if(percent >= PCT_MINES_MIN && percent <= PCT_MINES_MAX) {
			return percent;
		}
	}
}

/************************************************************************
* Process cell selection by user during the game						*
************************************************************************/
Status selectCell(int row, int col, int size, Cell board[][size], bool flag)
{
	if(flag) {
		if(board[row][col].marked) {
			board[row][col].marked = false;
		} else {
			board[row][col].marked = true;
		}
		return INPROGRESS;
	}
	if(!board[row][col].is_mine) {
		board[row][col].visible=true;
		board[row][col].marked = false;
		setImmediateNeighborCellsVisible(row,col,size,board);
		if((nbrVisibleCells(size,board)+nbrOfMines(size,board)) >= (size*size)) {
			return WON;
		} else {
			return INPROGRESS;
		}

	}
	else {
		board[row][col].visible=true;
		return LOST;
	}
}

/************************************************************************
* Returns the number of cells that are currently visible.				*
************************************************************************/
int nbrVisibleCells(int size, Cell board[][size])
{
	int count = 0;

	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			if(board[i][j].visible) {
				count++;
			}
		}
	}

	return count;
}

/************************************************************************
* If the mine count of a cell at location (row,col) is zero, then make	*
* the cells ONLY in the immediate neighborhood visible.				*
************************************************************************/
void setImmediateNeighborCellsVisible(int row, int col, int size, Cell board[][size])
{
	if(row > 0 && row < size && col > 0 && col < size) {
		//middle
		board[row-1][col-1].visible =true;
		board[row-1][col].visible =true;
		board[row-1][col+1].visible =true;
		board[row][col-1].visible =true;
		board[row][col+1].visible =true;
		board[row+1][col-1].visible =true;
		board[row+1][col].visible =true;
		board[row+1][col+1].visible =true;
	}
	//corners
	else if(row == 0 && col == 0) {
		board[row+1][col].visible =true;
		board[row+1][col+1].visible =true;
		board[row][col+1].visible =true;
	}
	else if(row == (size-1) && col == 0) {
		board[row-1][col].visible =true;
		board[row-1][col+1].visible =true;
		board[row][col+1].visible =true;
	}
	else if(row == 0 && col == (size-1)) {
		board[row+1][col].visible =true;
		board[row+1][col-1].visible =true;
		board[row][col-1].visible =true;
	}
	else if(row == (size-1) && col == (size-1)) {
		board[row-1][col].visible =true;
		board[row-1][col-1].visible =true;
		board[row][col-1].visible =true;
	}
	//top and bottom row
	else if(row == 0 && col > 0 && col < size) {
		board[row][col-1].visible =true;
		board[row+1][col-1].visible =true;
		board[row+1][col].visible =true;
		board[row+1][col+1].visible =true;
		board[row][col+1].visible =true;
	} else if(row == (size-1) && col > 0 && col < size) {
		board[row][col-1].visible =true;
		board[row-1][col-1].visible =true;
		board[row-1][col].visible =true;
		board[row-1][col+1].visible =true;
		board[row][col+1].visible =true;
	}
	//left and right col
	else if(col == 0 && row > 0 && row < size) {
		board[row-1][col].visible =true;
		board[row+1][col].visible =true;
		board[row+1][col+1].visible =true;
		board[row][col+1].visible =true;
		board[row-1][col+1].visible =true;
	} else if(col == (size-1) && row > 0 && row < size) {
		board[row-1][col].visible =true;
		board[row-1][col-1].visible =true;
		board[row][col-1].visible =true;
		board[row+1][col-1].visible =true;
		board[row+1][col].visible =true;
	}

	for(int i=0; i<size; i++) {
		for(int j=0;j<size; j++) {
			if(board[i][j].is_mine) {
				board[i][j].visible = false;
			}
		}
	}
}

/************************************************************************
* If the mine count of a cell at location (row,col) is zero, then make	*
* the cells in the immediate neighborhood visible and repeat this		*
* process for each of the cells in this set of cells that have a mine	*
* count of zero, and so on.											*
************************************************************************/
void setAllNeighborCellsVisible(int row, int col, int size, Cell board[][size])
{
	// TO DO
}
