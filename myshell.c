#include <stdio.h>

/*
This function prepares the shell for command execution

Returns:
    0 if should continue and execute, 1 otherwise
*/
int prepare(void){
    return 0;
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