#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BACKGROUND_PROCESSES 100
#define MAX_ARGS 100

typedef struct {
    pid_t pid;
    char command[256];
} BackgroundProcess;

BackgroundProcess bg_processes[MAX_BACKGROUND_PROCESSES];
int bg_process_count = 0;

// Function to add a background process
void add_background_process(pid_t pid, const char* command) {
    if (bg_process_count < MAX_BACKGROUND_PROCESSES) {
        bg_processes[bg_process_count].pid = pid;
        strncpy(bg_processes[bg_process_count].command, command, 255);
        bg_processes[bg_process_count].command[255] = '\0';
        bg_process_count++;  // increment count of background processes
    }
}

// Function to remove a completed background process
void remove_background_process(int index) {
    if (index < bg_process_count - 1) {
        memmove(&bg_processes[index], &bg_processes[index + 1],  // Shifts elements to fill the gap
                (bg_process_count - index - 1) * sizeof(BackgroundProcess));
    }
    bg_process_count--;  // decrease count of background processes
}

// Function to check background processes
int check_background_processes() {
    int running_processes = 0;
    int status;
    
    for (int i = 0; i < bg_process_count; i++) {
        // Check if the process finished without blocking
        pid_t result = waitpid(bg_processes[i].pid, &status, WNOHANG);  //  WNOHANG flag tells parent process not to wait
        if (result == 0) {  // Process is still running
            printf("Process %d (%s) is still running\n", 
                   bg_processes[i].pid, bg_processes[i].command);
            running_processes++;
        } else if (result > 0) {  // Process has completed
            printf("Process %d finished\n", bg_processes[i].pid);
            remove_background_process(i);
            i--;  // Adjust index since we removed an element
        }
    }
    return running_processes;
}

