#include <iostream>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

using namespace ::std;

int main(int argc, char **argv)
{
    if (argc != 3) // Checking for three command line arguments
    {
        cout << "Error: Please enter a input .txt file & a destination .txt file.\n";
        exit(1);
    }

    // Opening source file and target file
    int inputFile = open(argv[1], 0);
    // Opened with permission to read/write, create file, and truncate file
    int targetFile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);

    if (inputFile == -1 || targetFile == -1) // Checking if fails to open source/target files
    {
        cout << "Error: File can not be opend\n";
        exit(1);
    }

    // Creating Pipe
    int mypipe[2];

    if (pipe(mypipe) == -1) // Checking if pipe failed
    {
        cout << "Error: Failed to create pipe.\n";
        exit(1);
    }

    // Creating fork
    pid_t pid = fork();
    char readBuffer[1024]; // Buffer for reading data into/out of pipe

    if (pid < 0) // Checking if fork() failed
    {
        cout << "Error: Failed to fork\n";
        exit(1);
    }
    else if (pid == 0)
    {
        /* Child Process*/
        close(mypipe[1]);
        while (read(mypipe[0], readBuffer, sizeof(readBuffer)) > 0) // Reading through lines of pipe
        {
            write(targetFile, readBuffer, strlen(readBuffer));
        }
        close(mypipe[0]);
        close(targetFile);
    }
    else
    {
        /* Parent Process*/
        close(mypipe[0]);
        while (read(inputFile, readBuffer, sizeof(readBuffer)) > 0) // Reading through lines of file
        {
            write(mypipe[1], readBuffer, sizeof(readBuffer));
            memset(readBuffer, 0, 1024); // Resetting readBuffer to '0' or empty for next line
        }
        close(mypipe[1]);
        close(inputFile);
        wait(NULL); // waiting for child process to terminate
        cout << "File successfully copied from input.txt to copy.txt.\n";
    }
}