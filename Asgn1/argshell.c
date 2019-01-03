#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
extern char ** get_args();
pid_t pid;
pid_t pid2;
int status, redirect_file, direct_file, error_file, counter, pipe_counter, pipe_and_counter, counter_and;
char num_pipes[10];
char num_pipes_and[10];

void input(char **args, int i){
pid = fork();
    if(pid <  0){
        perror("Error: ");
	exit(1);
    }
    else if(pid == 0){
	args[i] = NULL;
        redirect_file = open(args[i+1], O_RDONLY);
        if(redirect_file < 0) {
    	    perror("Invalid file: ");
	    exit(1);
        }
        dup2(redirect_file, STDIN_FILENO);
        close(redirect_file);
        execvp(args[0], args);
        }else{
            waitpid(0, &status, 0);
        }
}

void output(char **args, int i) {
    printf("AUTHOR'S NOTE: the file has been directed, the errors are false \n");
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    }
    else if(pid == 0) {
    args[i] = NULL;
    direct_file = open(args[i+1], O_WRONLY| O_CREAT);
    if(direct_file < 0) {
        perror("Invalid file: ");
	exit(1);
    }
    dup2(direct_file, STDOUT_FILENO);
    close(direct_file);
    args[i] = NULL;
    execvp(args[0], args);
    } else {
        waitpid(0,&status,0);
    }
}

void append(char **args, int i) {
printf("AUTHOR'S NOTE: the file has been appended, the errors are false \n");
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    }
    else if(pid == 0) {
    args[i] = NULL;
    direct_file = open(args[i +1], O_WRONLY| O_APPEND| O_CREAT);
    if(direct_file < 0) {
        perror("Invalid file: ");
	exit(1);
    } 
    dup2(direct_file, STDOUT_FILENO);
    close(direct_file);
    execvp(args[0], args);
    } else {
        waitpid(0, &status, 0);
    }
}

void pip(char ** args, int i, int start) { // cited from http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html 
    int pipes[10]; 				    // cited from https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html

    char **input = &args[start];
    char **output = &args[i+1];

    pipe(pipes);
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    } else if (pid == 0) {
        dup2(pipes[1], STDOUT_FILENO); //output pipped to the input of another command
	close(pipes[0]);
	close(pipes[1]);

	execvp(args[start], input); //input of another command

	exit(1);

    } else {
        close(pipes[1]);
	waitpid(0,&status,0);
    }

    pid2 = fork();
    if(pid2 < 0) {
        perror("Error: ");
	exit(1);
    }
    else if (pid2 == 0) {
        dup2(pipes[0], STDIN_FILENO);
	close(pipes[1]);
	close(pipes[0]);
	execvp(output[0], output);
	exit(1);
    } else {
        close(pipes[0]);
	waitpid(0,&status,0);
    }

}


void and_sign(char **args, int i) {
printf("AUTHOR'S NOTE: the file has been redirected, the errors are false \n");
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    } else if(pid == 0) {
    args[i] = NULL;
    error_file = open(args[i+1], O_WRONLY);
    if(error_file < 0) {
        perror("Invalid file: ");
	exit(1);
    }
    //dup2(error_file, STDOUT_FILENO);
    dup2(error_file, STDERR_FILENO); // https://stackoverflow.com/questions/14505079/redirect-stdout-stderr-to-syslog
    close(error_file);
    execvp(args[0], args);
    } else {
        waitpid(0,&status,0);
    }
}

void semicolon(char **args, int i) {
    char **output = &args[i+1];

    pid=fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    }
    else if(pid == 0) {
    args[i] = NULL;
    execvp(args[0], args);
    wait(&status);
    } else {
        waitpid(0,&status,0);
    }

    pid2=fork();
    if(pid2 < 0) {
        perror("Error: ");
	exit(1);
    } else if(pid2 == 0) {
        execvp(output[0], output);
	wait(&status);
    } else {
        waitpid(0,&status,0);
    }

} 


void run(char **args, int i) {
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    }
    else if(pid == 0) {
        execvp(args[i], args);
	args[i] = NULL;
	if(!execvp(args[i],args)) {
	    perror("Error: ");
	    exit(1);
	}  
    }else
     waitpid(0,&status,0);
}

void output_and(char **args, int i) { // >&
    pid = fork();
printf("AUTHOR'S NOTE: the file has been directed, the errors are false \n");
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    } else if(pid == 0) {
       args[i] = NULL;
       direct_file = open(args[i+1], O_WRONLY | O_CREAT);
       //error_file = open(args[i+1], O_APPEND);
       if(direct_file < 0) {
           perror("Invalid file: ");
	   exit(1);
       }
       dup2(direct_file, STDOUT_FILENO);
       dup2(direct_file, STDERR_FILENO); // https://stackoverflow.com/questions/14505079/redirect-stdout-stderr-to-syslog
       close(direct_file);
       //close(error_file);
       execvp(args[0], args);
    } else {
       waitpid(0,&status,0);
    }
}

