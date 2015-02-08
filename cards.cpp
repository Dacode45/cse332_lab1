/*
cards.cpp
David Ayeke, ayekedavidr@wustl.edu
Implementation of definitions from cards.h
*/

#include "stdafx.h"
#include "cards.h"


#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <string>

//Initializing mapping
const char* Card::suitMap[4] = { "C", "D", "H", "S" };
const char* Card::rankMap[13] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };


bool operator<(const Card& k, const Card& c){
	if (k.rank < c.rank){
		return true;
	}
	else if (k.rank == c.rank)
		return (k.suit < c.suit);
	else
		return false;
}

int getRankFromChar(char rChar, Card::RANK &rank){
	int toReturn;
	switch (rChar){
		case '2':
			rank = Card::RANK::two;
			toReturn =  SUCCESS;
		break;

		case '3':
			rank = Card::RANK::three;
			toReturn = SUCCESS;
		break;

		case '4':
			rank = Card::RANK::four;
			toReturn = SUCCESS;
		break;

		case '5':
			rank = Card::RANK::five;
			toReturn = SUCCESS;
		break;

		case '6':
			rank = Card::RANK::six;
			toReturn = SUCCESS;
		break;

		case '7':
			rank = Card::RANK::seven;
			toReturn = SUCCESS;
		break;

		case '8':
			rank = Card::RANK::eight;
			toReturn = SUCCESS;
		break;

		case '9':
			rank = Card::RANK::nine;
			toReturn = SUCCESS;
		break;

		case '0':
			rank = Card::RANK::ten;
			toReturn = SUCCESS;
		break;

		case 'J':
		case 'j':
			rank = Card::RANK::jack;
			toReturn = SUCCESS;
		break;

		case 'Q':
		case 'q':
			rank = Card::RANK::queen;
			toReturn = SUCCESS;
		break;

		case 'K':
		case 'k':
			rank = Card::RANK::king;
			toReturn = SUCCESS;
		break;

		case 'A':
		case 'a':
			rank = Card::RANK::ace;
			toReturn = SUCCESS;
		break;

		default:
			toReturn = IMPROPERFILEFORMAT;
		break;

	}

	return toReturn;
}

int getSuitFromChar(const char sChar, Card::SUIT &suit){
	switch (sChar)
	{
	case 'c':
	case 'C':
		suit = Card::SUIT::clubs;
		return SUCCESS;
	break;

	case 'd':
	case 'D':
		suit = Card::SUIT::diamonds;
		return SUCCESS;
	break;

	case 'h':
	case 'H':
		suit = Card::SUIT::hearts;
		return SUCCESS;
	break;

	case 's':
	case 'S':
		suit = Card::SUIT::spades;
		return SUCCESS;
	break;


	default:
		return IMPROPERFILEFORMAT;
		break;
	}

	return IMPROPERFILEFORMAT;
}

int parseCardFile(const char* filename, std::vector<Card> &cards){
	//Which Error to Return
	int toReturn = SUCCESS;
	std::string white_space_buffer; // used for skipping whitespace
	std::ifstream in(filename);
	std::string line;
	if (in.is_open()){
		
		while (std::getline(in, line)){
				
			if (in.fail()){
				in.clear();
				handleErrMessages(program_name, "Error durring File Handling. Will attempt to continue.");
				toReturn = ERRORDURINGFILEREADING;
			}
			
				std::istringstream iss(line);
				std::vector<Card> hand;

				char rankChar, suitChar;
				iss >> std::ws;
				while (iss >> rankChar){
					iss >> std::ws;
					iss >> suitChar;
					if (suitChar == '0'){
						rankChar = '0';
						iss >> std::ws;
						iss >> suitChar;
					}
					if (suitChar == '/' || rankChar == '/'){// reach end or comment
						break;
					}
					iss >> std::ws;

					Card::RANK r;
					Card::SUIT s;
					int rankErr = getRankFromChar(rankChar, r);
					int suitErr = getSuitFromChar(suitChar, s);
					if (rankErr || suitErr){
						if (!toReturn){
							handleErrMessages(program_name, "Improper File Format");
							if (rankErr){
								toReturn = IMPROPERFILEFORMAT;
							}
							else
								toReturn = IMPROPERFILEFORMAT;
						}
						continue;
					}
					Card c(s, r);
					hand.push_back(c);
				}
				if (hand.size() != 5){
					handleErrMessages(program_name, "Hands should only have 5 cards");
					toReturn = BADNUMBEROFCARDSINHAND;
				}
				else{
					std::move(hand.begin(), hand.end(), std::back_inserter(cards));
				}
			
	
		}
		in.close();
		return toReturn;
	}
	else{
		handleErrMessages(program_name, "Couldn't open the file");
		return CANTOPENFILE;
	}
}

