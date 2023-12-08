# File System Project

## Overview

This project implements a simple file system simulation that operates on a virtual floppy disk image. The file system allows users to perform basic file operations such as creating, deleting, listing, and printing text files.

## Features

- **Create Text File (`M` Command):** Users can create a new text file on the virtual floppy disk by providing a file name. The content of the text file can be entered interactively.

- **Delete File (`D` Command):** Users can delete an existing text file from the virtual floppy disk by specifying the file name.

- **List Files (`L` Command):** The system can display a list of all existing files on the virtual floppy disk, including their names, extensions, and sizes.

- **Print Text File (`P` Command):** Users can print the content of a text file by providing the file name. If the file is marked as unreadable (`x` extension), a corresponding message will be displayed.

## Compilation

To compile the project, follow these steps:

1. Open a terminal in the project's directory.

2. Use the provided make file by typing `make`

Or If you wish to compile manually, run the command:

   ```bash
   gcc filesys.c -o filesystem
   ```

## Usage

To run the program, use the following command:

```bash
./filesystem <command> [filename]
    <command>: A single character representing the operation to perform (M for create, D for delete, L for list, P for print).
    [filename]: Optional parameter for specifying the file name when required by the command.