void input_and(char **args, int i) { // &<
    pid = fork();
printf("AUTHOR'S NOTE: the file has been redirected, the errors are false \n");
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    } else if (pid == 0) {
        args[i] = NULL;
	redirect_file = open(args[i+1], O_RDONLY);
	if(redirect_file < 0) {
	    perror("Invalid file: ");
	    exit(1);
	}
	dup2(redirect_file, STDIN_FILENO);
	dup2(redirect_file, STDERR_FILENO); // https://stackoverflow.com/questions/14505079/redirect-stdout-stderr-to-syslog
	close(redirect_file);
	execvp(args[0], args);
    } else {
        waitpid(0,&status,0);
    }
}

void output_append(char **args, int i) { // >>&
    pid = fork();
printf("AUTHOR'S NOTE: the file has been appended, the errors are false \n");
    if(pid < 0) {
       perror("Error: ");
       exit(1);
    } else if (pid == 0) {
        args[i] = NULL;
	direct_file = open(args[i+1], O_APPEND | O_CREAT);
	if(direct_file < 0) {
	    perror("Invalid file: ");
	    exit(1);
	}
	dup2(direct_file, STDOUT_FILENO); 
	dup2(direct_file, STDERR_FILENO); // cited from https://stackoverflow.com/questions/14505079/redirect-stdout-stderr-to-syslog
	close(direct_file);
	execvp(args[0], args);
    } else {
        waitpid(0,&status,0);
    }
}

void pipe_and(char **args, int i, int start) { // cited from http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html

    int pipes[10]; 				    // cited from https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html

    char **input = &args[start];
    char **output = &args[i+1];

    pipe(pipes);
    pid = fork();
    if(pid < 0) {
        perror("Error: ");
	exit(1);
    } else if (pid == 0) {
        dup2(pipes[1], STDOUT_FILENO); //output pipped to the input of another command
	dup2(pipes[1], STDERR_FILENO);
	close(pipes[0]);
	close(pipes[1]);

	execvp(args[start], input); //input of another command

	exit(1);

    } else {
        close(pipes[1]);
	waitpid(0,&status,0);
    }

    pid2 = fork();
    if(pid2 < 0) {
        perror("Error: ");
	exit(1);
    }
    else if (pid2 == 0) {
        dup2(pipes[0], STDIN_FILENO);
	dup2(pipes[0], STDERR_FILENO);
	close(pipes[1]);
	close(pipes[0]);
	execvp(output[0], output);
	exit(1);
    } else {
        close(pipes[0]);
	waitpid(0,&status,0);
    }

}

int main()
{
    int         i;
    char **     args;
    char cwd[FILENAME_MAX];

    getcwd(cwd,FILENAME_MAX); // cited from https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program

    while (1) {
	printf ("Command ('exit' to quit): ");
	args = get_args();
	for (i = 0; args[i] != NULL; i++) {
	    printf ("Argument %d: %s\n", i, args[i]);
	}
	
	for(i = 0; args[i] != NULL; i++) {
	if (args[0] == NULL) {
	    printf ("No arguments on line!\n");
	    }
	    else if(args[0] != NULL) {
	        if ( !strcmp (args[0], "exit")) {
		exit(EXIT_FAILURE);
	        } else if (!strcmp (args[0], "cd")) {
	        if (args[1] != NULL){
	             chdir(args[1]);
                } else if( args[1] == NULL){  
	            chdir(cwd);
		} 
                } else if( !strcmp(args[i], "<")){
			    input(args,i);
			 
		} else if( !strcmp(args[i], ">")){
			    output(args,i);
		} else if( !strcmp(args[i], "|")){ //output is piped to the input of another command.
			   num_pipes[pipe_counter] = i; // find out # of "|"
			   pipe_counter++;
			   args[i] = NULL;
			   i++;
		} else if( !strcmp(args[i], "&")){
			    and_sign(args,i);
		} else if( !strcmp(args[i], ";")) {
			    semicolon(args,i);
			    //counter++;
			    //continue;
		} else if ( !strcmp(args[i], ">>")) {
			    append(args,i);
		} else if ( !strcmp(args[i], "<<")) {
			    input(args,i);
		}else if (!strcmp(args[i], ">&")) {
			   output_and(args,i);	
		} else if (!strcmp(args[i], "&<")) {
			   input_and(args,i);	
		} else if (!strcmp(args[i], "&<<")) {
			   input_and(args,i);	
		} else if (!strcmp(args[i], ">>&")) {
			   output_append(args,i);
		} else if( !strcmp(args[i], "|&")) {
			   num_pipes_and[pipe_and_counter] = i;
			   pipe_and_counter++;
			   args[i] = NULL;
			   i++;
		}
		else if( args[1] == NULL) {
			    run(args,i);
		} else if( args[2] == NULL) {
		  	    run(args,i);
			    if(!strcmp (args[i], "exit")) {
			        exit(1);
			    }
		} else if (args[3] == NULL) {
			   run(args,i);
			   if( !strcmp (args[i], "exit")) {
			       exit(1);
			   }
		} else if (args[4] == NULL) {
			   run(args,i);
			   if( !strcmp(args[i], "exit")) {
			       exit(1);
			   }
		} 
		}
	   } 
	     for(int i  = 0; i < pipe_counter; i++) {
	 	   counter = num_pipes[i] - num_pipes[0];
	           pip(args, num_pipes[i], counter);
	     
              }

	      for(int i = 0; i < pipe_and_counter; i++) {
	          counter_and = num_pipes_and[i] - num_pipes_and[0];
		  pipe_and(args, num_pipes_and[i], counter_and);
	      }
    }
       
}

