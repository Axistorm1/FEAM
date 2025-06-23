#include <algorithm>
#include <cstring>
#include <functional>
#include "file_manager.hpp"

namespace fs = filesystem;

static auto get_comparable_key(const fs::directory_entry& entry)
{
    return make_tuple(!entry.is_directory(),  // show directories before files
                      entry.path().filename().string());
}

// Alphabetical sorting functions
static bool alphabetical_increasing_sort(const fs::directory_entry& entry_a,
                                         const fs::directory_entry& entry_b)
{
    const auto key_a = get_comparable_key(entry_a);
    const auto key_b = get_comparable_key(entry_b);

    if (get<0>(key_a) != get<0>(key_b)) {
        return static_cast<int>(get<0>(key_a)) <
               static_cast<int>(get<0>(key_b));
    }
    return strcasecmp(get<1>(key_a).c_str(), get<1>(key_b).c_str()) < 0;
}

static bool alphabetical_decreasing_sort(const fs::directory_entry& entry_a,
                                         const fs::directory_entry& entry_b)
{
    return !alphabetical_increasing_sort(entry_a, entry_b);
}

// Case-sensitive versions
static bool alphabetical_increasing_case_sort(
    const fs::directory_entry& entry_a, const fs::directory_entry& entry_b)
{
    const auto key_a = get_comparable_key(entry_a);
    const auto key_b = get_comparable_key(entry_b);

    if (get<0>(key_a) != get<0>(key_b)) {
        return static_cast<int>(get<0>(key_a)) <
               static_cast<int>(get<0>(key_b));
    }
    return get<1>(key_a) < get<1>(key_b);
}

static bool alphabetical_decreasing_case_sort(
    const fs::directory_entry& entry_a, const fs::directory_entry& entry_b)
{
    return !alphabetical_increasing_case_sort(entry_a, entry_b);
}

// File size sorting
static auto get_size_comparable_key(const fs::directory_entry& entry)
{
    return make_tuple(!entry.is_regular_file(),  // Directories first
                      entry.is_regular_file()
                          ? entry.file_size()
                          : count_files_in_folder(entry.path()));
}

static bool file_size_increasing_sort(const fs::directory_entry& entry_a,
                                      const fs::directory_entry& entry_b)
{
    return get_size_comparable_key(entry_a) < get_size_comparable_key(entry_b);
}

static bool file_size_decreasing_sort(const fs::directory_entry& entry_a,
                                      const fs::directory_entry& entry_b)
{
    return get_size_comparable_key(entry_a) > get_size_comparable_key(entry_b);
}

// Modification time sorting
static auto get_time_comparable_key(const fs::directory_entry& entry)
{
    return make_tuple(!entry.is_regular_file(),  // Directories first
                      entry.last_write_time());
}

static bool last_modified_sort(const fs::directory_entry& entry_a,
                               const fs::directory_entry& entry_b)
{
    return get_time_comparable_key(entry_a) > get_time_comparable_key(entry_b);
}

static bool first_modified_sort(const fs::directory_entry& entry_a,
                                const fs::directory_entry& entry_b)
{
    return get_time_comparable_key(entry_a) < get_time_comparable_key(entry_b);
}

// Main sorting function
void sort_files(vector<fs::directory_entry>* files, int sort_type)
{
    static const unordered_map<int, function<bool(const fs::directory_entry&,
                                                  const fs::directory_entry&)>>
        sort_functions = {
            {ALPHABETICAL_INCREASING, alphabetical_increasing_sort},
            {ALPHABETICAL_DECREASING, alphabetical_decreasing_sort},
            {ALPHABETICAL_INCREASING_CASE_SENSITIVE,
             alphabetical_increasing_case_sort},
            {ALPHABETICAL_DECREASING_CASE_SENSITIVE,
             alphabetical_decreasing_case_sort},
            {FILE_SIZE_INCREASING, file_size_increasing_sort},
            {FILE_SIZE_DECREASING, file_size_decreasing_sort},
            {LAST_MODIFIED, last_modified_sort},
            {FIRST_MODIFIED, first_modified_sort}};

    if (auto sort_func = sort_functions.find(sort_type);
        sort_func != sort_functions.end()) {
        sort(files->begin(), files->end(), sort_func->second);
    }
}

// Display sort information in the bottom right of the files list
void display_sort_info(WINDOW* window, int sort_type)
{
    static const unordered_map<int, pair<const char*, int>> sort_descriptions =
        {{ALPHABETICAL_INCREASING, {" aA->zZ ", 9}},
         {ALPHABETICAL_DECREASING, {" zZ->aA ", 9}},
         {ALPHABETICAL_INCREASING_CASE_SENSITIVE, {" A->Z a->z ", 12}},
         {ALPHABETICAL_DECREASING_CASE_SENSITIVE, {" z->a Z->A ", 12}},
         {FILE_SIZE_INCREASING, {" small->big ", 13}},
         {FILE_SIZE_DECREASING, {" big->small ", 13}},
         {LAST_MODIFIED, {" new->old ", 11}},
         {FIRST_MODIFIED, {" old->new ", 11}}};

    int width = getmaxx(window);
    int height = getmaxy(window);
    if (auto sort_func = sort_descriptions.find(sort_type);
        sort_func != sort_descriptions.end()) {
        mvwprintw(window, height - 1, width - sort_func->second.second, "%s",
                  sort_func->second.first);
    } else {
        mvwprintw(window, height - 1, width - 12, " ?->? ");
    }
}
