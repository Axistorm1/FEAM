#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

namespace fs = filesystem;

#include <ncurses.h>
#include <unistd.h>
#ifndef CTRL  // ncurses specific macro
#define CTRL(c) ((c) & 037)
#endif

#define ERROR_ATTRON(wd) (wattron(wd, A_UNDERLINE | COLOR_PAIR(1)))
#define ERROR_ATTROFF(wd) (wattroff(wd, A_UNDERLINE | COLOR_PAIR(1)))

#define FILE_PATH(file) (file).path().string()
// #define FILE_PATH(file) (file).path().filename().string()
#define FILE_NAME(file) (file).path().filename().string()

const array<unsigned char, 16> ELF_MAGIC_NUMBER = {0x7F, 'E', 'L', 'F', 0};

using sort_types_t = enum sort_types_e {
    ALPHABETICAL_INCREASING,
    ALPHABETICAL_DECREASING,
    ALPHABETICAL_INCREASING_CASE_SENSITIVE,
    ALPHABETICAL_DECREASING_CASE_SENSITIVE,
    FILE_SIZE_INCREASING,
    FILE_SIZE_DECREASING,
    LAST_MODIFIED,
    FIRST_MODIFIED,
};

using file_color_t = enum file_color_e {
    WHITE = 2,
    CYAN = 20,
    GREEN,
    YELLOW,
    MAGENTA
};

class FileManager {
  public:
    string cwd;
    map<string, vector<fs::directory_entry>> folders_cache;
    vector<fs::directory_entry> files;
    size_t file_position;
    string current_search;
    int sort_type;
    bool directory_change;
    bool hidden_files;
    bool preview;
    bool in_shell;
    bool in_search;
    bool help_menu;
};

// ncurses_setup.cpp
int start_ncurses();
void close_ncurses();
void handle_signals();

// sort_functions.cpp
void sort_files(vector<fs::directory_entry> *files, int sort_type);
void display_sort_info(WINDOW *window, int sort_type);

// files_list.cpp
vector<fs::directory_entry> get_files_in_folder(const string &folder);
void display_files(WINDOW *window, FileManager *file_manager);
string format_bytes(uint64_t bytes);
bool can_read_file(const string &file_path);
size_t count_files_in_folder(const string &folder);
int find_file_color(const fs::directory_entry &file);
vector<fs::directory_entry> load_folder(FileManager *file_manager,
                                        const std::string &folder,
                                        bool force_update, bool search);

// file_preview.cpp
void preview_file(const fs::directory_entry &file, WINDOW *window,
                  FileManager *file_manager);

// handle_shell.cpp
int run_command(string command, string current_file);
void display_shell(WINDOW *window, bool in_shell);
int handle_shell_input(WINDOW *window, FileManager *file_manager);
void handle_shell_return(int return_value, FileManager *file_manager);

// help_widget.cpp
void display_help(WINDOW *window);

// search_files.cpp
int handle_search_input(FileManager *file_manager);
vector<fs::directory_entry> search_files(
    const vector<fs::directory_entry> &files, const string &needle);
void display_search(WINDOW *window, FileManager *file_manager);

#endif /* FILE_MANAGER_H_ */
