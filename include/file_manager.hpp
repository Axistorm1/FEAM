#ifndef FILE_MANAGER_H_
    #define FILE_MANAGER_H_

    #include <string>
    #include <vector>
    #include <algorithm>
    #include <fstream>
    #include <filesystem>
    #include <cstdio>
    #include <cmath>
    using namespace std;

    namespace fs = filesystem;

    #include <ncurses.h>
    #ifndef CTRL // ncurses specific macro
    #define CTRL(c) ((c) & 037)
    #endif

    #define FILE_PATH(file) (file).path().filename().string()

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
        size_t file_position;
        bool directory_change;
        bool hidden_files;
        int sort_type;
        vector <fs::directory_entry> files;
};

// ncurses_setup.cpp
int start_ncurses();
void close_ncurses();

// sort_functions.cpp
void sort_files(vector <fs::directory_entry> *files, int sort_type);

// files_list.cpp
vector<fs::directory_entry> get_files_in_folder(const string &folder, int sort_type, bool hidden_files);
void display_files(WINDOW *wd, FileManager *fm);

#endif /* FILE_MANAGER_H_ */