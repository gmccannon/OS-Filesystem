// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:___George McCannon___ Date:__12/08/2023___
// filesys.c
// Based on a program by Michael Black, 2007
// Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define MAX_NAME_LENGTH 8
#define MAX_FILE_SIZE 12288
#define FLOPPY_IMAGE "floppya.img"

// Function to delete a file from the floppy disk
void deleteFile(FILE* floppy, char name[MAX_NAME_LENGTH]);

// Function to list all files on the floppy disk
void listFiles(FILE* floppy);

// Function to create a text file on the floppy disk
void createTextFile(FILE* floppy, char name[MAX_NAME_LENGTH]);

// Function to print the contents of a text file on the floppy disk
void printTextFile(FILE* floppy, char name[MAX_NAME_LENGTH]);

int main(int argc, char* argv[]) {
    // Array to store the file name
    char name[MAX_NAME_LENGTH] = {0};
    
    // Check for the correct number of command-line arguments
    if (argc < 2 || argc > 3) {
        printf("Invalid number of commands \n");
        return 0;
    }

    // If a file name is provided, copy it to the 'name' array
    if (argc > 2) {
        strncpy(name, argv[2], MAX_NAME_LENGTH - 1);
    }

    // Open the floppy disk image file in read/write mode
    FILE* floppy = fopen(FLOPPY_IMAGE, "r+");
    if (!floppy) {
        printf("%s not found\n", FLOPPY_IMAGE);
        return 0;
    }

    // Determine the command and execute the corresponding function
    switch (toupper(*argv[1])) {
        case 'D':
            deleteFile(floppy, name);
            break;

        case 'L':
            listFiles(floppy);
            break;

        case 'M':
            createTextFile(floppy, name);
            break;

        case 'P':
            printTextFile(floppy, name);
            break;

        default:
            printf("Unknown command\n");
    }

    // Close the floppy disk image file
    fclose(floppy);
    return 0;
}

