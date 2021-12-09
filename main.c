#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#define BUFF_SIZE 2048
#define WR_END 1
#define RD_END 0

void print_error(int err, char *msg) {
    if (err == -1) {
        // If Error Print Error Message and Errno
        printf("Error: %s\n", msg);
        printf("%s\n", strerror(errno));
    }
}

int main() {
    // Constantly Runs
    while (1) {
        // Creating Pipe Array
        // Pipes[0]: Parent -> Child
        // Pipes[1]: Child -> Parent
        int pipes[2][2];

        // Attempting To Create Parent To Child Pipe
        int err1 = pipe(pipes[0]);
        if (err1 == -1) {
            print_error(-1, "Unable To Create Parent To Child Pipe");
            return -1;
        }

        // Attempting To Create Child To Parent Pipe
        int err2 = pipe(pipes[1]);
        if (err2 == -1) {
            print_error(-1, "Unable To Create Child To Parent Pipe");
            return -1;
        }

        // Forking
        int frk = fork();

        if (frk == 0) { // Child
            // Attempting To Close Write End For Parent To Child Pipe
            int err3 = close(pipes[0][WR_END]);
            if (err3 == -1) {
                print_error(-1, "Unable To Close Write End Of Pipe 0 In Child Process");
                exit(-1);
            }

            // Attempting To Close Read End For Child To Parent Pipe
            int err4 = close(pipes[1][RD_END]);
            if (err4 == -1) {
                print_error(-1, "Unable To Close Read End Of Pipe 1 In Child Process");
                exit(-1);
            }

            // Reading From Parent To Child Pipe
            char *buff2 = calloc(BUFF_SIZE, sizeof(char));
            int err5 = read(pipes[0][RD_END], buff2, BUFF_SIZE);
            if (err5 == -1) {
                print_error(-1, "Unable To Read From Parent Process In Child Process");
                exit(-1);
            }

            // String Manipulation (Upper Case)
            int i;
            for (i = 0; i < BUFF_SIZE; i++) {
                if (buff2[i] == '\0') break;
                buff2[i] = toupper(buff2[i]);
            }

            // Writing To Child To Parent Pipe
            int err6 = write(pipes[1][WR_END], buff2, BUFF_SIZE);
            if (err6 == -1) {
                print_error(-1, "Unable To Write To Parent Process In Child Process");
                exit(-1);
            }

            // Exit Normally
            free(buff2);
            exit(0);
        } else if (frk == -1) { // Error
            print_error(-1, "Unable To Fork Child Process");
            continue;
        } else { // Parent
            // Attempting To Close Read End For Parent To Child Pipe
            int err3 = close(pipes[0][RD_END]);
            if (err3 == -1) {
                print_error(-1, "Unable To Close Read End Of Pipe 0 In Parent Process");
                continue;
            }

            // Attempting To Close Write End For Child To Parent Pipe
            int err4 = close(pipes[1][WR_END]);
            if (err4 == -1) {
                print_error(-1, "Unable To Close Write End Of Pipe 1 In Parent Process");
                continue;
            }

            // Asking For User Input
            printf("Message To Child: ");
            char *buff = calloc(BUFF_SIZE, sizeof(char));
            fgets(buff, BUFF_SIZE, stdin);
            *strchr(buff, '\n') = '\0';

            // Quit Or Exit Command
            if (strcmp(buff, "exit") == 0 || strcmp(buff, "quit") == 0) break;

            // Writing To Parent To Child Pipe
            int err5 = write(pipes[0][WR_END], buff, BUFF_SIZE);
            if (err5 == -1) {
                print_error(-1, "Unable To Write To Child Process In Parent Process");
                continue;
            }

            // Waiting For Child Process To End
            int status;
            int err6 = waitpid(frk, &status, 0);
            if (err6 == -1) {
                print_error(-1, "Unable To Wait For Child Process");
                continue;
            }
            if (WEXITSTATUS(status) == -1) {
                print_error(01, "Unable To Run Child Process Normally");
                continue;
            }

            // Reading From Child To Parent Pipe
            char *buff2 = calloc(BUFF_SIZE, sizeof(char));
            int err7 = read(pipes[1][RD_END], buff2, BUFF_SIZE);
            if (err7 == -1) {
                print_error(-1, "Unable To Read From Child Process In Parent Process");
                continue;
            }
            printf("Message From Child: %s\n", buff2);

            // Closing Leftover Write File
            int err8 = close(pipes[0][WR_END]);
            if (err8 == -1) {
                print_error(-1, "Unable To Close Read End Of Pipe 0 In Parent Process");
                continue;
            }

            // Closing Leftover Read File
            int err9 = close(pipes[1][RD_END]);
            if (err9 == -1) {
                print_error(-1, "Unable To Close Write End Of Pipe 1 In Parent Process");
                continue;
            }

            // Continuing Normally
            free(buff);
            free(buff2);
        }

    }

    // Exiting Function
    return 0;
}
