#include <ncurses.h>
#include <cstring>
#include <ctime>

#include "file_manager.hpp"

void change_folder(FileManager *file_manager, const string &folder)
{
    if (chdir(folder.c_str()) == -1) {
        return;
    }
    file_manager->cwd = fs::current_path().string();
    file_manager->file_position = 0;
    file_manager->directory_change = true;
    file_manager->files.clear();
}

void handle_enter_key(FileManager *file_manager)
{
    fs::directory_entry selected_entry =
        file_manager->files[file_manager->file_position];

    bool is_valid_directory = false;

    if (fs::is_directory(selected_entry) && !fs::is_symlink(selected_entry)) {
        is_valid_directory = true;
    }

    else if (fs::is_symlink(selected_entry)) {
        fs::path symlink_target = fs::read_symlink(selected_entry);
        // an example of this bullshit is /usr/bin/X11 which just links to .
        if (symlink_target.string() != ".") {
            is_valid_directory = true;
        }
    }

    if (is_valid_directory) {
        string folder_name = selected_entry.path().filename().string();
        change_folder(file_manager, folder_name);
    }
}

int get_user_input(FileManager *file_manager, WINDOW *file_preview_wd,
                   WINDOW *files_list_wd)
{
    int input = getch();

    // enable/disable hidden files (.*)
    if (input == 'a') {
        file_manager->hidden_files = !file_manager->hidden_files;
        file_manager->directory_change = true;
    }

    if (input == 'p') {
        file_manager->preview = !file_manager->preview;
        if (!file_manager->preview) {
            werase(file_preview_wd);
            wrefresh(file_preview_wd);
            wresize(files_list_wd, LINES, COLS);
        } else {
            wresize(files_list_wd, LINES, COLS / 2);
        }
    }

    // loop through sorts
    if (input == 's') {
        file_manager->sort_type++;
        if (file_manager->sort_type > FIRST_MODIFIED) {
            file_manager->sort_type = 0;
        }
        file_manager->directory_change = true;
    }

    // move through files
    if (input == KEY_UP) {
        file_manager->file_position--;
        if (file_manager->file_position > file_manager->files.size()) {
            file_manager->file_position = file_manager->files.size() - 1;
        }
    }
    if (input == KEY_DOWN && file_manager->files.size() != 0) {
        file_manager->file_position++;
        if (file_manager->file_position > file_manager->files.size() - 1) {
            file_manager->file_position = 0;
        }
    }

    if (input == 534) {
        file_manager->file_position = file_manager->files.size() - 1;
    }
    if (input == 575) {
        file_manager->file_position = 0;
    }

    if (input == KEY_LEFT && file_manager->cwd != "/") {
        change_folder(file_manager, "..");
    }

    if ((input == KEY_ENTER || input == KEY_RIGHT || input == 10) &&
        !file_manager->files.empty()) {
        handle_enter_key(file_manager);
    }

    if (input == 'q') {
        return -1;
    }
    return 0;
}

int main_app_loop(FileManager *file_manager)
{
    file_manager->cwd = fs::current_path().string();
    file_manager->file_position = 0;
    file_manager->directory_change = true;
    file_manager->sort_type = ALPHABETICAL_INCREASING;
    file_manager->hidden_files = false;
    file_manager->preview = true;
    int user_return = 0;

    WINDOW *files_list_wd = subwin(stdscr, LINES, COLS / 2, 0, 0);

    WINDOW *file_preview_wd = subwin(stdscr, LINES, COLS / 2, 0, COLS / 2);

    while (user_return == 0) {
        if (file_manager->directory_change) {
            file_manager->files = load_folder(file_manager, file_manager->cwd);
            file_manager->directory_change = false;
        }
        if (file_manager->files.size() != 0 && file_manager->preview) {
            preview_file(file_manager->files[file_manager->file_position],
                         file_preview_wd, file_manager);
        }
        display_files(files_list_wd, file_manager);
        user_return =
            get_user_input(file_manager, file_preview_wd, files_list_wd);
        doupdate();
    }
    return 0;
}

int main(void)
{
    if (start_ncurses() == 1) {
        return 1;
    }
    FileManager file_manager;
    main_app_loop(&file_manager);
    close_ncurses();
}
