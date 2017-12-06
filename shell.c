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
  //check through if statements to see if command is built in
  if (strcmp(command[0],"exit" )==0){
    command_free(command);
    exit(0); 
  }
  else if (strcmp(command[0],"help" ) == 0){
    //for help, print all built in commands
    printf("exit\nhelp\ncd\njobs\n");
  }
  else if ((strcmp(command[0],"cd" ) == 0)){
    //for cd, go to the path in the second argument if it exists
    //if not, go to where "HOME" is 
    if (command[1] != NULL){
      chdir(command[1]);
    }
    else{
      chdir(getenv("HOME"));
    }
  }
  else if ((strcmp(command[0],"jobs" ) == 0)){
    //iterate through all current jobs 
    job_iter(jobs, job_print);
  }
  else {
    return 0;
  }
  //if command is built in, free the command array 
  command_free(command);
  return 1;
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
  int child_status;
  //if waitpid returns an error, print error and exit
  if (-1 == waitpid(pid, &child_status, 0)){
    perror("Error");
    exit(-1);
    return 1;
  }
  return 0;
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
  pid_t pid = fork();
  JobStatus status = 0;
  //in the child, replace shell with command and print if there is an error
  if (pid == 0){
    execvp(command[0], command);
    perror("Error");
    command_free(command);
    exit(-1); 
  }
  //in the parent, create the job and either print its status or wait for child if in the foreground then delete
  else{
    Job* job = job_save(jobs, pid, command, status);
    if (foreground == 0){
      job_set_status(jobs, job, 3);
      job_print(jobs, job);
    }
    else{
      shell_wait_fg(pid);
      job_set_status(jobs, job, 2);
      job_delete(jobs, job);
    }
  }
  return 0;
}

//helper function to use in job_iter
//Checks to see if a job has terminated, if so it prints deletes the job and terminates the zombie process
void backJobHelp(JobList* jobs, Job* job){
  int status;
  //get the pid returned from waitpid
  int currpid = waitpid(job->pid, &status, WNOHANG);
  //if pid id the same and job pid and status is a valid pointer, print and delete job 
  if(WIFEXITED(status) && currpid==job->pid){
    job_set_status(jobs, job, 4);
    job_print(jobs, job);
    job_delete(jobs, job);
  }
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

  //JOBLIST, 6.1
  JobList* mahJobs = joblist_create();
  
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
    if(command[0] != NULL){
      //check to see if jobs are built in 
      int built = shell_builtin(mahJobs, command);
      //if not, preform job in sheel_run_job 
      if (built == 0){
	shell_run_job(mahJobs, command, fg);
      }
      //iterate through all saved jobs -> this will print and delete and terminated jobs
      job_iter(mahJobs, backJobHelp);
    }
  }
  
  // If ^D (EOF), do the same thing as for the exit command.
  // ...

  //free joblist 
  joblist_empty(mahJobs);
  joblist_free(mahJobs);

  return 0;
}

