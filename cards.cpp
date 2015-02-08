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
const char* Card::rankMap[14] = { " ","2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };


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
from 0 to <9x12^15 to represent what type of hand it is, AND sort hands based on the rank of the cards 
by adding all card ranks together. These can be further sorted by 
giving each card a weight from 1to13 (13 for cards that do matter, 1 for cards that dont) and multiplying the rank by that weight.
This means among the possible hands of rank 1 there are <55*12^5 different ways to order that hand.
I need to do this due to the arbitrary conditions for sorting an individual hand
The max number returned is is around 200 million for 5 aces which there simply cannot be, still this is 
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
	//Understanding the math.
	//Return KINDOFHAND*(5*rank of highest + 4*rane of next highest ...1*lowest)
	//cards are sorted from lowest to highest. Highest card is index 4;
	if (numConsecutive == 4){
		//check strait flush
		if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
			//two straight flush hands are sorted by the highest card in each hand;
			return STRAITFLUSH * (5 + 4 + 3 + 2 + 1)*cards[4].rank;
		}
		//two straight hands are sorted by the highest card in each hand;
		return STRAIT * (5 + 4 + 3 + 2 + 1)*cards[4].rank;
		//return "Strait";
	}
	//suits.
	if (numSpades == 5 || numClubs == 5 || numDiamonds == 5 || numHearts == 5){
		//two flush hands are sorted by highest ranking card, then by the next highest ranking card, etc.;
		return FLUSH * (5 * cards[4].rank + 4 * cards[3].rank + 3 * cards[2].rank + 2 * cards[1].rank + 1 * cards[0].rank);
		//return "Flush";
	}
	
	if (numSame == 3){//could be four of a kind or full house
		//in four of a kind the third card always equals the second and the fourth
		if ((cards.begin() + 2)->rank == (cards.begin() + 1)->rank && (cards.begin() + 2)->rank == (cards.begin() + 3)->rank){
			//two four of a kind hands are sorted by the rank of the 4 cards that have the same rank;
			//due to sorting cards 1,2,and 3 (zero indexed) are always in
			//I picked 2 because computer sciencentist love 2
			return FOURKIND*(5 + 4 + 3 + 2 + 1)*cards[2].rank;
			//return "Four of a Kind";
		}
		else{
			//two full house hands are sorted by the rank of the 3 cards that have the same rank and then by the rank of the 2 cards that have the same rank;
			//full house is 3 of a kind followed by two of a kind, i need to adjust for if the three of a kind is at the begining or the end.
			//simple enough if card at position 1 == card at position 2 it is at the end
			//the three pairs are at the other end, then its just basic math
			if (cards[1].rank == cards[2].rank){
				return FULLHOUSE * (5 * cards[0].rank + 4 * cards[1].rank + 3 * cards[2].rank + 2 * cards[3].rank + 1 * cards[4].rank);
			}else{
				return FULLHOUSE * (5 * cards[2].rank + 4 * cards[3].rank + 3 * cards[4].rank + 2 * cards[0].rank + 1 * cards[1].rank);
			}
			//return "Full House";
		}	
	}
	if (numSame == 2){ // three of kind or 2 pair
		//check if ends are equal for middle card. only three of a kind has this property
		//two three of a kind hands are sorted by the rank of the 3 cards that have the same rank;

		if (cards.begin()->rank == (cards.begin() + 2)->rank){
			return THREEKIND * (5 * cards[0].rank + 4 * cards[1].rank + 3 * cards[2].rank );

		}if( (cards.begin() + 4)->rank == (cards.begin() + 2)->rank){
			return THREEKIND * (5 * cards[4].rank + 4 * cards[3].rank + 3 * cards[2].rank );

			//return "Three of a Kind";
		}
		if ((cards.begin() + 1)->rank == (cards.begin() + 2)->rank && (cards.begin() + 2) == (cards.begin() + 3)){ //check if middle cards are equal
			return THREEKIND * (5 * cards[1].rank + 4 * cards[2].rank + 3 * cards[3].rank );
			//return "Three of a kind";
		}
		//two two pair hands are sorted by the rank of the higher pair, then by the rank of the lower pair, and then by the 5th card;
		
		//Determing the sorting order of two pair is more tedius than complicated.
		//The card that is not a pair is either in the middle, the beginning, or the end
		//Once we know that cards location we know the locations of the two pairs.
		//we sort those cards by the above.
		
		//middle
		if ((cards.begin() + 2)->rank != (cards.begin() + 3)->rank){
			//paris are at positions 0,1 & 3,4
			//check if higher rank is at position 1 or 3.
			if ((cards.begin() + 1)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR * (5 * cards[0].rank + 4 * cards[1].rank + 3 * cards[3].rank + 2 * cards[4].rank + 1 * cards[2].rank);

			}
			else{//equalto is trivial
				return TWOPAIR * (5 * cards[3].rank + 4 * cards[4].rank + 3 * cards[0].rank + 2 * cards[1].rank + 1 * cards[2].rank);

			}
		}
		//beginning'
		else if ((cards.begin())->rank != (cards.begin() + 1)->rank){
			//paris are at positions 1,2 & 3,4
			//check if higher rank is at position 2 or 3.
			if ((cards.begin() + 2)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR * (5 * cards[1].rank + 4 * cards[2].rank + 3 * cards[3].rank + 2 * cards[4].rank + 1 * cards[0].rank);

			}
			else{//equalto is trivial
				return TWOPAIR * (5 * cards[3].rank + 4 * cards[4].rank + 3 * cards[1].rank + 2 * cards[2].rank + 1 * cards[0].rank);

			}
		}
		//end
		else{
			//paris are at positions 0,1 & 2,3
			//check if higher rank is at position 1 or 3.
			if ((cards.begin() + 2)->rank > (cards.begin() + 3)->rank){
				return TWOPAIR * (5 * cards[1].rank + 4 * cards[2].rank + 3 * cards[3].rank + 2 * cards[4].rank + 1 * cards[0].rank);

			}
			else{//equalto is trivial
				return TWOPAIR * (5 * cards[3].rank + 4 * cards[4].rank + 3 * cards[1].rank + 2 * cards[2].rank + 1 * cards[0].rank);

			}
		}
	}
	if (numSame == 1){
		//two one pair hands are sorted by the rank of the pair, then by the highest unpaired card, then the next highest unpaired card, and then by the next highest unpaired card;
		//for this to work the weight of the pair must satisfy the inequality
		//2x > 13+12+11 The 13+12+11 comes from the highest possible sum of cards
		//number the other 3 cards could have that being a A,K,Q or 13*3+12*3+11*3
		//one card of the pair will always be at positions 1,2,3. I then check adjacents
		if ((cards.begin() + 1)->rank == (cards.begin() + 0)->rank){
			return ONEPAIR * (20 * cards[0].rank + 20 * cards[1].rank + cards[4].rank +  cards[3].rank +  cards[2].rank);

		}
		if ((cards.begin() + 1)->rank == (cards.begin() + 2)->rank){
			return ONEPAIR * (20 * cards[1].rank + 20 * cards[2].rank +  cards[4].rank +  cards[3].rank +  cards[0].rank);

		}
		if ((cards.begin() + 2)->rank == (cards.begin() + 3)->rank){
			return ONEPAIR * (20 * cards[2].rank + 20 * cards[3].rank + cards[4].rank + cards[1].rank + cards[0].rank);

		}
		if ((cards.begin() + 3)->rank == (cards.begin() + 4)->rank){
			return ONEPAIR * (20* cards[3].rank + 20 * cards[4].rank + cards[2].rank + cards[1].rank +  cards[0].rank);

		}
	}
	//two unranked hands are sorted by highest ranking card, then by the next highest ranking card, etc.
	return UNRANKED * ( cards[4].rank +  cards[3].rank +  cards[2].rank +  cards[1].rank + cards[0].rank);
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