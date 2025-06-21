#include "file_manager.hpp"

static bool is_line_ascii(string line)
{
    for (size_t i = 0; i < line.size(); i++)
        if (!isascii(line[i]))
            return false;
    return true;
}

// read all file (or at least a big chunk) to be able to scroll without re-reading everything
static bool preview_text_file(string file_path, WINDOW *wd, size_t file_size)
{
    ifstream file(file_path, ios::binary);

    if (!file)
        return false;
    
    size_t height = getmaxy(wd), width = getmaxx(wd); 

    string line;

    for (size_t i = 0; i < height - 2 && getline(file, line); i++) {
        if (line.size() == 0)
            continue;
        if (line.size() > width - 2)
            line.erase(width - 2);
        if (!is_line_ascii(line)){
            werase(wd);
            box(wd, ACS_VLINE, ACS_HLINE);
            file.close();
            return false;
        }
        mvwprintw(wd, i + 1, 1, "%s", line.c_str());
    }

    mvwprintw(wd, 0, 2, " Text file [%s] - %s ", file_path.c_str(), format_bytes(file_size).c_str());

    file.close();
    return true;
}

static void preview_folder(fs::directory_entry folder, WINDOW *wd, FileManager *fm)
{
    static map<string, vector <fs::directory_entry>> loaded_folders;
    const size_t max_cached_folders = 100;

    string folder_path = FILE_PATH(folder);
    vector <fs::directory_entry> files;

    if (loaded_folders.size() > max_cached_folders)
        loaded_folders.clear();

    if (loaded_folders.count(folder_path))
        files = loaded_folders.at(folder_path);
    else {
        files = get_files_in_folder(folder_path, fm->sort_type, fm->hidden_files);
        loaded_folders.insert(std::pair(folder_path, files));
    }

    string display_path = fm->cwd;
    if (!folder_path.empty()) {
        if (!display_path.empty() && display_path.back() != '/') {
            display_path += '/';
        }
        display_path += folder_path;
    }

    if (files.size() != 0)
        mvwprintw(wd, 0, 2, " Folder [%s] - %ld %s ", display_path.c_str(), files.size(), files.size() > 1 ? "files" : "file");

    if (files.size() == 0) {
        mvwprintw(wd, 0, 2, "Folder [%s] - Empty ", (fm->cwd + "/" + folder_path).c_str());
        ERROR_ATTRON(wd);
        mvwprintw(wd, 1, 1, "Directory is empty");
        ERROR_ATTROFF(wd);     
        return;
    }

    size_t height = getmaxy(wd), width = getmaxx(wd);

    for (size_t i = 0; i < files.size() && i < height - 2; i++)
        mvwprintw(wd, i + 1, 1, "%.*s%c", static_cast <int>(width - 3), FILE_PATH(files[i]).c_str(), FILE_PATH(files[i]).size() > width - 3 ? '+' : ' ');   
}

static bool check_magic_number(char buffer[16], const unsigned char *magic_number)
{
    for (size_t i = 0; i < 16 && magic_number[i]; i++)
        if (buffer[i] != magic_number[i])
            return false;
    return true;
}

static bool preview_binary_file(fs::directory_entry file, WINDOW *wd)
{
    ifstream file_stream(FILE_PATH(file), ios::binary);

    if (!file_stream)
        return false;

    char magic_number[16] = {0};

    file_stream.read(magic_number, 16);

    string file_type = "Unknown";

    if (check_magic_number(magic_number, ELF_MAGIC_NUMBER))
        file_type = "ELF";

    mvwprintw(wd, 0, 2, " %s file - %s ", file_type.c_str(), format_bytes(file.file_size()).c_str());

    file_stream.close();

    return true;
}

void preview_file(fs::directory_entry file, WINDOW *wd, FileManager *fm)
{
    werase(wd);

    box(wd, ACS_VLINE, ACS_HLINE);

    if (!can_read_file(FILE_PATH(file))) {
        ERROR_ATTRON(wd);
        mvwprintw(wd, 1, 1, "Missing permissions");
        ERROR_ATTROFF(wd);
        wrefresh(wd);
        return;
    }
        

    if (file.is_regular_file() && file.file_size() == 0) {
        ERROR_ATTRON(wd);
        mvwprintw(wd, 1, 1, "Empty file");
        ERROR_ATTROFF(wd);
        wrefresh(wd);
        return;
    }

    bool previewed;

    if (file.is_directory()) {
        preview_folder(file, wd, fm);
        previewed = true;
    }

    if (file.is_regular_file())
        previewed = preview_text_file(FILE_PATH(file), wd, file.file_size());
    
    if (!previewed)
        previewed = preview_binary_file(file, wd);

    if (!previewed) {
        ERROR_ATTRON(wd);
        mvwprintw(wd, 1, 1, "Couldn't preview file");
        ERROR_ATTROFF(wd);
        wrefresh(wd);
        return;
    }

    wrefresh(wd);
}