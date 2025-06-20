#include <iostream>
#include <string>
using namespace std;

#include <unistd.h>
#include <ncurses.h>

bool color_support(void)
{
    if (has_colors() == true)
        return true;

    cout << "Your terminal doesn't have color support, continue? [y/n] ";
    
    char letter;
    while (1) {
        letter = tolower(getchar());
        if (letter == 'y')
            return true;
        if (letter == 'n')
            return false;
    }
    cout << endl;
    return true;
}

int start_ncurses()
{
    initscr();

    start_color();
    if (!color_support())
        return 1;
    init_pair(1, COLOR_RED, COLOR_BLACK);

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
