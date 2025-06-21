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
    string folder_path = FILE_PATH(folder);
    vector <fs::directory_entry> files = get_files_in_folder(folder_path, fm->sort_type, fm->hidden_files);

    if (files.size() != 0)
        mvwprintw(wd, 0, 2, " Folder [%s] - %ld %s ", (fm->cwd + "/" + folder_path).c_str(), files.size(), files.size() > 1 ? "files" : "file");

    if (!can_read_file(folder_path)) {
        wattron(wd, COLOR_PAIR(1));
        wattron(wd, A_UNDERLINE);
        mvwprintw(wd, 1, 1, "Missing permissions");
        wattroff(wd, A_UNDERLINE);
        wattroff(wd, COLOR_PAIR(1));
        return;
    }

    if (files.size() == 0) {
        mvwprintw(wd, 0, 2, "Folder [%s] - Empty ", (fm->cwd + "/" + folder_path).c_str());
        wattron(wd, COLOR_PAIR(1));
        wattron(wd, A_UNDERLINE);
        mvwprintw(wd, 1, 1, "Directory is empty");
        wattroff(wd, A_UNDERLINE);
        wattroff(wd, COLOR_PAIR(1));        
        return;
    }

    size_t height = getmaxy(wd);

    for (size_t i = 0; i < files.size() && i < height - 2; i++)
        mvwprintw(wd, i + 1, 1, "%s", FILE_PATH(files[i]).c_str());    
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

    mvwprintw(wd, 0, 2, " %s file - ", file_type.c_str());

    file_stream.close();

    return true;
}

void preview_file(fs::directory_entry file, WINDOW *wd, FileManager *fm)
{
    werase(wd);

    box(wd, ACS_VLINE, ACS_HLINE);

    if (file.is_regular_file() && file.file_size() == 0) {
        wattron(wd, COLOR_PAIR(1));
        wattron(wd, A_UNDERLINE);
        mvwprintw(wd, 1, 1, "Empty file");
        wattroff(wd, A_UNDERLINE);
        wattroff(wd, COLOR_PAIR(1));
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
    
    if (!previewed) {
        previewed = preview_binary_file(file, wd);
    }

    if (!previewed) {
        wattron(wd, COLOR_PAIR(1));
        wattron(wd, A_UNDERLINE);
        mvwprintw(wd, 1, 1, "Couldn't preview file");
        wattroff(wd, A_UNDERLINE);
        wattroff(wd, COLOR_PAIR(1));
        wrefresh(wd);
        return;
    }

    wrefresh(wd);
}