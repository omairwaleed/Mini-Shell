
#ifndef command_h
#define command_h

// Command Data Structure
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;

	// Number of arguments
	int _numberOfArguments;
	char ** _arguments;
	
	SimpleCommand();
	void insertArgument( char * argument );
};

struct Command {
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	int _appendflag;
	
	SimpleCommand ** _simpleCommands;
	char * _outFile;
	char * _inputFile;
	char * _errFile;
	int _background;
	
	void activate_append();
	void activate_background();
	void prompt();
	void print();
	void execute();
	void clear();
	
	
	Command();
	void insertSimpleCommand( SimpleCommand * simpleCommand );

	static Command _currentCommand;
	static SimpleCommand *_currentSimpleCommand;
};
void change_directory(char *x);
void change_home_directory();

#endif
