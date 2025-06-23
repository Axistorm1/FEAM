#include <ncurses.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include "file_manager.hpp"

static bool is_line_ascii(const string &line)
{
    return all_of(line.begin(), line.end(), isascii);
}

// read all file (or at least a big chunk) to be able to scroll without
// re-reading everything
static bool preview_text_file(const string &file_path, WINDOW *window,
                              size_t file_size)
{
    ifstream file(file_path, ios::binary);

    if (!file) {
        return false;
    }

    size_t height = getmaxy(window);
    size_t width = getmaxx(window);

    string line;

    for (size_t i = 0; i < height - 2 && getline(file, line); i++) {
        if (line.size() == 0) {
            continue;
        }
        if (line.size() > width - 2) {
            line.erase(width - 2);
        }
        if (!is_line_ascii(line)) {
            werase(window);
            box(window, ACS_VLINE, ACS_HLINE);
            file.close();
            return false;
        }
        mvwprintw(window, i + 1, 1, "%s", line.c_str());
    }

    mvwprintw(window, 0, 2, " Text file [%s] - %s ", file_path.c_str(),
              format_bytes(file_size).c_str());

    file.close();
    return true;
}

static void preview_folder(const fs::directory_entry &folder, WINDOW *window,
                           FileManager *file_manager)
{
    string folder_path = FILE_PATH(folder);
    vector<fs::directory_entry> files;

    files = load_folder(file_manager, folder_path);

    size_t width = getmaxx(window);

    if (width < 28) {
        return;
    }

    // 28 is basically the "margin" allowed to everything that isn't the
    // folder's name in the preview
    if (folder_path.size() + 28 > width) {
        folder_path.erase(width - 28);
        folder_path.append("+");
    }

    if (files.size() != 0) {
        mvwprintw(window, 0, 2, " Folder [%s] - %ld %s ", folder_path.c_str(),
                  files.size(), files.size() > 1 ? "files" : "file");
    }

    if (files.size() == 0) {
        mvwprintw(window, 0, 2, "Folder [%s] - Empty ", folder_path.c_str());
        ERROR_ATTRON(window);
        mvwprintw(window, 1, 1, "Directory is empty");
        ERROR_ATTROFF(window);
        return;
    }

    size_t height = getmaxy(window);

    for (size_t i = 0; i < files.size() && i < height - 2; i++) {
        if (!can_read_file(files[i].path().string())) {
            wattron(window, COLOR_PAIR(1));
        } else {
            wattron(window, COLOR_PAIR(find_file_color(files[i])));
        }
        mvwprintw(window, i + 1, 1, "%.*s%c", static_cast<int>(width - 3),
                  FILE_NAME(files[i]).c_str(),
                  FILE_NAME(files[i]).size() > width - 3 ? '+' : ' ');
        wattrset(window, A_NORMAL);
    }
}

static bool check_magic_number(const array<unsigned char, 16> buffer,
                               const array<unsigned char, 16> magic_number)
{
    for (int i = 0; i < 16 && magic_number.at(i) != 0; i++) {
        if (buffer.at(i) != magic_number.at(i)) {
            return false;
        }
    }
    return true;
}

static bool preview_binary_file(const fs::directory_entry &file, WINDOW *window)
{
    FILE *fstream = fopen(FILE_PATH(file).c_str(), "rb");

    array<unsigned char, 16> buffer = {0};

    if (fread(buffer.data(), 1, 16, fstream) == 0) {
        return false;
    }

    string file_type = "Unknown";

    if (check_magic_number(buffer, ELF_MAGIC_NUMBER)) {
        file_type = "ELF";
    }

    mvwprintw(window, 0, 2, " %s file - %s ", file_type.c_str(),
              format_bytes(file.file_size()).c_str());

    fclose(fstream);

    return true;
}

void preview_file(const fs::directory_entry &file, WINDOW *window,
                  FileManager *file_manager)
{
    werase(window);

    box(window, ACS_VLINE, ACS_HLINE);

    int width = getmaxx(window);

    string display_name = FILE_PATH(file);

    if (!can_read_file(FILE_PATH(file))) {
        if (display_name.size() > (size_t)width - 17) {
            display_name.erase(width - 17);
            display_name.append("+");
        }
        mvwprintw(window, 0, 2, " Unknown [%s] ", display_name.c_str());
        ERROR_ATTRON(window);
        mvwprintw(window, 1, 1, "Missing permissions");
        ERROR_ATTROFF(window);
        wrefresh(window);
        return;
    }

    if (file.is_regular_file() && file.file_size() == 0) {
        if (display_name.size() > (size_t)width - 20) {
            display_name.erase(width - 20);
            display_name.append("+");
        }
        mvwprintw(window, 0, 2, " Empty file [%s] ", display_name.c_str());
        ERROR_ATTRON(window);
        mvwprintw(window, 1, 1, "Empty file");
        ERROR_ATTROFF(window);
        wrefresh(window);
        return;
    }

    bool previewed;

    if (file.is_directory()) {
        preview_folder(file, window, file_manager);
        previewed = true;
    }

    if (file.is_regular_file()) {
        previewed =
            preview_text_file(FILE_PATH(file), window, file.file_size());
    }

    if (!previewed) {
        previewed = preview_binary_file(file, window);
    }

    if (!previewed) {
        ERROR_ATTRON(window);
        mvwprintw(window, 1, 1, "Couldn't preview file");
        ERROR_ATTROFF(window);
        wrefresh(window);
        return;
    }

    wrefresh(window);
}
