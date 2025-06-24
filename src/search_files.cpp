#include "file_manager.hpp"

vector<fs::directory_entry> search_files(
    const vector<fs::directory_entry> &files, const string &needle)
{
    vector<fs::directory_entry> searched_files;

    for (const auto &entry : files) {
        if (FILE_NAME(entry).find(needle) != string::npos) {
            searched_files.push_back(entry);
        }
    }

    return searched_files;
}

void display_search(WINDOW *window, FileManager *file_manager)
{
    if (file_manager->in_shell) {
        return;
    }

    werase(window);

    box(window, ACS_VLINE, ACS_HLINE);

    mvwprintw(window, 1, 1, "%s", file_manager->current_search.c_str());

    if (file_manager->in_search) {
        wprintw(window, "_");
    } else {
        wprintw(window, " ");
    }

    wrefresh(window);
}

int handle_search_input(FileManager *file_manager)
{
    if (!file_manager->in_search) {
        return 0;
    }

    int input = getch();

    if (input == 27) {
        file_manager->in_search = false;
        return 0;
    }

    if (input == 263 && !file_manager->current_search.empty()) {
        file_manager->current_search.pop_back();
        return 1;
    }

    if (isascii(input) == 1 && input != 10) {
        file_manager->current_search += input;
        return 1;
    }

    return 0;
}
