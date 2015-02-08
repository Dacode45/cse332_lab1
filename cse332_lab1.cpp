// cse332_lab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cards.h"

const char* program_name;

using namespace std;


int main(int argc, char* argv[])
{
	int toReturn = SUCCESS;

	program_name = argv[command_line_argument::pragram_name];

	if (argc > expected_argument_count){//too many arguments
		int usageErr = usageMessage(program_name);
		handleErrMessages(program_name, usageErr);
		return TOOMANYARGUMENTS;
	}
	if (argc != expected_argument_count){ //not enough arguments
		int usageErr = usageMessage(program_name);
		handleErrMessages(program_name, usageErr);
		return NOTENOUGHARGUMENTS;
	}

	const char* file_name = argv[command_line_argument::file_name];
	vector<Card> cards;
	int parseErr = parseCardFile(file_name, cards);
	if (parseErr){
		handleErrMessages(program_name, parseErr);
	}

	int printErr = printCards(cards);
	if (printErr){
		toReturn = printErr;
		handleErrMessages(program_name, printErr);
	}

	//a parsing error is ranked higher than any other errors
	//so I will return it before any other errors.
	return (parseErr) ? parseErr : toReturn;
}

