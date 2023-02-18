
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/resource.h>
#include "command.h"




SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0; 
	_background = 0;
	_appendflag=0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_appendflag=0;
}

void
Command::print()
{

	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
	
}
void
Command::activate_background()
{
	_background=1;
}
void
Command::activate_append()
{
	_appendflag=1;
	
}

void
Command::execute()
{
	
	// Don't do anything if there are no simple commands
	int value=1;
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
	

	// Print contents of Command data structure
	
	print();
	

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec
	
	if(_numberOfSimpleCommands==1){
	
	int defaultin  = dup(0);
	int defaultout = dup(1);
	int defaulterr = dup(2);
	int outfd;
	int infd;
	int errfd;
	       if (_outFile != 0){
			if(_appendflag){
				 outfd=open(_outFile,O_CREAT | O_APPEND | O_WRONLY,0666);
				 
			}else{
				 outfd= creat(_outFile,0666);
			
			}
			
			if (outfd < 0){
				perror("error creating outfile");
				exit(2);
			}
			dup2(outfd,1);
			close(outfd);
			
		}
		if(_inputFile != 0){
			infd=open(_inputFile, O_RDONLY,0666);
			dup2(infd,0);
			close(infd);
			}
			
		if(_errFile != 0){
			errfd=open(_errFile,O_CREAT | O_APPEND | O_WRONLY,0666);
			dup2(errfd,2);
			close(errfd);
			
		
		}
		int pid = fork();
		if ( pid == -1 ) {
			perror( "error in forking\n");
			exit( 2 );
		}

		else if(pid == 0) {
			//Child
			execvp(_simpleCommands[0]->_arguments[ 0 ],_simpleCommands[0]->_arguments);
			perror( "error in child process");
			exit( 2 );
		}
		else if(pid>0){
			//parent
			dup2(defaultin,0);
			dup2(defaultout,1);
			dup2(defaulterr,2);
			
			
			
			close(defaultin);
			close(defaultout);
			close(defaulterr);
			
			if (!_background){
				waitpid(pid,0,0);
			}
		}
	
	
	}
	else{
		int num_pipes=_numberOfSimpleCommands-1;
		pid_t pid;
		int fdpipe[2*num_pipes];
		int j =0;

		for ( int i=0 ; i<num_pipes ;i++){
			if (pipe (fdpipe +i*2) < 0){
				perror("error in pipe");
				exit(2);
			}
		}
		for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
			pid = fork();
			if( pid ==0 ){
				if( i != (_numberOfSimpleCommands-1)){
					if (dup2(fdpipe[j+1],1) < 0){
						perror ("error in dup2");
						exit(2);
					}
				
				}
				if ( i != 0){
					if (dup2(fdpipe[j-2],0) < 0){
						perror ("error in dup2");
						exit(2);
					}
					
				}
				if ( _outFile != 0 && i ==_numberOfSimpleCommands-1 ){
					int outfd;
					if(_appendflag){
						 outfd=open(_outFile,O_CREAT | O_APPEND | O_WRONLY,0666);
				 
					}else{
						 outfd= creat(_outFile,0666);
			
					}
					if (outfd < 0){
						perror("error creating outfile");
						exit(2);
					}
					dup2(outfd,1);
					close(outfd);
				}
				for ( int i=0 ; i<2*num_pipes ;i++){
					close(fdpipe[i]);
				}
				execvp(_simpleCommands[i]->_arguments[ 0 ],_simpleCommands[i]->_arguments);
			}
			else if (pid < 0 ){
				perror ("error in child process");
				exit(2);
			}
			j+=2;
		}	
		for ( int i=0 ; i<2*num_pipes ;i++){
			close(fdpipe[i]);
		}
		for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
			wait (0);			
		}
		
	}
		
		
	
	/*else
	{
		int defaultin = dup( 0 ); 
		int defaultout = dup( 1 ); 
		int defaulterr = dup( 2 ); 
		int fdpipe[2];
		if ( pipe(fdpipe) == -1) {
			perror( "cat_grep: pipe");
			exit( 2 );
		}
		for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
			
		
			if( i == 0 ){
				
				dup2( defaultin, 0 );
				dup2( fdpipe[ 1 ], 1 );
				dup2( defaulterr, 2 );

				int pid = fork();
				if ( pid == -1 ) {
					perror( "cat_grep: fork\n");
					exit( 2 );
				}

				if (pid == 0) {
		
					close(fdpipe[0]);
					close(fdpipe[1]);
					close( defaultin );
					close( defaultout );
					close( defaulterr );

					execvp(_simpleCommands[i]->_arguments[ 0 ],_simpleCommands[i]->_arguments);

					perror( "cat_grep: exec cat");
					exit( 2 );
				}
					
				
			}
			else if(i == (_numberOfSimpleCommands-1)){
				dup2( fdpipe[0], 0);
				dup2( defaultout ,1);
				dup2( defaulterr, 2 );

				int pid = fork();
				if (pid == -1 ) {
					perror( "cat_grep: fork");
					exit( 2 );
				}
	
				if (pid == 0) {
				
					close(fdpipe[0]);
					close(fdpipe[1]);
					close( defaultin );
					close( defaultout );
					close( defaulterr );

					execvp(_simpleCommands[i]->_arguments[ 0 ],_simpleCommands[i]->_arguments);
	
					perror( "cat_grep: exec grep");
					exit( 2 );

				}		
			}
			else{
			        dup2( fdpipe[0], 0);
				dup2( fdpipe[1] ,1);
				dup2( defaulterr, 2 );

				int pid = fork();
				if (pid == -1 ) {
					perror( "cat_grep: fork");
					exit( 2 );
				}
	
				if (pid == 0) {
				
					close(fdpipe[0]);
					close(fdpipe[1]);
					close( defaultin );
					close( defaultout );
					close( defaulterr );

					execvp(_simpleCommands[i]->_arguments[ 0 ],_simpleCommands[i]->_arguments);
	
					perror( "cat_grep: exec grep");
					exit( 2 );

				}		
			
			}
		}
		dup2( defaultin, 0 );
		dup2( defaultout, 1 );
		dup2( defaulterr, 2 );

	
	        close(fdpipe[0]);
		close(fdpipe[1]);
		close( defaultin );
		close( defaultout );
		close( defaulterr );
		
		wait( 0 );
		wait( 0 );
		

	}*/
	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);
void sigintHandler(int sig_num)
{
    signal(SIGINT, sigintHandler);
    printf("\n You should type exit to terminate \n");
    printf("myshell>");
    fflush(stdout);
    fflush(stdout);
}
void change_directory(char *x){
	char y [200];
	chdir(x);
	printf("current dir is : %s \n",getcwd(y,200));
	printf("myshell>");
	fflush(stdout);
}
void change_home_directory(){
	struct passwd *pass =getpwuid(getuid());
	const char *homedirectory = pass->pw_dir;
	char y [200];
	chdir(homedirectory);
	printf("current dir is : %s \n",getcwd(y,200));
	printf("myshell>");
	fflush(stdout);
	

}

void processhandler(int sig)
{	
	int pid = getpid();
  	FILE *file_ptr= fopen("logs.txt","a");
  	fprintf(file_ptr,"process terminated with pid = %d\n",pid);
  	//printf("process terminated with pid = %d\n",pid);
  	fclose(file_ptr);
  	
}

int 
main()
{
	signal(SIGINT, sigintHandler);
	signal(SIGCHLD, processhandler);	
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

