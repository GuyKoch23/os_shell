#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include<stdlib.h>

void printify(char *str){
    printf("%s\n", str);
}

/*
This function prepares the shell for command execution

Returns:
    0 if should continue and execute, 1 otherwise
*/
int prepare(void){
    signal(SIGINT, SIG_IGN); // Disables ctrl^c
    return 0;
}

/*

This function executes shell command, meaning case 1 of section 1.1

Params: 
    arglist: parsed command to array
Returns:
    the result of the execution: 
        1 for successful
        0 for unsuccessful

*/
int execute_command(char **arglist){
    pid_t pid;
    pid = fork();
    if(pid == 0){
        execvp(arglist[0], arglist);
		printf("execvp error: %s\n", strerror(errno));
		return 0;    
    }
    else if (pid > 0){
        int result;
        wait(&result);
        if(!WIFEXITED(result)){
            return 0;
        }
        return 1;
    }
    else{
        printf("fork error: %s\n", strerror(errno));
        return 0;
    }
}

/*
This function executes commands in the background

Params:
    arglist: parsed command to array
*/
void background_execute_command(char **arglist){
    pid_t pid;
    pid_t result;
    pid = fork();
    if(pid == 0){
        execvp(arglist[0], arglist);
		printf("execvp error: %s\n", strerror(errno));
    }
    else if (pid > 0){
        result = waitpid(pid, &result, WNOHANG);
    }
    else{
        printf("fork error: %s\n", strerror(errno));
    }
}

/*
This function executed the pipe command

Parans: 
    arglist: parsed command to array
Return:
    1 if successful
    0 else
*/
int execute_pipe(char **arglist, int pipe_loc){
    int pipe_arr[2];
    pid_t pid_1, pid_2;
    arglist[pipe_loc] = NULL; /* indicating the end of first one */
    if(pipe(pipe_arr) == -1){
        printf("pipe error: %s\n", strerror(errno));
        return 0;
    }
    pid_1= fork();
    if(pid_1 == -1){
        printf("fork error: %s\n", strerror(errno));
        return 0;
    }
    else if(pid_1 == 0){
        close(pipe_arr[0]);
        if(dup2(pipe_arr[1], STDOUT_FILENO) == -1){
            printf("dup error: %s\n", strerror(errno));
            return 0;
        }
        execvp(arglist[0], arglist);
        printf("execvp error: %s\n", strerror(errno));
        return 0;
    }

    pid_2 = fork();
    if(pid_2 == -1){
        printf("fork error: %s\n", strerror(errno));
        return 0;
    }
    else if(pid_2 == 0){
        close(pipe_arr[1]);
        if(dup2(pipe_arr[0], STDIN_FILENO) == -1){
            printf("dup error: %s\n", strerror(errno));
            return 0;
        }
        execvp(arglist[pipe_loc+1], &arglist[pipe_loc+1]);
        printf("execvp error: %s\n", strerror(errno));
        return 0;
    }
    close(pipe_arr[0]);
    close(pipe_arr[1]);
    waitpid(pid_1, NULL, 0);
    waitpid(pid_2, NULL, 0);
    return 1;
}

/*
This function executes the command given by the arglist words-array

Params: 
    count: the number of words in the command ( Null is last, arr[count])
    arglist: the array of words contained in the command

Returns: 
    1 if no error occured
    0 if error occured

*/
int process_arglist(int count, char **arglist){
    int i = 0, command_flag = 0, background_flag = 0, pipe_flag = 0, input_flag = 0, output_flag = 0, pipe_loc = 0;
    printify("start");
    for(i=0; i < count; i++){
        if(strcmp(arglist[i],"&") == 0){
            background_flag = 1;
        }
        else if (strcmp(arglist[i],"|") == 0){
            pipe_flag = 1;
            pipe_loc = i;
        }
        else if (strcmp(arglist[i],"<") == 0){
            input_flag = 1;
        }
        else if (strcmp(arglist[i],">>") == 0){
            output_flag = 1;
        }
    }
    if(!(background_flag || pipe_flag || input_flag || output_flag)){
        command_flag = 1;
    }
    
    if(command_flag){
        printify("command");
        return execute_command(arglist);
    }
    if(background_flag){
        printify("back");
        arglist[count-1] = NULL;
        background_execute_command(arglist);
        return 1;
    }
    if(pipe_flag){
        printify("pipe");
        return execute_pipe(arglist, pipe_loc);
    }
    return 1;
}

/*
This function handles all finalizations required for the app. 

Returns:
    0 if everything is finalized as expected, 1 otherwise. 
*/
int finalize(void){
    return 0;
}