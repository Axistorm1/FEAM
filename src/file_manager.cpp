#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#include "file_manager.hpp"

void change_folder(FileManager *fm, string folder)
{
    if (chdir(folder.c_str()) == -1)
        return;
    fm->cwd = fs::current_path().string();
    fm->file_position = 0;
    fm->directory_change = true;
    fm->files.clear();
}

int get_user_input(FileManager *fm, WINDOW *file_preview_wd, WINDOW *files_list_wd)
{
    int input = getch();

    // enable/disable hidden files (.*)
    if (input == 'a') {
        fm->hidden_files = !fm->hidden_files;
        fm->directory_change = true;
    }

    if (input == 'p') {
        fm->preview = !fm->preview;
        if (!fm->preview) {
            werase(file_preview_wd);
            wrefresh(file_preview_wd);
            wresize(files_list_wd, LINES, COLS);
        } else
            wresize(files_list_wd, LINES, COLS / 2);
    }

    // loop through sorts
    if (input == 's') {
        fm->sort_type++;
        if (fm->sort_type > FIRST_MODIFIED)
            fm->sort_type = 0;
        fm->directory_change = true;
    }

    // move through files
    // TODO: Make it wrap
    if (input == KEY_UP && fm->file_position > 0)
        fm->file_position--;
    if (input == KEY_DOWN && fm->files.size() != 0 && fm->file_position < fm->files.size() - 1)
        fm->file_position++;

    if (input == 534)
        fm->file_position = fm->files.size() - 1;
    if (input == 575)
        fm->file_position = 0;

    if (input == KEY_LEFT && fm->cwd != "/")
        change_folder(fm, "..");

    if ((input == KEY_ENTER || input == KEY_RIGHT || input == 10) && fm->files.size() > 0) {
        const auto& entry = fm->files[fm->file_position];
        if (fs::is_directory(entry.status()) || fs::is_symlink(entry.status()))
            change_folder(fm, entry.path().filename().string());
        // this would allow to run a file in a file explorer, need to make it an option
        //else if (fs::is_regular_file(entry.status()))
            //execl("/usr/bin/vim", "/usr/bin/vim", entry.path().filename().c_str(), NULL);
    }

    if (input == 'q')
        return -1;
    return 0;
}

int main_app_loop(FileManager *fm)
{
    fm->cwd = fs::current_path().string();
    fm->file_position = 0;
    fm->directory_change = true;
    fm->sort_type = ALPHABETICAL_INCREASING;
    fm->hidden_files = false;
    fm->preview = true;
    int user_return = 0;

    WINDOW *files_list_wd = subwin(stdscr, LINES, COLS / 2, 0, 0);

    WINDOW *file_preview_wd = subwin(stdscr, LINES, COLS / 2, 0, COLS / 2);

    while (user_return == 0) {
        if (fm->directory_change) {
            fm->files = get_files_in_folder(fm->cwd, fm->sort_type, fm->hidden_files);
            fm->directory_change = false;
        }
        if (fm->files.size() != 0 && fm->preview)
            preview_file(fm->files[fm->file_position], file_preview_wd, fm);
        display_files(files_list_wd, fm);
        user_return = get_user_input(fm, file_preview_wd, files_list_wd);
        doupdate();
    }
    return 0;
}

int main(void)
{
    if (start_ncurses() == 1)
        return 1;
    FileManager fm;
    main_app_loop(&fm);
    close_ncurses();
}
