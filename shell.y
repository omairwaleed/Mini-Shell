
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */
%error-verbose
%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE AMPERSAND APPEND LESS ERROR PIPE EXIT CHDIR

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	EXIT {
		printf( " GOOD BYE !! \n");
		exit(2);
	}
	|
	CHDIR WORD{
		change_directory($2);
		
	}
	|
	CHDIR{
		change_home_directory();
		
	}
	|
	simplecommands iomodifier_opt NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	|
	simplecommands iomodifier_opt AMPERSAND NEWLINE {
		printf("   Yacc: Execute background command\n");
		Command::_currentCommand.activate_background();
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;
	
simplecommands:
	command_and_args
	|
	simplecommands command_and_args
	;

command_and_args:
	command_word arg_list pipe {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;
pipe:
	PIPE{
		printf("   Yacc: insert pipe \n");
	}
	|
	;
	
arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
	       Command::_currentSimpleCommand->insertArgument( $1 );\
               printf("   Yacc: insert argument \"%s\"\n", $1);
	}
	;

command_word:
	WORD { 
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	       printf("   Yacc: insert command \"%s\"\n", $1);
	}
	;

iomodifier_opt:
	iomodifier_optional
	|
	iomodifier_opt iomodifier_optional
	;
	
	
iomodifier_optional:

	GREAT WORD {
		Command::_currentCommand._outFile = $2;
		printf("   Yacc: insert output -overwrite \"%s\"\n", $2);
	}
	|
	APPEND WORD {
		Command::_currentCommand.activate_append();
		Command::_currentCommand._outFile = $2;
		printf("   Yacc: insert output -append \"%s\"\n", $2);
	}
	|
	LESS WORD{
		Command::_currentCommand._inputFile = $2;
		printf("   Yacc: insert input \"%s\"\n", $2);
	
	}
	|
	ERROR WORD{
		Command::_currentCommand._errFile = $2;
		printf("   Yacc: insert error \"%s\"\n", $2);
	
	}
	| /* can be empty */ 
	;

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
