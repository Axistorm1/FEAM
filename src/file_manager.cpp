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
}

int get_user_input(WINDOW *wd, FileManager *fm)
{
    int input = wgetch(wd);

    // enable/disable hidden files (.*)
    if (input == 'a') {
        fm->hidden_files = !fm->hidden_files;
        fm->directory_change = true;
    }

    // loop through sorts
    if (input == 's') {
        fm->sort_type++;
        if (fm->sort_type > FILE_SIZE_DECREASING)
            fm->sort_type = 0;
        fm->directory_change = true;
    }

    // move through files
    // TODO: Make it wrap
    if (input == KEY_UP && fm->file_position > 0)
        fm->file_position--;
    if (input == KEY_DOWN && fm->file_position < fm->files.size() - 1)
        fm->file_position++;

    if (input == 534)
        fm->file_position = fm->files.size() - 1;
    if (input == 575)
        fm->file_position = 0;

    if (input == KEY_LEFT && fm->cwd != "/")
        change_folder(fm, "..");

    if (input == KEY_ENTER || input == KEY_RIGHT) {
        const auto& entry = fm->files[fm->file_position];
        if (fs::is_directory(entry.status()) || fs::is_symlink(entry.status()))
            change_folder(fm, entry.path().filename().string());
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
    int user_return = 0;

    WINDOW *files_list_wd = subwin(stdscr, LINES, COLS / 2, 0, 0);
    keypad(files_list_wd, true);

    while (user_return == 0) {
        if (fm->directory_change) {
            fm->files = get_files_in_folder(fm->cwd, fm->sort_type, fm->hidden_files);
            fm->directory_change = false;
        }
        display_files(files_list_wd, fm);
        user_return = get_user_input(files_list_wd, fm);
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
