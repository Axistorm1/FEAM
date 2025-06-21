#include "file_manager.hpp"

#include <string.h>

static bool alphabetical_increasing_sort(const fs::directory_entry& a, const fs::directory_entry& b)
{
    const bool a_is_dir = a.is_directory();
    const bool b_is_dir = b.is_directory();

    // Directories come before files
    if (a_is_dir != b_is_dir)
        return a_is_dir;

    return strcasecmp(
        a.path().filename().string().c_str(),
        b.path().filename().string().c_str()
    ) < 0;
}

static bool alphabetical_decreasing_sort(fs::directory_entry a, fs::directory_entry b)
{   
    const bool a_is_dir = a.is_directory();
    const bool b_is_dir = b.is_directory();

    // Directories come before files
    if (a_is_dir != b_is_dir)
        return a_is_dir;

    return strcasecmp(
        a.path().filename().string().c_str(),
        b.path().filename().string().c_str()
    ) > 0;
}

static bool alphabetical_increasing_case_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    const bool a_is_dir = a.is_directory();
    const bool b_is_dir = b.is_directory();

    // Directories come before files
    if (a_is_dir != b_is_dir)
        return a_is_dir;

    return strcmp(
        a.path().filename().string().c_str(),
        b.path().filename().string().c_str()
    ) < 0;
}

static bool alphabetical_decreasing_case_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    const bool a_is_dir = a.is_directory();
    const bool b_is_dir = b.is_directory();

    // Directories come before files
    if (a_is_dir != b_is_dir)
        return a_is_dir;

    return strcmp(
        a.path().filename().string().c_str(),
        b.path().filename().string().c_str()
    ) > 0;
}

static bool file_size_increasing_sort(const fs::directory_entry& a, const fs::directory_entry& b)
{
    const bool a_is_file = a.is_regular_file();
    const bool b_is_file = b.is_regular_file();
    
    // Directories come before files
    if (a_is_file != b_is_file)
        return !a_is_file;
    
    if (a_is_file)
        return a.file_size() < b.file_size();
    
    return count_files_in_folder(a.path()) < count_files_in_folder(b.path());
}

static bool file_size_decreasing_sort(const fs::directory_entry& a, const fs::directory_entry& b)
{
    const bool a_is_file = a.is_regular_file();
    const bool b_is_file = b.is_regular_file();
    
    // Directories come before files
    if (a_is_file != b_is_file)
        return !a_is_file;
    
    if (a_is_file)
        return a.file_size() > b.file_size();
    
    return count_files_in_folder(a.path()) > count_files_in_folder(b.path());
}

static bool last_modified_sort(const fs::directory_entry& a, const fs::directory_entry& b)
{
    const bool a_is_file = a.is_regular_file();
    const bool b_is_file = b.is_regular_file();
    
    // Directories come before files
    if (a_is_file != b_is_file)
        return !a_is_file;
    
    return a.last_write_time() > b.last_write_time();
}
static bool first_modified_sort(const fs::directory_entry& a, const fs::directory_entry& b)
{
    const bool a_is_file = a.is_regular_file();
    const bool b_is_file = b.is_regular_file();
    
    // Directories come before files
    if (a_is_file != b_is_file)
        return !a_is_file;

    return a.last_write_time() < b.last_write_time();
}

void sort_files(vector <fs::directory_entry> *files, int sort_type)
{
    if (sort_type == ALPHABETICAL_INCREASING)
        sort(files->begin(), files->end(), alphabetical_increasing_sort);
    if (sort_type == ALPHABETICAL_DECREASING)
        sort(files->begin(), files->end(), alphabetical_decreasing_sort);
    if (sort_type == ALPHABETICAL_INCREASING_CASE_SENSITIVE)
        sort(files->begin(), files->end(), alphabetical_increasing_case_sort);
    if (sort_type == ALPHABETICAL_DECREASING_CASE_SENSITIVE)
        sort(files->begin(), files->end(), alphabetical_decreasing_case_sort);
    if (sort_type == FILE_SIZE_INCREASING)
        sort(files->begin(), files->end(), file_size_increasing_sort);
    if (sort_type == FILE_SIZE_DECREASING)
        sort(files->begin(), files->end(), file_size_decreasing_sort);
    if (sort_type == LAST_MODIFIED)
        sort(files->begin(), files->end(), last_modified_sort);
    if (sort_type == FIRST_MODIFIED)
        sort(files->begin(), files->end(), first_modified_sort);
}

void display_sort_info(WINDOW *wd, int sort_type)
{
    int width = getmaxx(wd);

    switch (sort_type) {
        case ALPHABETICAL_INCREASING:
            mvwprintw(wd, LINES - 1, width - 9, " aA->zZ ");
            break;
        case ALPHABETICAL_DECREASING:
            mvwprintw(wd, LINES - 1, width - 9, " zZ->aZ ");
            break;
        case ALPHABETICAL_INCREASING_CASE_SENSITIVE:
            mvwprintw(wd, LINES - 1, width - 12, " A->Z a->z ");
            break;
        case ALPHABETICAL_DECREASING_CASE_SENSITIVE:
            mvwprintw(wd, LINES - 1, width - 12, " z->a Z->A ");
            break;
        case FILE_SIZE_INCREASING:
            mvwprintw(wd, LINES - 1, width - 13, " small->big ");
            break;
        case FILE_SIZE_DECREASING:
            mvwprintw(wd, LINES - 1, width - 13, " big->small ");
            break;
        case LAST_MODIFIED:
            mvwprintw(wd, LINES - 1, width - 11, " new->old ");
            break;
        case FIRST_MODIFIED:
            mvwprintw(wd, LINES - 1, width - 11, " old->new ");
            break;
        default:
            mvwprintw(wd, LINES - 1, width - 12, " ?->? ");
    }
}