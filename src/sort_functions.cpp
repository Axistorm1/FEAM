#include "file_manager.hpp"

#include <string.h>

static bool alphabetical_increasing_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    return strcasecmp(a.path().filename().string().c_str(), b.path().filename().string().c_str()) < 0;
}

static bool alphabetical_decreasing_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    return strcasecmp(b.path().filename().string().c_str(), a.path().filename().string().c_str()) < 0;
}

static bool alphabetical_increasing_case_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    return strcmp(a.path().filename().string().c_str(), b.path().filename().string().c_str()) < 0;
}

static bool alphabetical_decreasing_case_sort(fs::directory_entry a, fs::directory_entry b)
{ 
    return strcmp(b.path().filename().string().c_str(), a.path().filename().string().c_str()) < 0;
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
}