/*
How it works. The highest rank a card can have is 12 (0..12) as defined
by the enum. There are 9 possible types of hands. That means I can return an integer
from 0 to 9x13^15 to represent what type of hand it is, AND sort hands based on the rank of the cards 
by adding all card ranks together. These can be further sorted by 
giving each card a weight from 1 to 5 and multiplying the ranke by that weight.
This means among the possible hands of rank 1 there are 15*13^5 different ways to order that hand.
The max number returned is is around 400 million for 5 aces which there simply cannot be, still this is 
less than the size of an unsigned int but more than the number of possible hands in a poker deck.
This is due to the fact I must sort hands in different ways depending on what hand shows up.
*/
const int UNRANKED = 1,
ONEPAIR = 2,
TWOPAIR = 3,
THREEKIND = 4,
STRAIT = 5,
FLUSH = 6,
FULLHOUSE = 7,
FOURKIND = 8,
STRAITFLUSH = 9;
//
//two four of a kind hands are sorted by the rank of the 4 cards that have the same rank;
//two full house hands are sorted by the rank of the 3 cards that have the same rank and then by the rank of the 2 cards that have the same rank;
//two flush hands are sorted by highest ranking card, then by the next highest ranking card, etc.;
//two three of a kind hands are sorted by the rank of the 3 cards that have the same rank;
//two two pair hands are sorted by the rank of the higher pair, then by the rank of the lower pair, and then by the 5th card;
//two one pair hands are sorted by the rank of the pair, then by the highest unpaired card, then the next highest unpaired card, and then by the next highest unpaired card;
//two unranked hands are sorted by highest ranking card, then by the next highest ranking card, etc.

unsigned int checkHand(std::vector<Card> &cards){
	std::sort(cards.begin(), cards.end());
	int numSpades = 0, numClubs = 0, numDiamonds = 0, numHearts = 0, numSame = 0, numConsecutive = 0;
	int numBreaks = 0;//Check for when cards stop being same value

	for (auto it = cards.begin(); it != cards.end()-1; ++it){
		switch (it->suit){
		case Card::SUIT::clubs:
			numClubs++;
		break;
		case Card::SUIT::spades:
			numSpades++;
		break;
		case Card::SUIT::diamonds:
			numDiamonds++;
		break;
		case Card::SUIT::hearts:
			numHearts++;
		break;
		}
		if (it->rank == (it + 1)->rank){
			numSame++;
		}
		else{
			numBreaks++;
			if (it->rank == ((it + 1)->rank - 1)){
				numConsecutive++;
			}
		}
	}

	//check last card suit
	switch ((cards.end()-1)->suit){
	case Card::SUIT::clubs:
		numClubs++;
		break;
	case Card::SUIT::spades:
		numSpades++;
		break;
	case Card::SUIT::diamonds:
		numDiamonds++;
		break;
	case Card::SUIT::hearts:
		numHearts++;
		break;
	}

	//check strait
	//Understanding math.
	//Return KINDOFHAND*(5*rank of highest + 4*rane of next highest ...1*lowest)
	//cards are sorted from lowest to highest. Highest card is index 4;
	if (numConsecutive == 4){
		//check strait flush
		if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
			//two straight flush hands are sorted by the highest card in each hand;
			return STRAITFLUSH * (5 + 4 + 3 + 2 + 1)*cards[4].rank;
		}
		//two straight hands are sorted by the highest card in each hand;
		return STRAITFLUSH * (5 + 4 + 3 + 2 + 1)*cards[4].rank;
		//return "Strait";
	}
	//suits.
	if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
		return "Flush";
	}
	
	if (numSame == 3){//could be four of a kind or full house
		//in four of a kind the third card always equals the second and the fourth
		if ((cards.begin() + 2)->rank == (cards.begin() + 1)->rank && (cards.begin() + 2)->rank == (cards.begin() + 3)->rank)
			return "Four of a Kind";
		else
			return "Full House";
	}
	if (numSame == 2){ // three of kind or 2 pair
		if (cards.begin()->rank == (cards.begin() + 2)->rank || (cards.begin() + 4)->rank == (cards.begin() + 2)->rank) //check if ends are equal for middle card. only three of a kind has this property
			return "Three of a Kind";
		if ((cards.begin() + 1)->rank == (cards.begin() + 2)->rank && (cards.begin() + 2) == (cards.begin() + 3)) //check if middle cards are equal
			return "Three of a kind";

		return "Two Pair";
	}
	if (numSame == 1){
		return "One Pair";
	}
	return "no rank";
}

