#include <ncurses.h>
#include "file_manager.hpp"

void display_help(WINDOW *window)
{
    werase(window);

    box(window, ACS_VLINE, ACS_HLINE);

    wrefresh(window);
}