// Function to delete a file from the floppy disk
void deleteFile(FILE* floppy, char name[MAX_NAME_LENGTH]) {
    // Check if a filename is provided
    if (name[0] != -1) {
        printf("Command D\n");

        // Load the directory from sector 257
        char dir[512];
        int i, j;
        bool found = 0;
        int secStart, secSize;
        fseek(floppy, 512 * 257, SEEK_SET);
        for (i = 0; i < 512; i++)
            dir[i] = fgetc(floppy);

        // Load the map
        char map[512];
        fseek(floppy, 512 * 256, SEEK_SET);
        for (i = 0; i < 512; i++)
            map[i] = fgetc(floppy);

        // Iterate through the directory entries
        for (i = 0; i < 512; i = i + 16) {
            if (dir[i] == 0) {
                continue;
            }
            // Extract the file name from the directory entry
            char nameOfFile[8] = "";
            for (j = 0; j < 8; j++) {
                if (dir[i + j] != 0) {
                    nameOfFile[j] = dir[i + j];
                }
            }
            // Check if the file name matches the provided name
            if (strcmp(nameOfFile, name) == 0) {
                found = 1;
                dir[i] = 0;
                secStart = dir[i + 9];
                secSize = dir[i + 10];
                break;
            }
        }

        // If the file is found, delete it
        if (found) {
            for (i = 0; i < secSize; i++) {
                map[secStart] = 0;
                secStart++;
            }
            printf("%s was deleted.\n", name);
        } else {
            printf("%s doesn't exist.\n", name); // File doesn't exist
        }

        // Write the map and directory back to the floppy image
        fseek(floppy, 512 * 256, SEEK_SET);
        for (i = 0; i < 512; i++)
            fputc(map[i], floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        for (i = 0; i < 512; i++)
            fputc(dir[i], floppy);
    } else {
        printf("No filename has been given");
        return;
    }
}

// Function to list all files on the floppy disk
void listFiles(FILE* floppy) {
    printf("Command L\n");

    // Load the directory from sector 257
    int i, j, allocatedSectors = 0, allocatedBytes = 0;
    char dir[512];
    fseek(floppy, 512 * 257, SEEK_SET);
    for (i = 0; i < 512; i++)
        dir[i] = fgetc(floppy);

    printf("Files list:\n"); // Print list of files
    for (int i = 0; i < 512; i = i + 16) {
        if (dir[i] == 0) {
            continue;
        }

        // Extract the file name from the directory entry
        char nameOfFile[8] = "";
        for (j = 0; j < 8; j++) {
            if (dir[i + j] != 0) {
                nameOfFile[j] = dir[i + j];
            }
        }

        char extension;
        // Determine the file extension based on the directory entry
        if (toupper(dir[i + 8]) == 'T') {
            extension = 't';
        } else {
            if (toupper(dir[i + 8]) == 'X') {
                extension = 'x';
            }
        }

        // Update allocated sectors and bytes
        allocatedSectors = allocatedSectors + dir[i + 10];
        allocatedBytes = allocatedBytes + dir[i + 10] * 512;
        // Print file information
        printf("%8s.%c %d bytes used\n", nameOfFile, extension, dir[i + 10] * 512);
    }

    printf("\n");
    printf("Total space used up: %d sectors, %d bytes\n", allocatedSectors, allocatedBytes);
    printf("Free space left: %d sectors, %d bytes\n", 512 - allocatedSectors, 512 * 512 - allocatedBytes);
}

// Function to create a text file on the floppy disk
void createTextFile(FILE* floppy, char name[MAX_NAME_LENGTH]) {
    // Check if a filename is provided
    if (name[0] != -1) {
        printf("Command M\n");

        // Load the directory from sector 257
        char dir[512];
        int i, j;
        int freeDirectory = -1;
        int freeMap = -1;
        bool found = 0;
        bool isText = 0;

        fseek(floppy, 512 * 257, SEEK_SET);
        for (i = 0; i < 512; i++)
            dir[i] = fgetc(floppy);

        // Load the map
        char map[512];
        fseek(floppy, 512 * 256, SEEK_SET);
        for (i = 0; i < 512; i++)
            map[i] = fgetc(floppy);

        // Iterate through the directory entries
        for (i = 0; i < 512; i = i + 16) {
            // Find the first available sector
            if (freeDirectory == -1 && dir[i] == 0) {
                freeDirectory = i;
                continue;
            }

            // Extract the file name from the directory entry
            char nameOfFile[8] = "";
            for (j = 0; j < 8; j++) {
                if (dir[i + j] != 0) {
                    nameOfFile[j] = dir[i + j];
                }
            }

            // Determine the file extension based on the directory entry
            char extension;
            if (toupper(dir[i + 8]) == 'T') {
                extension = 't';
            } else {
                if (toupper(dir[i + 8]) == 'X') {
                    extension = 'x';
                }
            }

            // Check if the file already exists
            if (strcmp(nameOfFile, name) == 0) {
                found = 1;
                if (extension == 't') {
                    isText = 1;
                    break;
                }
                break;
            }
        }

        // If the file exists, print a message
        if (found) {
            if (isText) {
                printf("File currently exists\n"); // Duplicate file
            }
        } else { // File doesn't exist
            // Find an open spot in the map
            for (i = 0; i < 16; i++) {
                if (freeMap != -1) {
                    break;
                }
                for (j = 0; j < 16; j++) {
                    if (freeMap != -1) {
                        break;
                    }
                    // Located an open map spot
                    if (map[16 * i * j] == 0) {
                        map[16 * i * j] = 0xff;
                        freeMap = 16 * i * j;
                    }
                }
            }

            // If no open map spot is found, print an error message
            if (freeMap == -1) {
                printf("Not enough disk space.\n");
            } else { // Write file information to the directory
                for (i = 0; i < strlen(name); i++) {
                    dir[freeDirectory + i] = name[i];
                }
                for (i = strlen(name); i < 8; i++) {
                    dir[freeDirectory + i] = 0;
                }
                dir[freeDirectory + 8] = 't'; // The text file
                dir[freeDirectory + 9] = freeMap; // The offset
                dir[freeDirectory + 10] = 1; // 1-sector is the size of file
            }

            // Print a prompt to enter text
            printf("Enter text %s\n", name);
            char text[512];
            fgets(text, sizeof(text), stdin);

            // Write the contents of the text to the floppy disk
            fseek(floppy, 512 * freeMap, SEEK_SET);
            for (i = 0; i < 512; i++)
                fputc(text[i], floppy);

            // Write the map and directory back to the floppy image
            fseek(floppy, 512 * 256, SEEK_SET);
            for (i = 0; i < 512; i++)
                fputc(map[i], floppy);

            fseek(floppy, 512 * 257, SEEK_SET);
            for (i = 0; i < 512; i++)
                fputc(dir[i], floppy);
        }
    } else {
        printf("No filename has been given");
        return;
    }
}

// Function to print the file
void printTextFile(FILE* floppy, char name[MAX_NAME_LENGTH]) {
    // Check if a filename has been provided
    if (name[0] != -1) {
        printf("Command P\n");

        // Load the directory from sector 257
        char dir[512];
        int i, j, secStart, secSize; // Variables for sector start and sector size

        bool found = 0;
        bool readable = 0;

        fseek(floppy, 512 * 257, SEEK_SET);
        for (i = 0; i < 512; i++)
            dir[i] = fgetc(floppy);

        // Iterate through the directory entries
        for (i = 0; i < 512; i = i + 16) {
            // Skip empty directory entries
            if (dir[i] == 0) {
                continue;
            }

            // Extract the filename from the directory entry
            char nameOfFile[8] = "";
            for (j = 0; j < 8; j++) {
                if (dir[i + j] != 0) {
                    nameOfFile[j] = dir[i + j];
                }
            }

            char extension;
            // Determine the file extension based on the directory entry
            if (toupper(dir[i + 8]) == 'T') {
                extension = 't';
            } else {
                if (toupper(dir[i + 8]) == 'X') {
                    extension = 'x';
                }
            }

            // Check if the current directory entry corresponds to the provided filename
            if (strcmp(nameOfFile, name) == 0) {
                found = 1;
                secStart = dir[i + 9];
                secSize = dir[i + 10];

                // Check if the file is of type 'x' (not readable)
                if (extension == 'x') {
                    printf("%s is unable to be read.\n", nameOfFile);
                    break;
                }

                readable = 1;
                break;
            }
        }

        // Process the file if found
        if (found) {
            if (readable) {
                char contentOfFile[12288];

                // Read and print the content of the file
                fseek(floppy, 512 * secStart, SEEK_SET);
                for (i = 0; i < 12288; i++) {
                    contentOfFile[i] = fgetc(floppy);

                    // Break if the end of the file is reached
                    if (contentOfFile[i] == '\0') {
                        break;
                    }

                    printf("%c", contentOfFile[i]);
                }
            }
        } else {
            // Print a message if the file is not found
            printf("File has not been located\n");
        }
    } else {
        // Print a message if no filename has been given
        printf("No filename has been given");
        return;
    }
}

