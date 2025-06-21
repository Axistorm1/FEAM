#ifndef FILE_MANAGER_H_
    #define FILE_MANAGER_H_

    #include <string>
    #include <vector>
    #include <algorithm>
    #include <fstream>
    #include <iostream>
    #include <filesystem>
    #include <cstdio>
    #include <cmath>
    #include <cctype>
    using namespace std;

    namespace fs = filesystem;

    #include <unistd.h>

    #include <ncurses.h>
    #ifndef CTRL // ncurses specific macro
    #define CTRL(c) ((c) & 037)
    #endif

    #define FILE_PATH(file) (file).path().filename().string()

const unsigned char ELF_MAGIC_NUMBER[] = { 0x7F, 'E', 'L', 'F' , 0};

typedef enum sort_types_e {
    ALPHABETICAL_INCREASING,
    ALPHABETICAL_DECREASING,
    ALPHABETICAL_INCREASING_CASE_SENSITIVE,
    ALPHABETICAL_DECREASING_CASE_SENSITIVE,
    FILE_SIZE_INCREASING,
    FILE_SIZE_DECREASING,
} sort_types_t;

class FileManager{
    public:
        string cwd;
        vector <fs::directory_entry> files;
        size_t file_position;
        int sort_type;
        bool directory_change;
        bool hidden_files;
        bool preview;
};

// ncurses_setup.cpp
int start_ncurses();
void close_ncurses();

// sort_functions.cpp
void sort_files(vector <fs::directory_entry> *files, int sort_type);

// files_list.cpp
vector<fs::directory_entry> get_files_in_folder(const string &folder, int sort_type, bool hidden_files);
void display_files(WINDOW *wd, FileManager *fm);
string format_bytes(uint64_t bytes);
bool can_read_file(const string file_path);

// file_preview.cpp
void preview_file(fs::directory_entry file, WINDOW *wd, FileManager *fm);

#endif /* FILE_MANAGER_H_ */