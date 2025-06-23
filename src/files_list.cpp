#include <ncurses.h>
#include "file_manager.hpp"

vector<fs::directory_entry> get_files_in_folder(const std::string &folder)
{
    vector<fs::directory_entry> files;

    try {
        for (const auto &entry : fs::directory_iterator(folder)) {
            if (entry.path().filename() == "." ||
                entry.path().filename() == "..") {
                continue;
            }
            files.push_back(entry);
        }

    } catch (const fs::filesystem_error &e) {
        return files;
    }
    return files;
}

vector<fs::directory_entry> load_folder(FileManager *file_manager,
                                        const std::string &folder)
{
    const size_t max_cached_folders = 100;

    auto &cache = file_manager->folders_cache;
    auto cached_pair = cache.find(folder);

    vector<fs::directory_entry> files;

    if (cached_pair != cache.end()) {
        files = cached_pair->second;
    } else {
        // should clear based on a score or something
        if (cache.size() >= max_cached_folders) {
            cache.clear();
        }

        files = get_files_in_folder(folder);
        cache[folder] = files;
    }

    // Filter out hidden files if needed
    if (!file_manager->hidden_files) {
        files.erase(remove_if(files.begin(), files.end(),
                              [](const fs::directory_entry &entry) {
                                  return FILE_NAME(entry).front() == '.';
                              }),
                    files.end());
    }

    sort_files(&files, file_manager->sort_type);
    return files;
}

bool can_read_file(const string &file_path)
{
    ifstream file(file_path);

    bool state = file.good();

    file.close();

    return state;
}

static string read_repo_data(const string &file_path)
{
    ifstream file(file_path);

    if (!file) {
        return "";
    }

    string line;
    string repo_name;
    string branch_name;

    while (getline(file, line)) {
        if (line.find("url") != string::npos) {
            repo_name = line.erase(line.find_last_of('.'))
                            .substr(line.find_last_of('/') + 1);
        }
        if (line.find("branch \"") != string::npos) {
            branch_name = line.erase(line.find_last_of('\"'))
                              .substr(line.find_first_of('\"') + 1);
        }
    }

    return repo_name + " <" + branch_name + ">";
}

string find_git_repo(string folder)
{
    while (!folder.empty() && (folder.back() == '/')) {
        folder.pop_back();
    }

    while (!folder.empty()) {
        string config_path = folder + "/.git/config";
        if (can_read_file(config_path)) {
            return read_repo_data(config_path);
        }

        size_t last_slash = folder.find_last_of('/');
        if (last_slash == string::npos) {
            break;
        }
        folder = folder.substr(0, last_slash);
    }

    return "";
}

static void display_folder_info(WINDOW *window, FileManager *file_manager)
{
    string folder_path = file_manager->cwd;

    size_t width = getmaxx(window);

    if (width < 24) {
        return;
    }

    // 24 is basically the "margin" allowed to everything that isn't the
    // folder's name
    if (folder_path.size() + 24 > width) {
        folder_path.erase(width - 24);
        folder_path.append("+");
    }

    if (file_manager->files.size() == 0) {
        mvwprintw(window, 0, 2, " [%s] - Empty ", folder_path.c_str());
        return;
    }
    mvwprintw(window, 0, 2, " [%s] - Entry %ld/%ld ", folder_path.c_str(),
              file_manager->file_position + 1, file_manager->files.size());

    string repo_name = find_git_repo(file_manager->cwd);

    if (repo_name.size() != 0) {
        mvwprintw(window, LINES - 1, 2, " Git: %s ", repo_name.c_str());
    }
}

size_t count_files_in_folder(const string &folder)
{
    size_t count;
    try {
        count =
            distance(fs::directory_iterator(folder), fs::directory_iterator{});
    } catch (const fs::filesystem_error &e) {
        return 0;
    }
    return count;
}

