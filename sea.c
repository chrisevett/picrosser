#include <curses.h>
#include <stdlib.h>
#include <unistd.h>

#define PAIR_GREEN_BLACK 1
#define PAIR_YELLOW_BLACK 2

#define DELAY 35000

// a pirates life for c

int main(void) {

  // intiialize curses

  initscr();
  // stdscr is the default window you can add subsets
  keypad(stdscr, TRUE);
  // cbreak disables line buffering so we get
  // straight up input like esc chars etc
  cbreak();
  // dont echo inputs to term
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(FALSE);

  if (has_colors() == FALSE) {
    // endwin returns the term to normal behavior
    endwin();
    printf("Terminal does not support colors");
    exit(1);
  }

  // start color allows the term to use colors
  // each color has a foreground and
  // a background value
  start_color();
  init_pair(PAIR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
  init_pair(PAIR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);

  clear();

  int max_y = 0, max_x = 0;
  int position_y = 0, position_x = 0;

  int last_input = -1;

  // get initial dimensions so we cna place our actor
  getmaxyx(stdscr, max_y, max_x);

  int ch; // input

  position_y = max_y / 2;

  do {

    ch = getch();

    getmaxyx(stdscr, max_y, max_x);

    // if we are taller than the position descend
    if (position_y < max_y / 2) {
      position_y += 1;
    }

    clear();
    attron(COLOR_PAIR(PAIR_GREEN_BLACK));
    // print our boat pieces
    // todo - move this to a function

    mvprintw(position_y, position_x, "   |}");
    mvprintw(position_y + 1, position_x, "   |}}");
    mvprintw(position_y + 2, position_x, "   |}}}");
    mvprintw(position_y + 3, position_x, " __|_____");
    // have to escape...
    mvprintw(position_y + 4, position_x, " \\______/");

    if (ch > 0) {
      last_input = ch;
    }
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

static char *itoa(int a) {
  static char buf[10];

  snprintf(buf, sizeof buf, "%d", a);
  return (buf);
}
