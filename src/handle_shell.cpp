#include <ncurses.h>
#include <unistd.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include "file_manager.hpp"

int run_command(string command)
{
    string silent_command = command + " > /dev/null 2>&1";
    return system(silent_command.c_str());
}

void display_shell(WINDOW *window, bool in_shell)
{
    werase(window);

    if (in_shell) {
        wattron(window, COLOR_PAIR(1));
    }
    box(window, ACS_VLINE, ACS_HLINE);
    if (in_shell) {
        wattroff(window, COLOR_PAIR(1));
    }

    wrefresh(window);
}

int handle_shell_input(WINDOW *window, FileManager *file_manager)
{
    static string command;

    mvwprintw(window, 1, 1, "%s", command.c_str());
    wattron(window, COLOR_PAIR(1));
    wprintw(window, "_");
    wattroff(window, COLOR_PAIR(1));
    wrefresh(window);

    int input = getch();

    if (input == 10) {
        run_command(command);

        command.erase();

        file_manager->in_shell = false;
        return 2;
    }

    if (input == 9) {
        command.append(
            FILE_PATH(file_manager->files[file_manager->file_position]));
        return 0;
    }

    if (input == 27) {
        file_manager->in_shell = false;
        return 0;
    }

    if (input == 263 && !command.empty()) {
        command.pop_back();
        return 0;
    }

    if (isascii(input) == 1) {
        command += input;
    }

    return 0;
}