string format_bytes(uint64_t bytes)
{
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
    uint64_t divisor = 1000;

    if (bytes == 0) {
        return "0";
    }

    int magnitude = static_cast<int>(log10(bytes) / log10(divisor));
    magnitude = min(magnitude, 6);

    double scaledSize = static_cast<double>(bytes) / pow(divisor, magnitude);

    char buffer[32];

    // A lot of stuff just to have 3 total digits
    if (scaledSize < 10) {
        snprintf(buffer, sizeof(buffer), "%.2f", scaledSize);
        string numStr(buffer);
        numStr = numStr.substr(0, 3);
        if (numStr.back() == '.') {
            numStr.pop_back();
        }
        snprintf(buffer, sizeof(buffer), "%s %s", numStr.c_str(),
                 suffixes[magnitude]);
    } else if (scaledSize < 100) {
        snprintf(buffer, sizeof(buffer), "%.1f", scaledSize);
        string numStr(buffer);
        numStr = numStr.substr(0, 3);
        if (numStr.back() == '.') {
            numStr.pop_back();
        }
        snprintf(buffer, sizeof(buffer), "%s %s", numStr.c_str(),
                 suffixes[magnitude]);
    } else {
        snprintf(buffer, sizeof(buffer), "%.0f %s", scaledSize,
                 suffixes[magnitude]);
    }

    return buffer;
}

int find_file_color(const fs::directory_entry &file)
{
    if (file.is_symlink()) {
        return MAGENTA;
    }
    if (file.is_directory()) {
        return CYAN;
    }
    if (file.is_regular_file()) {
        return GREEN;
    }
    return WHITE;
}

void display_files(WINDOW *window, FileManager *file_manager)
{
    werase(window);

    size_t height = getmaxy(window);
    size_t width = getmaxx(window);

    box(window, ACS_VLINE, ACS_HLINE);

    display_folder_info(window, file_manager);
    display_sort_info(window, file_manager->sort_type);

    if (file_manager->files.size() == 0) {
        ERROR_ATTRON(window);
        mvwprintw(window, 1, 1, "Directory is empty");
        ERROR_ATTROFF(window);
        wrefresh(window);
        return;
    }

    size_t available_rows = height - 2;

    size_t start_pos = 0;
    if (file_manager->files.size() > available_rows) {
        int tmp = file_manager->file_position -
                  available_rows / 2;  // Divide by 2 to center
        if (tmp < 0) {
            start_pos = 0;
        } else {
            start_pos = tmp;
        }
        if (start_pos > file_manager->files.size() - available_rows) {
            start_pos = file_manager->files.size() - available_rows;
        }
    }

    size_t end_pos =
        min(start_pos + available_rows, file_manager->files.size());
    for (size_t i = start_pos; i < end_pos; i++) {
        fs::directory_entry file = file_manager->files[i];

        if (!can_read_file(FILE_PATH(file))) {
            wattron(window, COLOR_PAIR(1));
        } else {
            wattron(window, COLOR_PAIR(find_file_color(file)));
        }

        if (i == file_manager->file_position) {
            wattron(window,
                    A_REVERSE);  // Reverse colors to highlight selected file
        }

        string file_line = FILE_NAME(file);

        if (file_manager->files[i].is_symlink()) {
            file_line.append(" -> ").append(
                fs::read_symlink(file_manager->files[i].path()).string());
        }

        // string is a byte format (125 B, 78 MB...) for regular files and the
        // file count for folders
        string byte_format =
            file.is_regular_file()
                ? format_bytes(file.file_size())
                : to_string(count_files_in_folder(FILE_PATH(file)));

        // file names too long
        if (file_line.size() > width - 11) {
            file_line.erase(width - 11);
            file_line += '+';
        }

        // add zeros between file name and byte format
        int to_append =
            width - 10 - file_line.size() + (7 - byte_format.size());
        if (to_append < 0) {
            to_append = 0;
        }
        file_line.append(to_append, ' ');

        file_line += byte_format;

        mvwprintw(window, i - start_pos + 1, 1, "%s", file_line.c_str());

        wattrset(window, A_NORMAL);
    }

    wrefresh(window);
}
