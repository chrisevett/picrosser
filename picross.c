#include <curses.h>
#include <stdlib.h>
#include <unistd.h> // todo find out what this is

#define PAIR_GREEN_BLACK 1
#define PAIR_YELLOW_BLACK 2
#define PAIR_RED_BLACK 3
#define PAIR_WHITE_BLACK 4
#define PAIR_BLACK_GREEN 5

#define DELAY 35000

int HintFromTuple(int[], int, int *);
void GetColumnAtIndex(int **, int, int, int *);
int **PopulatePuzzle(int, int);
int *PuzzleRow(int[], int);
int **PopulateRowHints(int **, int, int);
int **PopulateColumnHints(int **, int, int);
void PaintBoard(int, int, int, int, int, int);

static char *itoa(int a);

int main(void) {

  int rowSize = 5, columnSize = 5;
  int **puzzleMap = PopulatePuzzle(rowSize, columnSize);
  int **rowHints = PopulateRowHints(puzzleMap, rowSize, columnSize);
  int **columnHints = PopulateColumnHints(puzzleMap, rowSize, columnSize);

  initscr();
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(FALSE);

  if (has_colors() == FALSE) {
    endwin();
    printf("Terminal does not support colors");
    exit(1);
  }

  start_color();
  init_pair(PAIR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(PAIR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
  init_pair(PAIR_RED_BLACK, COLOR_RED, COLOR_BLACK);
  init_pair(PAIR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(PAIR_BLACK_GREEN, COLOR_BLACK, COLOR_GREEN);

  clear();

  int ch; // input
  int max_y = 0, max_x = 0;
  int position_y = 0, position_x = 0;

  int last_input = -1;
  getmaxyx(stdscr, max_y, max_x);

  // position_y = max_y / 2;
  int board_anchor_y = (max_y / 2) - 5;
  int board_anchor_x = (max_x / 2) - 10;
  int current_x = board_anchor_x, current_y = board_anchor_y;

  do {

    ch = getch();

    // think this is in case we resize
    getmaxyx(stdscr, max_y, max_x);

    clear();

    // print the board
    PaintBoard(board_anchor_x, board_anchor_y, rowSize, columnSize, current_x,
               current_y); // print the  row hints
    // print the column hints

    if (ch > 0) {
      last_input = ch;
    }
    attron(COLOR_PAIR(PAIR_GREEN_BLACK));
    mvprintw(max_y - 5, max_x - 10, "last char");
    mvprintw(max_y - 6, max_x - 5, itoa(last_input));

    attroff(COLOR_PAIR(PAIR_GREEN_BLACK));
    refresh();

    usleep(DELAY);

    // our boat jumps
    if (ch == ' ') {
      position_y -= 11;
    }

    // left arrow
    if (ch == 260) {
      position_x -= 5;
    }

    // right arrow
    if (ch == 261) {
      position_x += 5;
    }

    // up arrow
    if (ch == 258) {
      position_y += 5;
    }

    // down arrow
    if (ch == 259) {
      position_y -= 5;
    }

  } while ((ch != 'q') && ch != 'Q');

  // wrap it up
  endwin();
  return 0;
}

int HintFromTuple(int tuple[], int tupleSize, int *hints) {
  int currentClueSum = 0;
  // void *realloc(void *p, size_t size)
  int hintCount = 0; // the array will be one bigger

  for (int i = 0; i < tupleSize; i++) {
    if (tuple[i] == 1) {
      // add 1 to the existing clue sum
      currentClueSum++;
    } else {
      if (currentClueSum > 0) {
        // if the tuple is 0 and we have previous clue sum write a new clue and
        // zero
        hintCount++;
        hints = (int *)realloc(hints, hintCount);
        hints[hintCount - 1] = currentClueSum;
        currentClueSum = 0;
      }
    }
  }
  // dump the last clue sum if its >0
  if (currentClueSum > 0) {
    hintCount++;
    hints = (int *)realloc(hints, hintCount);
    hints[hintCount - 1] = currentClueSum;
  }

  // if no hints make a single 0 hint
  if (hintCount == 0) {
    hints[0] = 0;
    hintCount++;
  }
  return hintCount;
}

// given a 5 x 5 array, returns the
void GetColumnAtIndex(int **grid, int columnSize, int index, int *column) {
  for (int i = 0; i < columnSize; i++) {
    column[i] = grid[i][index];
  }
}

// make this load a level later
int **PopulatePuzzle(int rowSize, int columnSize) {
  int **grid = (int **)malloc(sizeof(int) * rowSize * columnSize);

  int row1[] = {1, 1, 0, 1, 1};
  int row2[] = {1, 1, 1, 1, 1};
  int row3[] = {0, 0, 0, 0, 0};
  int row4[] = {0, 0, 1, 0, 0};
  int row5[] = {0, 1, 0, 1, 0};

  grid[0] = PuzzleRow(row1, 5);
  grid[1] = PuzzleRow(row2, 5);
  grid[2] = PuzzleRow(row3, 5);
  grid[3] = PuzzleRow(row4, 5);
  grid[4] = PuzzleRow(row5, 5);
  return grid;
}

int *PuzzleRow(int row[], int size) {
  int *dynamicRow = (int *)malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    dynamicRow[i] = row[i];
  }
  return dynamicRow;
}

// these are laid out left to right
int **PopulateRowHints(int **puzzleMap, int rowSize, int columnSize) {
  // todo: this is probably too big i think we just need one int size per ptr ie
  // per row hint
  int **RowHints = (int **)malloc(sizeof(int) * rowSize * columnSize);
  for (int i = 0; i < rowSize; i++) {
    int *rowHint = (int *)malloc(sizeof(int));
    HintFromTuple(puzzleMap[i], rowSize, rowHint);
    RowHints[i] = rowHint;
  }

  return RowHints;
}

// note these are laid out top to bottom
int **PopulateColumnHints(int **puzzleMap, int rowSize, int columnSize) {
  int **ColumnHints = (int **)malloc(sizeof(int) * rowSize * columnSize);

  for (int i = 0; i < columnSize; i++) {
    int *columnHint = (int *)malloc(sizeof(int));
    int *column = (int *)malloc(sizeof(int) * columnSize);
    GetColumnAtIndex(puzzleMap, columnSize, i, column);
    HintFromTuple(column, columnSize, columnHint);
    ColumnHints[i] = columnHint;
  }

  return ColumnHints;
}

static char *itoa(int a) {
  static char buf[10];

  snprintf(buf, sizeof buf, "%d", a);
  return (buf);
}

void PaintBoard(int anchor_x, int anchor_y, int rowSize, int columnSize,
                int current_x, int current_y) {
  attron(COLOR_PAIR(PAIR_GREEN_BLACK));
  for (int i = 0; i < rowSize; i++) {
    for (int j = 0; j < columnSize; j++) {
      mvprintw(anchor_y + (2 * j), anchor_x + (4 * i), "X");
    }
  }
  attroff(COLOR_PAIR(PAIR_GREEN_BLACK));
  attron(COLOR_PAIR(PAIR_BLACK_GREEN));
  mvprintw(current_y, current_x, "O");
  attroff(COLOR_PAIR(PAIR_BLACK_GREEN));
}
