#include <utility>
#include "file_manager.hpp"

void display_help(WINDOW *window)
{
    werase(window);

    box(window, ACS_VLINE, ACS_HLINE);

    size_t height = getmaxy(window);
    size_t width = getmaxx(window);

    if (width < 45 || height < 13) {
        return;
    }

    const array<pair<string, string>, 10> keybinds = {
        {{"Key", "Action"},
         {"Up/Down", "Move selection up/down"},
         {"Left", "Go to parent directory"},
         {"Right", "Go to selected directory"},
         {"a", "Toggle hidden files"},
         {"p", "Toggle file preview pane"},
         {"s", "Cycle through sorting nodes"},
         {"t", "Open shell"},
         {"h", "Open help menu"},
         {"q", "Quit"}}};

    for (size_t i = 0; i < keybinds.size(); i++) {
        mvwprintw(window, i + 1, 1, "%-9s  %s", keybinds.at(i).first.c_str(),
                  keybinds.at(i).second.c_str());
    }

    wrefresh(window);
}
