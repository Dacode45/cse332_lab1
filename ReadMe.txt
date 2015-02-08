I made program_name a global variable for 2 reasons. 
<rant>
1) cards.cpp and cards.h are not self contained headers that I will be 
importing into other projects where they might be conflicts. They are 
tied closely to this project ( I know this because we declare usage messages
and error handling in cards.h. I library header file wouldn't have these).
Given that the 3 files are tied so closely together, I figured I might as well
make program_name a global because 
2) it make it easier to call functions. Since functions that throw errors
will print out usage messages, and to print out a usage message requires
the program_name, these functions should have acess to the constant program_name
natively. 
3) program_name is only written to once (its a constant variable.) Also it is a char*
Its an essentially static variable that I can't perform operations on or with save for << and >>.
It is fundamentally useless to this program, I shouldn't bloat my parameter list by adding
program_name to every function called in cse332_lab1.cpp
</rant>
I may loose points for this, but I don't see a good reason not to do this right now.
source: many post from Stack Overflow

No errors or warnings happened during compilation. Tested program with
hands.txt

KD JD 6s 5s 3c // no rank (king high)

6H // not a hand (too few)

Qs QH	 JH Qc Jc // full house

6c 10s 9H  8s  7H // straight

AH Ks Js AD Ac // three of a kind

9D  6D 5D 8D 7D // straight flush

7s 8c 2s 5H 2c // one pair
     
Kc 3s 3D 2D 7c 5c  // not a hand (too many)

4s 4H  4D As 4c  // four of a kind

KH   10H 3H 2H 8H // flush

9c QD 10c 10D 9s // two pairs

This has comments, invalid hands, spaces, and all ranks.