int main() {
    while (1) {
        // Check status of background processes
        check_background_processes();
        
        char userInput[256], host[256];

        // Getting the username
        const char *username = getenv("USER");
        if (username == NULL) {
            fprintf(stderr, "Error: Unable to retrieve username\n");
            exit(1);
        }

        if (gethostname(host, sizeof(host)) != 0) {
            fprintf(stderr, "Error: unable to retrieve hostname\n");
            exit(1);
        }

        printf("%s@%s> ", username, host);
        fflush(stdout);
        if (fgets(userInput, sizeof(userInput), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(1);
        }

        userInput[strcspn(userInput, "\n")] = '\0';

        // Parse command
        char* command = strtok(userInput, " ");
        if (command == NULL) continue;

        if (strcmp(command, "globalusage") == 0) {
            char *args[MAX_ARGS];
            int redirect = 0; // Flag to indicate redirection
            char *output_file = NULL; // Store the file name for redirection

            // Parse arguments for redirection
            args[0] = command;
            int arg_count = 1;
            char *token = strtok(NULL, " ");
            while (token != NULL) {
                // Handle output redirection
                if (strcmp(token, ">") == 0) {
                    // Found redirection operator
                    redirect = 1;
                    output_file = strtok(NULL, " "); // Get the file name
                    if (output_file == NULL) {
                        fprintf(stderr, "Error: No file specified for output redirection\n");
                        break;
                    }
                    break; // Stop processing further arguments
                } else {
                    args[arg_count++] = token;
                }
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL; // Null-terminate the arguments

            if (redirect && output_file == NULL) {
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error: Fork failed\n");
                continue;
            } else if (pid == 0) {
                // Child process
                if (redirect) {
                    // Open the file in append mode (create it if it doesn't exist)
                    FILE *file = fopen(output_file, "a");
                    if (file == NULL) {
                        fprintf(stderr, "Error: Could not open file %s for writing\n", output_file);
                        exit(1);
                    }
                    // Redirect stdout to the file
                    int file_fd = fileno(file);
                    dup2(file_fd, STDOUT_FILENO);
                    fclose(file); // Close the FILE* after duplicating
                }
                // Print global usage information
                fprintf(stdout, "IMCSH Version 1.618 created by Mario Palagyi, Nevin Joseph\n");
                exit(0); // Exit child process after writing
            } else {
                // Parent process waits for the child to finish
                int status;
                waitpid(pid, &status, 0);
                if (redirect) {
                    // Only display PID if redirection is used
                    printf("Process %d finished\n", pid);
                }
            }
            continue;
        }

        if (strcmp(command, "quit") == 0) {
            int running = check_background_processes();
            if (running > 0) {
                fprintf(stdout, "The above processes are running, are you sure you want to quit? [Y/n]\n");
                char choice[256];
                fgets(choice, sizeof(choice), stdin);
                choice[strcspn(choice, "\n")] = '\0';
                if (strcmp(choice, "Y") == 0 ||
                    strcmp(choice, "Yes") == 0 ||
                    strcmp(choice, "y") == 0 ||
                    strcmp(choice, "yes") == 0) {
                    fprintf(stdout, "Quitting...\n");
                    exit(0);
                }
            } else {
                //fprintf(stdout, "No background processes running. Quitting...\n");
                exit(0);
            }
            continue;
        }

    if (strcmp(command, "exec") == 0) {
        char *args[MAX_ARGS];
        int arg_count = 0;
        int background = 0;
        int redirect = 0; // Flag to indicate redirection
        char *output_file = NULL; // Store the file name for redirection
        char original_command[256] = "exec ";

        // Parse the command and arguments
        args[arg_count] = strtok(NULL, " ");
        if (args[arg_count] == NULL) {
            fprintf(stderr, "Error: No program specified for exec\n");
            continue;
        }
        strcat(original_command, args[arg_count]);

        while (args[arg_count] != NULL && arg_count < MAX_ARGS - 1) {
            arg_count++;
            args[arg_count] = strtok(NULL, " ");
            if (args[arg_count] != NULL) {
                if (strcmp(args[arg_count], ">") == 0) {
                    // Found redirection operator
                    redirect = 1;
                    args[arg_count] = NULL; // End the command arguments here
                    output_file = strtok(NULL, " "); // Get the file name
                    if (output_file == NULL) {
                        fprintf(stderr, "Error: No file specified for output redirection\n");
                        break;
                    }
                    break; // Stop processing further arguments
                } else if (strcmp(args[arg_count], "&") == 0) {
                    // Handle background process flag
                    background = 1;
                    args[arg_count] = NULL; // Remove & from arguments
                    break;
                } else {
                    strcat(original_command, " ");
                    strcat(original_command, args[arg_count]);
                }
            }
        }

    // If output redirection is specified, ensure a file is provided
    if (redirect && output_file == NULL) {
        continue;
    }

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: Fork failed\n");
        continue;
    } 
    else if (pid == 0) {
        // Child process
        if (redirect) {
            // Open the file in append mode (create it if it doesn't exist)
            FILE *file = fopen(output_file, "a");
            if (file == NULL) {
                fprintf(stderr, "Error: Could not open file %s for writing\n", output_file);
                exit(1);
            }
            // Redirect stdout to the file
            int file_fd = fileno(file);
            dup2(file_fd, STDOUT_FILENO);
            fclose(file); // Close the FILE* after duplicating
        }
        
        // execute command
        execvp(args[0], args);
        // If execvp returns, there was an error
        fprintf(stderr, "Error: Failed to execute %s\n", args[0]);
        exit(1);
        } 
    else {
        // Parent process
        if (background) {
            // Add to background processes and continue wihout waiting
            add_background_process(pid, original_command);
            printf("Started background process %d\n", pid);
        } 
        else {
            // Wait for foreground process to complete
            int status;
            waitpid(pid, &status, 0);
            if (redirect) {
                // Only display PID if redirection is used
                printf("Process %d finished\n", pid);
            } else {
                printf("Process %d finished\n", pid);
            }
        }
    }
    continue;
        }

    }

    return 0;
}