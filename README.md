# IMCSH (IMC - SHell), Implementation of an Alternative Linux Shell
*by: Márió Palágyi, Nevin Joseph @ IMC Krems*

**Overview of Contents**
- Introduction and Structure
- Libraries/Dependencies
- Execution Guide
- Description of Relevant Parts
- Interaction of Modules
- A guide on executing the project
- Summary
---

## 1. Introduction and Structure
To introduce the project and provide a presentation of the structure of this project, this chapter is worth taking a look at.
The assignment was to implement an Alternative Linux Shell, called IMCSH, meaning "IMC Shell". As we already know from Operating Systems class, the shell is an important part of the Operating System (OS) that communicates with the user either via graphical UI or a command line where we can type commands, in this case, we had to program the latter. Our shell can accept commands from the user from the standard input (stdin), it can execute the provided commands and print the result to the standard output (stdout).
_Note!_ It was fobidden to use the system() call so as not to invoke the standard Linux shell, as the main focus of this assignment is to create our own.

Executing the shell, the IMCSH opens for us, we can see the command prompt of the form `user@host>`, this is where we can type the 3 following commands and appropriate *modifiers*.

## 2. Libraries/Dependencies
The libraries that we imported and why we did for the IMCSH system are:
- stdio.h: functions for input/output (e.g., printf, scanf, fgets).
- stdlib.h: functions useful for memory management, program control (e.g., malloc, free, exit).
- unistd.h: POSIX operating system API functions (e.g., file handling, fork, exec).
- string.h: functions for strings and their manipulation (e.g., strlen, strcpy, strcmp).
- sys/types.h: necessary data types used in system calls (e.g., pid_t, size_t).
- sys/wait.h: necessary functions for process control and waiting (e.g., wait, waitpid).


## 3. Execution Guide
1. With the provided `Makefile` we can compile and link the program or clean the directory of the executable
2. Now we have the imcsh executable file, we type into our terminal (we used VSCode) `./icmcsh` to execute the alternative linux shell known as IMCSH
3. Now that we see our custom command prompt of the form `user@host>`, we can use our commands, which are
- exec
- globalusage or
- quit
4. This is how we can execute the commands, optionally with modifiers
- exec: `exec <command>` to the command part, we can insert any linux command that we know, like: ls, cd, pwd, cat, mkdir etc...
the above mentioned processes - mention
- exec + ">": `exec ls > output.txt` to redirect the output of a command into a file. If file exists, writes to the end of it, if not, file gets created.
- exec + "&": `exec sleep 30 &` this makes the process a background task and continues with the option to further execute commands.
- globalusage: `globalusage` outputs the version of IMCSH
- globalusage: `globalusage > output.txt` redirects the output of globalusage into a file. Creates it too if it doesn't exist yet.


## 4. Description of Relevant Parts
As mentioned, the 3 available commands are:
- **exec:** this is used for executing a program with its parameters, by creating a child process. After termination of the child process, the PID of the finished process is printed as output in a new line.
- **globalusage:** this is a command that shows details about the current version of IMCSH that is being executed. For example, in our case "IMCSH Version 1.618 created by Mario Palagyi, Nevin Joseph"
- **quit:** Is a command that, as the name suggests, provides the option to quit execution of IMCSH. If there are any running processes, a question gets prompted to the user if they really want to quit. "The above processes are running, are you sure you want to quit? [Y/n]". (we changed the wording of this question, it will be explained later, why) If the user wants to quit, they push "y"/"Y"/"yes"/"Yes" and the shell quits, with all running processes terminated. If they don't want to quit, they shall push "n", but any other key works fine (except the ones used for quitting...)

The 2 modifiers are:
- **& (Ampersand):** this modifier is used at the end of an exec command and it is used for telling IMCSH to execute the given command in the background and to continue execution without waiting for the command to finish. When the process ends, it instantly outputs the PID of the finished process in a new line.
- **">" (greater than):** When this command is used, it's used either after `exec + <command>` or `globalusage`. The way it works is by writing an appropriate output file after it, like this: `exec ls -l > output.txt` or `globalusage > output.txt`. This modifier redirects the output of the command to the given file and appends it to the end of it. If the file doesn't exist yet, it gets created. Then, the PID of the process after termination gets printed as the output to the console.

There is a struct for BackgroundProcess.
Necessary 3 functions:
- add_background_process for adding background processes
- remove_background_process for removing background processes
- check_background_processes for checking the state of the background processes, also prints output
There are 2 limit definitions, MAX_BACKGROUND_PROCESSES (100) for the maximum amont of running background processes and MAX_ARGS for the maximum amount of arguments for execution.
Everything else is included in the main program.


## 5. Interaction of Modules
The program is able to track background processes running with the struct, it stores process IDs and commands. It also has functions for adding and removing processes from the tracking system and finally it can check background running processes, which are most important for the quitting function.
We have 3 commands that have already been explained, they contain the main functionality and interaction with our shell.
When our IMCSH is in execution, it displays a prompt, where we can communicate with the OS, also output redirection to files is supported and basic command-line argument parsing.
In case of child process creation, it uses fork() for process forking and execvp() for command execution, managing both processes running in priority and those that run in the background. When processes finish, the program track their completion and does relevant cleanup.

In the main loop is where command processing and modifier handling happens: it continuously reads user input and passes parsed commands to appropriate handlers (like globalusage, quit, or exec). During execution the handler for "exec" checks if the program is supposed to be a primary or a background process, if background, then the part of the code which is background process manager (we call it that, but it's the 3 initial functions) must manage and track it. Primary processes, which do not run in the background are waited for completion, print statement arrives and they return control. I/O handling happens in the code where the prompt is programmed, that's where we can input our commands and output is handled in a way that errors get reported and also status messages are printed out.
Process Management → I/O Handling:

## 6. Summary and Takeaways
It was a really cool project that we could learn how to implement an alternative Linux shell. The customizability of the computer is exciting and it opened up new ways of how we thought about the OS in general. Initially, it seemed harder than expected, but continuous learning and YouTube tutorials with internet searches, used with our C programming experience gained from the classroom, at last we would say that this was an adequately difficult project. The end result is not perfect, but the from lessons gained it was a really worth it experience. We split the work to not exhaust ourselves and reiterated over our solutions, discussed them before implementing one's changes.
For grasping the theory part, we went over briefly the relevant class materials, and for the practical one, we used the provided C exercises and the ones we made for assignments before to our advantage.
Our sources include:
- YouTube: tutorials from BroCode and FreeCodeCamp
- Internet materials: StackOverflow and GeeksforGeeks
- ChatGPT and Claude for explaining concepts in C, finding bugs in the code and logic checking after the project was done

A note to improve:
sometimes, using Ampersand (&) at the end of commands messes the output up, but it doesn't affect the terminal in any way, it is still usable, we can type commands. In this instance, the `user@host` isn't aligned with our current input, but in a previous line, the current one is empty. Also, after typing in another command we only get the result "Process ### finished".
Since our function that checks for running processes works like that, we changed the output when the user tries to use the quit command from "The following processes are running, are you sure you want to quit? [Y/n]" to "The **above** processes are running, are you sure you want to quit? [Y/n]".

If we had mmore time, we would:
- figure out why the "&" modified processes don't get printed out as soon as they terminate
- trim down the code, as it contains repeated logic
Nice to have: extra decoration and functions for our IMCSH
