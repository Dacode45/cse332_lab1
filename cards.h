/*
cards.cpp
David Ayeke, ayekedavidr@wustl.edu
Definitions of playing cards and functions to use them
*/
#ifndef CARDS_H
#define CARDS_H

#include<vector>

extern const char* program_name;

const int expected_argument_count = 2;
enum  command_line_argument{pragram_name, file_name};

struct Card{
	enum  SUIT{clubs, diamonds, hearts, spades};
	//map { "Clubs", "Diamonds", "Hearts", "Spades" };
	static const char* suitMap[4]; 
	//made the underlying type an int so that I could compare two cards rank easilly
	enum  RANK{two=1, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace};//Somecode will access the buffer rank, rather than check i have this buffer. Save code. Since no card can ever have a rank of buffer it is fine. 
	// map Ranks to string
	static const char* rankMap[14];
	SUIT suit;
	RANK rank;

	Card(SUIT s, RANK r){
		suit = s;
		rank = r;
	}

	

};


bool operator<(const Card& k, const Card& c);

//Parses ; file, and puts contents into a vector. Returns Success or Error.
int parseCardFile(const char* filename, std::vector<Card> &cards);
//Print out cards in vector
int printCards( std::vector<Card> &cards);
//Usage Message 
int usageMessage(const char* programName);
//couts error message
void handleErrMessages(const char* pName, int err);
void handleErrMessages(const char* pName, const char* errMsg);


const int SUCCESS= 0;
const int CANTOPENFILE= 1;
const int IMPROPERFILEFORMAT= 2;
const int ERRORDURINGFILEREADING =3;
const int FAILEDTOPRINTUSAGE = 4;
const int PRINTEDUSAGEMESSAGE= 5;
const int FAILEDTOPRINTCARDS= 6;
const int BADNUMBEROFCARDSINHAND =7;
const int TOOMANYARGUMENTS = 8;
const int NOTENOUGHARGUMENTS = 9;

#endif // !CARDS_H
