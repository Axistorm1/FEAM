#include "file_manager.hpp"

bool color_support()
{
    if (has_colors()) {
        return true;
    }

    cout << "Your terminal doesn't have color support, continue? [y/n] ";

    char letter;
    while (true) {
        letter = tolower(getchar());
        if (letter == 'y') {
            return true;
        }
        if (letter == 'n') {
            return false;
        }
    }
    cout << endl;
    return true;
}

int start_ncurses()
{
    initscr();

    start_color();
    if (!color_support()) {
        return 1;
    }

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    // file colors
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);

    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    curs_set(0);

    return 0;
}

void close_ncurses()
{
    clear();
    reset_shell_mode();
    endwin();
}

static void sigint_handler(int value [[maybe_unused]])
{
    close_ncurses();
    exit(0);
}

void handle_signals() { signal(SIGINT, sigint_handler); }
