#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

/*
This function prepares the shell for command execution

Returns:
    0 if should continue and execute, 1 otherwise
*/
int prepare(void){
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
        waitpid(pid, &result, 0);
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
This function executes the command given by the arglist words-array

Params: 
    count: the number of words in the command ( Null is last, arr[count])
    arglist: the array of words contained in the command

Returns: 
    1 if no error occured
    0 if error occured

*/
int process_arglist(int count, char **arglist){
    
    int i, command_flag, background_flag, pipe_flag, input_flag, output_flag = 0;
    for(i=0; i < count; i++){
        if(strcmp(arglist[i],"&") == 0){
            background_flag = 1;
        }
        else if (strcmp(arglist[i],"|") == 0){
            pipe_flag = 1;
        }
        else if (strcmp(arglist[i],"<") == 0){
            input_flag = 1;
        }
        else if (strcmp(arglist[i],">>") == 0){
            output_flag = 1;
        }
    }
    if(!(background_flag && pipe_flag && input_flag && output_flag)){
        command_flag = 1;
    }
    
    if(command_flag){
        return execute_command(arglist);
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