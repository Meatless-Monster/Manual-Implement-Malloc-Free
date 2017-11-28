/**
 * CS 240 Shell Spells
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "joblist.h"

#define NAME "shell"
#define PROMPT "-> "
#define HIST_FILE ".shell_history"

/**
 * If command is built in, do it directly.  (Parts 1, 2, 3)
 *
 * Arguments:
 * jobs    - job list (Part 2-3)
 *           During Part 1, pass NULL for jobs when calling.
 * command - command to run
 *
 * Return:
 * 1 if the command is built in
 * 0 if the command is not built in
 */
int shell_builtin(JobList* jobs, char** command) {
  return 0;
}


/**
 * Place the process with the given pid in the foreground and wait for
 * it to terminate (Part 1) or stop (Part 3). Do not use this
 * functions for other (non-blocking) types of waiting.
 *
 * Exit in error if an error occurs while waiting (Part 1).
 *
 * Arguments:
 * pid - wait for the process with this process ID.
 *
 * Return:
 * 0 if process pid has terminated  (Part 1)
 * 1 if process pid has stopped     (Part 3)
 */
int shell_wait_fg(pid_t pid) {
  return -1;
}

/**
 * Fork and exec the requested program in the foreground (Part 1)
 * or background (Part 2).
 *
 * Use shell_wait_fg to do all *foreground* waiting.
 *
 * Exit in error if an error occurs while forking (Part 1).
 *
 * Arguments:
 * jobs       - job list for saving or deleting jobs (Part 2-3)
 *              During Part 1, pass NULL for jobs when calling.
 * command    - command array of executable and arguments
 * foreground - indicates foregounrd (1) vs. background (0)
 *
 * Return:
 * 0 if the foreground process terminated    (Part 1)
 * 0 if a background process was forked      (Part 2)
 * 1 if the foreground process stopped       (Part 3)
 */
int shell_run_job(JobList* jobs, char** command, int foreground) {
  return -1;
}


/**
 * Main shell loop: read, parse, execute.
 *
 * Arguments:
 * argc - number of command-line arguments for the shell.
 * argv - array of command-line arguments for the shell.
 *
 * Return:
 * exit status - 0 for normal, non-zero for error.
 */
int main(int argc, char** argv) {
  // Load history if available.
  using_history();
  read_history(HIST_FILE);

  // Until ^D (EOF), read command line.
  char* line = NULL;
  while ((line = readline(PROMPT))) {
    // Add line to history.
    add_history(line);
    int fg = -1;
    // Parse command line: this is Pointer Potions.
    // It allocates a command array.
    char** command = command_parse(line, &fg);
    // Free command line.
    free(line);

    // Replace me!  This just echos the command.
    if (command){
      command_print(command);
      printf("\n");
      // Currently, we free every command array immediately upon
      // completion of the loop body.  WHEN YOU IMPLEMENT PART 2, YOU
      // MUST RETHINK THIS.
      command_free(command);
    }
  }
  // If ^D (EOF), do the same thing as for the exit command.
  // ...

  return 0;
}