int printCards( std::vector<Card> &cards){
	//EX Three of Diamonds
	try{

		//Print Hand
		for (auto it = cards.begin(); it != cards.end(); it += 5){
			
			std::vector<Card> hand;
			std::copy(it, it + 5, std::back_inserter(hand));
			std::cout << checkHand(hand) << std::endl;
		}
		//Print all Cards

		std::sort(cards.begin(), cards.end());

		for (auto it = cards.begin(); it != cards.end(); ++it){
			std::cout << Card::rankMap[it->rank] << Card::suitMap[it->suit] << std::endl;
		}
		return SUCCESS;
	}
	catch (int e){
		handleErrMessages(program_name, "C++ Error " + e);
		return FAILEDTOPRINTCARDS;
	}
	
}

int usageMessage(const char* programName){
	try{

		std::printf("Usage: %s card_file.txt", programName);
		return PRINTEDUSAGEMESSAGE;
	}
	catch (int e){
		std::cout << "C++ Error " << e;
		return FAILEDTOPRINTUSAGE;
	}
	
}

void handleErrMessages(const char* pName, int err){

	switch (err){
	case SUCCESS:// SUCCESS:

		break;
	case CANTOPENFILE://CANTOPENFILE:
		std::cout << "Couldn't Open File. May not Exist\n";
		usageMessage(pName);
		break;
	case IMPROPERFILEFORMAT://IMPROPERFILEFORMAT:
		std::cout << "Card Format incorrect. Check Readme on the format\n";
		break;
	case ERRORDURINGFILEREADING:// ERRORDURINGFILEREADING:
		std::cout << "File opened, but there was an error while reading. File may be currupt\n";
		break;
	case FAILEDTOPRINTUSAGE://FAILEDTOPRINTUSAGE:
		std::cout << "This should never ever happen. Failed to print usage message\n";
		break;
	case PRINTEDUSAGEMESSAGE://PRINTEDUSAGEMESSAGE:
		std::cout << "Printed Usage Message \n";
		break;

	case FAILEDTOPRINTCARDS: //FAILEDTOPRINTCARDS
		std::cout << "This should not happen. C++ error: Unable to access vector<Cards>.\n";
		break;
	case BADNUMBEROFCARDSINHAND: //BADNUMBEROFCARDSINHAND
		std::cout << "There should only ever be 5 cards in your hand at a time. no more no less\n";
		break;
	default:
		break;
	}
}

void handleErrMessages(const char* pName, const char* errMsg){
	std::printf("%s: %s\n", pName, errMsg);
}