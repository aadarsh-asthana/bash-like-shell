
#define DELIMITERS " \t\r\n\a"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>




/*int builtin_cd(char **args){

    int error; // For storing the return value of chdir, will be -1 if there was an error.

    if(args[1] == NULL) // If no arg given, chdir to HOME.
    {
        char *home = getenv("HOME");
        if(home == NULL) // If there is no home, chdir to root.
        {
            error = chdir("/");
        }
        else{
            error = chdir(home);
        }

        if(error == -1){
            fprintf(stderr, "Unknown error with cd.\n");
        }
    }
    else // If an arg is given, chdir to arg.
    {
        error = chdir(args[1]);
        if(error == -1){
            fprintf(stderr, "Directory %s not found.\n", args[1]);
        }
    }
    findDir();

}
*/
int shellExecute(char **args){

        pid_t pid;
        int status;
	
	
        pid = fork();

        if (pid == 0){
                if (execvp(args[0], args) == -1){
                        perror("shell");
                }
                exit(EXIT_FAILURE);
        } else if (pid < 0) {
                perror("shell");
        } else {
                wait(NULL);
        }

        return 1;

}
		
char *getLine(void){

        char *line = NULL;
  	ssize_t bufsize = 0;
        if (getline(&line, &bufsize, stdin) == -1) {
    	if (feof(stdin)) {
      	exit(EXIT_SUCCESS);
    	} else  {
      		perror("lsh: getline\n");
      		exit(EXIT_FAILURE);
   	 }
  }
	
        return line;
}



char **splitLine(char *line){
  	 size_t buffsize = 32;
        char **token_holder;
        char *token;

        token_holder = malloc(buffsize * sizeof(char *));
        int i = 0;
        token = strtok(line, DELIMITERS);
        while(token != NULL){
                token_holder[i] = token;
                i++;

                if (i >= buffsize){
                        buffsize += 32;
                        token_holder = realloc(token_holder, buffsize * sizeof(char *));
                        if (!token_holder) {
                                exit(EXIT_FAILURE);
                        }
                }


                token = strtok(NULL, DELIMITERS);
        }

        token_holder[i] = NULL;
        return token_holder;

}



void sigHandler(int sig, siginfo_t *siginfo, void *ucontext){
	if(sig == SIGQUIT){
		printf("Do you really want to exit? (y/n) :");
		char x = getchar();
		if ( x == 'y') exit(EXIT_SUCCESS);
		if (x == 'n' ) main(0, NULL);
	}
	

}
int parsePipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0;  
    else { 
        return 1; 
    } 
} 
void parseSpace(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < 1000; i++) { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
} // 

void shellExecutePiped(char **parsed, char**parsedpipe){
    
    int pfd[2];  
    pid_t p1, p2; 
  
    if (pipe(pfd) < 0) { 
        printf("Pipe error\n"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("Fork Error\n"); 
        return; 
    } 
  
    if (p1 == 0) { 
        close(pfd[0]); 
        dup2(pfd[1], STDOUT_FILENO); 
        close(pfd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("Error\n"); 
            exit(0); 
        } 
    } else {  
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("Fork error\n"); 
            return; 
        } 
  
         
         
        if (p2 == 0) { 
            close(pfd[1]); 
            dup2(pfd[0], STDIN_FILENO); 
            close(pfd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("Error\n"); 
                exit(0); 
            } 
        } else { 
             
            close(pfd[1]);
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 	
	
int parse2Pipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "||"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; 
    else { 
        return 1; 
    } 
} 	
void shellExecute2Piped(char **parsed, char ***parsedpipe){
    int pipefd[2];  
    pid_t p1, p2, p3; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe error"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nError in fork"); 
        return; 
    } 
  
    if (p1 == 0) {  
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nError "); 
            exit(0); 
        } 
    } else { 
         
        p2 = fork(); 
  
        if (p2 < 0) { 
            printf("\nError in fork"); 
            return; 
        } 
  
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe[0]) < 0) { 
                printf("\nError "); 
                exit(0); 
            } 
        } else { 
             
            p3 = fork();
            
            if(p3 == 0){
            	close(pipefd[1]);
            	dup2(pipefd[0], STDIN_FILENO); 
            	close(pipefd[0]); 
            	if (execvp(parsedpipe[0], parsedpipe[1]) < 0) { 
                	printf("\nError "); 
                	exit(0); 
            	} 
            }else{
            	close(pipefd[1]);
            	wait(NULL); 
            	wait(NULL); 
        } 
    } 
} 
}	

char **splitComma(char *line){
	size_t buffsize = 32;
	char **token_holder;
        char *token;

        token_holder = malloc(buffsize * sizeof(char *));
        int i = 0;
        token = strtok(line, ",");
        while(token != NULL){
                token_holder[i] = token;
                i++;

                if (i >= buffsize){
                        buffsize += 32;
                        token_holder = realloc(token_holder, buffsize * sizeof(char *));
                        if (!token_holder) {
                                exit(EXIT_FAILURE);
                        }
                }


                token = strtok(NULL, DELIMITERS);
        }

        token_holder[i] = NULL;
        return token_holder;

}

 
int main(int argc, char **argv){
	struct sigaction siga;
	siga.sa_sigaction = sigHandler;
	siga.sa_flags |= SA_SIGINFO;
	sigaction(SIGQUIT,&siga,NULL);
	char *line;
	int status;
	char **parsedpipe;		 
	char *strpiped[2];
	int piped =0;
	char **parsed;
	int doublepiped = 0;
	char *str2piped[2];
	char **parsed2pipe[2];
	char *parsed2pipea;
	char *parsed2pipeb;
	char **parsedcomma;
        for(;;){
		printf("PID :%d ", getpid());
		printf("$");
		int flag;
		
		line = getLine();
		piped = parsePipe(line,strpiped);
  			if (piped) { 
        			parsed = splitLine(strpiped[0]) ; 
        			parsedpipe = splitLine(strpiped[1]); 
    			} 
		doublepiped = parse2Pipe(line, str2piped);
		
		if (doublepiped) {
			
			parsed = splitLine(str2piped[0]);
			
			parsedcomma = splitComma(str2piped[1]);
			parsed2pipe[0] = splitLine(parsedcomma[0]);
			parsed2pipe[1] = splitLine(parsedcomma[1]);
											
		} 
  		if (!piped) parsed = splitLine(line);
    		
    		if (piped) shellExecutePiped(parsed, parsedpipe);
    		
    		else{
    		
    		if (doublepiped){
    			shellExecute2Piped(parsed, parsed2pipe);
    		}	      
		
		else status = shellExecute(parsed);
		}
        
	}
}
