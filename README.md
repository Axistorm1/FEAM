# File Manager

A terminal-based file manager written in C++ using [ncurses](https://invisible-island.net/ncurses/) for a text-based user interface. It allows you to browse, preview, and sort files and directories interactively.

## Features

- **Directory Navigation:** Move between folders, including following symlinks.
- **File Preview:** View text files, binary files (with type detection), and folder contents.
- **Sorting:** Sort files by name, size, or modification time, with both increasing and decreasing options, and case sensitivity.
- **Hidden Files:** Toggle visibility of hidden files (dotfiles).
- **Git Integration:** Shows Git repository and branch if present in the current directory.
- **Color Support:** Uses colors to distinguish file types (directories, symlinks, etc.).
- **Keyboard Shortcuts:** Navigate and control the interface using the keyboard.
- **Integrated Shell:** Write commands directly in the file manager.

## Controls

| Key           | Action                              |
|---------------|-------------------------------------|
| `UP/DOWN`     | Move selection up/down              |
| `LEFT`        | Go to parent directory              |
| `RIGHT/ENTER` | Enter selected directory            |
| `a`           | Toggle hidden files                 |
| `p`           | Toggle file preview pane            |
| `s`           | Cycle through sorting modes         |
| `t`           | Open shell                          |
| `f`           | Search in files list                |
| `h`           | Open help menu                      |
| `q`           | Quit                                |

## Build Instructions

1. **Dependencies:**
   - C++17 compiler (e.g., `g++`)
   - [ncurses](https://invisible-island.net/ncurses/) library

2. **Build:**
   Run:
   ```sh
   make
   ```
This will produce the [file_manager](file_manager) executable.

3. **Run:**
   ```sh
   ./file_manager
   ```

## Project Structure

   - [src](src) — Source files for the application logic
   - [file_manager.hpp](include/file_manager.hpp) — Main header file
   - [Makefile](Makefile) — Build instructions
