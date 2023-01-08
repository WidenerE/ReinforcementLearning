#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>
#include <limits>
#include <stdlib.h>
#include <time.h>
using namespace std;

//This struct is used to pass around information about the current state of the game between turns
struct gameState
{
	//Chips left in piles
        int pileA;
        int pileB;
        int pileC;

	//Determines when the game has ended
        bool gameOver;
};

//Prototypes
char playGame(vector<string> unsafe);
vector<string> buildUnsafe();
gameState playerTurn(int pileA, int pileB, int pileC);
gameState machineTurn(vector<string> unsafe, int pileA, int pileB, int pileC);
bool searchUnsafe(vector<string> unsafe, int pileA, int pileB, int pileC);
void appendUnsafe(vector<string> unsafe, int pileA, int pileB, int pileC);
void train(vector<string> unsafe, int trainingSets, int maxSize);

int main() 
{
	//All unsafe moves from the external file are read into this vector
	vector<string> unsafe;

	//Randomize the number generator seed
	srand(time(NULL));

	//Main loop, stops only when the user Types 'N' at the end of a game
	while(true)
	{
		//Read in the unsafe moves
		unsafe = buildUnsafe();
		
		//Play a game
		if(playGame(unsafe) != 'Y')
			break;

		//Reset cin
		cin.ignore();
	}
}

//Reads in unsafe moves from external file and stores them in a vector
vector<string> buildUnsafe()
{
	//Input stream
	fstream inOut("unsafe", ios::in | ios::out);

	//Vector of strings used to store all unsafe moves	
	vector<string> unsafe;

	//Each line of input from the file
	string lineInput;

	//Move to beginning of file
	inOut.seekg(0);

	//Reads in moves line by line
	while(getline(inOut, lineInput))
	{
		unsafe.push_back(lineInput);
	}

	return unsafe;
}

//Pre-Condition: A vector has been created to store all unsafe moves and is passed in
//Post-Condition: A game of nim has been played and the user decides whether or not to play another
char playGame(vector<string> unsafe)
{
	//Tracks the number of "chips" in each pile
	int pileA, pileB, pileC;

	//Stores line input from the user that will later be parsed
	string lineInput;

	//Stores single integer input from user
	int input;

	//Stores a Y or N to determine if another game should be played
	char playAgain;

	//Stores user input specifying the max pile size for a machine v.s machine game
	int max;

	//Game state struct used to track the current pile sizes
	gameState myGameState;

	///////////////////////////////////////////////////////////
	//                          Set Up                       //
	///////////////////////////////////////////////////////////

	//Prompt player for pile sizes	
	cout << "Enter a positive number of chips for piles A B C: ";


	getline(cin, lineInput);
	cout << "\n";

	//String stream used to parse line input
	istringstream iss(lineInput);


	//Loop until player enters a positive number of chips for each pile
	for(int i = 0; i < 3; i++)
	{
		iss >> input;
		
		if(input > 0)
		{
			switch(i)
			{
			case 0:
				pileA = input;
				break;
			case 1:
				pileB = input;
				break;
			case 2:
				pileC = input;
				break;
			}
		}
		else
		{
			cout << "Please enter a valid number! ";
			if(cin.fail())
			{
				cin.clear();
				cin.ignore();
			}

			cin >> input;
			cin.ignore();
			cout << "\n";

			i--;
		}
	}


	//Display pile sizes
	cout << "The number of chips in each pile initially";
	cout << "\n";
	cout << "A:" + to_string(pileA) + "	B:" + to_string(pileB) + "	C:" + to_string(pileC);
	cout << "\n";
	cout << "\n";

	//Prompt player for turn order
	cout << "Type 0 if you want the MACHINE to start, 1 if YOU want to start, or 2 if you want the machine to play ITSELF: ";
	cin >> input;
	cin.ignore();
	cout << "\n";

	//Loop until valid turn order is entered
	while((input != 0 && input != 1 && input != 2) || !cin)
	{
		cout << "Please enter a 0, 1, or 2! ";
	
                
                cin.clear();
                cin.ignore();

		cin >> input;
		cin.ignore();

	
		cout << "\n";
	}

	////////////////////////////////////////////////////////////////////////////////
	//                            Gameplay loop                                   //
	////////////////////////////////////////////////////////////////////////////////
	while(true)
	{
		//Machine has been selected to go first
		if(input == 0)
		{
			//Machine's turn is simulated, and the new state of each pile is returned
			myGameState = machineTurn(unsafe, pileA, pileB, pileC);
			pileA = myGameState.pileA;
			pileB = myGameState.pileB;
			pileC = myGameState.pileC;


			//Checks to see if the game has ended (All piles have 0 chips)
			//If so, break out of the loop
			if(myGameState.gameOver)
			{
				cout << "You have won the game!";
				cout << "\n";
				cout << "\n";
				break;
			}

			//Re-read in the unsafe moves incase the machine added more on its last turn
			unsafe = buildUnsafe();

			//Player takes their turn
			myGameState = playerTurn(pileA, pileB, pileC);
			pileA = myGameState.pileA;
                        pileB = myGameState.pileB;
                        pileC = myGameState.pileC;

			//Check for win
			if(myGameState.gameOver)
			{
				cout << "You lost the game!";
				cout << "\n";
				cout << "\n";
				break;
			}
		}
		//If the player decides to go first
		else if(input == 1)
		{
			//Player turn
			myGameState = playerTurn(pileA, pileB, pileC);
                        pileA = myGameState.pileA;
                        pileB = myGameState.pileB;
                        pileC = myGameState.pileC;

			//Check win
                        if(myGameState.gameOver)
                        {
                                cout << "You lost the game!";
                                cout << "\n";
                                cout << "\n";
                                break;
                        }

			//Machine turn
			myGameState = machineTurn(unsafe, pileA, pileB, pileC);
                        pileA = myGameState.pileA;
                        pileB = myGameState.pileB;
                        pileC = myGameState.pileC;

			//Check win
                        if(myGameState.gameOver)
                        {
                                cout << "You have won the game!";
                                cout << "\n";
                                cout << "\n";
                                break;
                        }

			unsafe = buildUnsafe();

	
		}
		//Machine simulates games against itself
		else if(input == 2)
		{
			//User is propmted for how many games to simulate
			cout << "How many games do you want to simulate? ";
			cin >> input;
			cout << endl;

			//User is prompted to limit the pile size for each machine game
			cout << "Max pile size? ";
			cin >> max;
			cout << endl;
	
			cin.ignore();

			//Games are simulated and then loop terminates
			train(unsafe, input, max);
			break;
		}
	}

	//User is asked if they would like to play another game
	cout << "Play again? (Y/N) ";

	cin >> playAgain;
	cout << "\n";
	cout << "\n";

	while(playAgain != 'Y' && playAgain != 'N')
	{
		cout << "Please enter a valid character! ";
		if(cin.fail())
                {
                	cin.clear();
                        cin.ignore();

                }


		cin >> playAgain;
		cin.ignore();
		cout << "\n";
		cout << "\n";
	}

	//User's answer is returned to the main loop
	return playAgain;
}

//Pre-condition: It is the player's turn in the gameplay loop
//Post-Condition: The player has taken chips from a pile and the game state is updated
gameState playerTurn(int pileA, int pileB, int pileC)
{
	//Store full line input from user
	string lineInput;

	//User selected pile letter
	char pile;

	//Number of chips to remove from pile
	int chipNum;

	//Ensures the user has provided valid input, otherwise used to continue the loop until they do
	bool invalidInput = true;

	//Current game state
	gameState myGameState;

	//User is prompted to remove 1-3 chips from a pile
	cout << "Enter the pile letter (A, B, C) and the number of chips to remove: ";

	getline(cin, lineInput);
	cout << "\n";

	istringstream iss(lineInput);

	//Line input is parsed
	iss >> pile >> chipNum;

	//Ensures chip number is valid
	while(chipNum > 3 || chipNum < 1)
	{
		cout << "Please enter a valid number of chips! ";
		if(cin.fail())
		{
			cin.clear();
			cin.ignore();

		}

		cin >> chipNum;
		cin.ignore();
		cout << "\n";
		cout << "\n";
	}

	//Removes chips from specified pile. If the user tries to pull more chips than are available, only the available chips are removed
	while(invalidInput)
	{
	
		switch(tolower(pile))
		{
		//Remove from pile a
		case 'a':
			if(pileA > 0)
			{
				if(pileA - chipNum >= 0)
					pileA -= chipNum;
				else
				{
					chipNum = pileA;
					pileA = 0;
				}
				invalidInput = false;
			}
			else
				invalidInput = true;
                        break;
		//Remove from pile b
		case 'b':
			if(pileB > 0)
			{
				if(pileB - chipNum >= 0)
                               		pileB -= chipNum;
                       		else
                       		{
                               		chipNum = pileB;
                                	pileB = 0;
                        	}
				invalidInput = false;
			}
			else
				invalidInput = true;
			break;
		//Remove from pile c
		case 'c':
			if(pileC > 0)
			{
				if(pileC - chipNum >= 0)
                                	pileC -= chipNum;
                        	else
                        	{
                                	chipNum = pileC;
                                	pileC = 0;
                        	}
				invalidInput = false;
			}
			else
				invalidInput = true;
                        break;


		default:
			invalidInput = true;

		}
		//Ensures a valid pile is specified
		if(invalidInput)
		{
			cout << "Please enter a valid pile! ";
			if(cin.fail())
			{
                       		cin.clear();
                        	cin.ignore();

			}

                        cin >> pile;
			cin.ignore();
			cout << "\n";
			cout << "\n";

		}
	}

	//Current state of the game is communicated to the player
	cout << "The human will take " + to_string(chipNum) + " chip(s) from pile " + pile;
	cout << "\n";

	cout << "THE NUMBER OF CHIPS IN EACH PILE NOW:";
	cout << "\n";
	cout << "A:" + to_string(pileA) + "     B:" + to_string(pileB) + "      C:" + to_string(pileC);
	cout << "\n";
	cout << "\n";

	myGameState.pileA = pileA;
	myGameState.pileB = pileB;
	myGameState.pileC = pileC;
	
	//If all chips have been removed, the game is over
	if(pileA == 0 && pileB == 0 && pileC == 0)
		myGameState.gameOver = true;
	else
		myGameState.gameOver = false;

	return myGameState;

}

//Pre-Condition: It is the machine's turn in the gameplay loop and an up to date list of unsafe moves has been created
//Post-Condition: The machine takes its turn and the game state is updated and returned
gameState machineTurn(vector<string> unsafe, int pileA, int pileB, int pileC)
{
	//The number of chips the machine removes
	int chipNum;
	//The pile from which it removes the chips
	char pile;

	gameState myGameState;

	//These if-else statments comprise the machines "intelligence" by telling it what the best moves to make based on the current game state are
	//The structure for each is largely the same
	
	//First, remove 3 chips from pile A if doing so will not land you on an unsafe move
	if(pileA >= 3 && !searchUnsafe(unsafe, pileA - 3, pileB, pileC))
	{
		//Remove chips from pile
		pileA -= 3;
		//Record removed chips
		chipNum = 3;
		//Record pile
		pile = 'A';

	}
	//3 from B
	else if(pileB >= 3 && !searchUnsafe(unsafe, pileA, pileB - 3, pileC))
        {
                pileB -= 3;
                chipNum = 3;
		pile = 'B';

        }
	//3 from C
	else if(pileC >= 3 && !searchUnsafe(unsafe, pileA, pileB, pileC - 3))
        {
                pileC -= 3;
                chipNum = 3;
		pile = 'C';

        }
	//2 from A
	else if(pileA >= 2 && !searchUnsafe(unsafe, pileA - 2, pileB, pileC))
        {
                pileA -= 2;
                chipNum = 2;
		pile = 'A';
	}
	//2 from B
	else if(pileB >= 2 && !searchUnsafe(unsafe, pileA, pileB - 2, pileC))
        {
                pileB -= 2;
                chipNum = 2;
		pile = 'B';

        }
	//2 from C
	else if(pileC >= 2 && !searchUnsafe(unsafe, pileA, pileB, pileC - 2))
        {
                pileC -= 2;
                chipNum = 2;
		pile = 'C';

        }
	//1 from A
	else if(pileA >= 1 && !searchUnsafe(unsafe, pileA - 1, pileB, pileC))
        {
                pileA -= 1;
                chipNum = 1;
		pile = 'A';

        }
	//1 from B
	else if(pileB >= 1 && !searchUnsafe(unsafe, pileA, pileB - 1, pileC))
        {
                pileB -= 1;
                chipNum = 1;
		pile = 'B';

        }
	//1 from C
	else if(pileC >= 1 && !searchUnsafe(unsafe, pileA, pileB, pileC - 1))
        {
                pileC -= 1;
                chipNum = 1;
		pile = 'C';

        }
	//Otherwise, if no moves are valid, add the moves leading to this game state to the external file and remove what chips are available
	else
	{
	
		//Add current game state to external file	
		appendUnsafe(unsafe, pileA, pileB, pileC);

		//Remove from pile A if possible, if not, try B and C
		if(pileA > 0)
		{
			if(pileA - 3 >= 0)
			{
				chipNum = 3;
				pileA -= 3;
			}
			else
			{
				chipNum = pileA;
				pileA = 0;
			}

			pile = 'A';

		}
		else if(pileB > 0)
                {
                        if(pileB - 3 >= 0)
                        {
                                chipNum = 3;
                                pileB -= 3;
                        }
                        else
                        {
                                chipNum = pileB;
                                pileB = 0;
                        }

			pile = 'B';

                }
		else if(pileC > 0)
                {
                        if(pileC - 3 >= 0)
                        {
                                chipNum = 3;
                                pileC -= 3;
                        }
                        else
                        {
                                chipNum = pileC;
                                pileC = 0;
                        }

			pile = 'C';

                }


	}

	//Machine reports on the current state of the game
	cout << "I take " + to_string(chipNum) + " chip(s) from pile " + pile + ".";
	cout << "\n";
	cout << "The number of chips in each pile after the computer moves is:";
	cout << "\n";
	cout << "A:" + to_string(pileA) + "     B:" + to_string(pileB) + "      C:" + to_string(pileC);
	cout << "\n";
	cout << "\n";

	
	myGameState.pileA = pileA;
        myGameState.pileB = pileB;
        myGameState.pileC = pileC;

	//If all chips have been removed, the game is over
        if(pileA == 0 && pileB == 0 && pileC == 0)
                myGameState.gameOver = true;
        else
                myGameState.gameOver = false;

        return myGameState;

		

}

//Pre-Condition: A machine turn is in progress and the machine wants to know what moves are valid
//Post-Condition: The machine is told if the requested move is safe or not 
bool searchUnsafe(vector<string> unsafe, int pileA, int pileB, int pileC)
{
	//Convert the current pile states to a string that matches the format in the external file
	string lineInput = to_string(pileA) + " " + to_string(pileB) + " " + to_string(pileC);

	//Lower and upper bounds for search
	int lowerBound = 0;
	int upperBound = unsafe.size() - 1;

	//Middle of the vector
	int mid;

	//Binary search 
	//Returns true if the requested move is found in the external file
	while(lowerBound <= upperBound)
	{
		//Set mid
		mid = lowerBound + (upperBound - lowerBound) / 2;

		//If found at mid, return true
		if(unsafe.at(mid) == lineInput)
			return true;

		//Otherwise, compare the strings to determine which half to search next
		if(unsafe.at(mid).compare(lineInput) < 0)
			lowerBound = mid + 1;
		else
			upperBound = mid - 1;
	
	}

	//If the vector is searched and the move is not found in the file (contents in vector), return false	
	return false;
}

//Pre-Condition: A losing game state has been identified by the machine on its turn
//Post-condition: The moves leading to the losing state are added to the external file
void appendUnsafe(vector<string> unsafe, int pileA, int pileB, int pileC)
{
	string inputLine;

	//Output stream
	fstream inOut("unsafe", ios::out | ios::trunc);

	//Move to beginning of the file
	inOut.seekg(0);

	//Loop through all 9 of the previous game states that can lead to the losing state, and add them to the vector
	for(int i = 1; i < 4; i ++)
	{
		//Pile A states
		inputLine = to_string(pileA + i) + " " + to_string(pileB) + " " + to_string(pileC);

		//Makes sure the state is not a duplicate
		if(!searchUnsafe(unsafe, pileA + i, pileB, pileC))
		{
			//Add to vector
			unsafe.push_back(inputLine);
	
		}

		//Pile B states
		inputLine = to_string(pileA) + " " + to_string(pileB + i) + " " + to_string(pileC);

                if(!searchUnsafe(unsafe, pileA, pileB + i, pileC))
		{
                        unsafe.push_back(inputLine);
		}

		//Pile C states
		inputLine = to_string(pileA) + " " + to_string(pileB) + " " + to_string(pileC + i);

                if(!searchUnsafe(unsafe, pileA, pileB, pileC + i))
		{
                        unsafe.push_back(inputLine);
		}

	}

	//Sort the vector
	sort(unsafe.begin(), unsafe.end());

	//Write sorted vector to file
	for(int i = 0; i < unsafe.size(); i++)
	{
		inOut << unsafe.at(i) << endl;
	}
}

//Pre-Condition: The user has decided to have the machine play itself during set up, and has specified how many games to simulate and the max pile size 
//Post-Condition: The machine has simulated all of the games and added any new unsafe states to the external file
void train(vector<string> unsafe, int trainingSets, int maxSize)
{
	gameState myGameState;

	int pileA, pileB, pileC;

	//This method is recursive and continues to be called until all games specified by the user have been played.
	//1 is subtracted from training sets at the conclusion of every game
	if(trainingSets > 0)
	{

		//Read in the external file
		unsafe = buildUnsafe();

		//Randomly add chips into each pile
		pileA = rand() % maxSize + 1;
		pileB = rand() % maxSize + 1;
		pileC = rand() % maxSize + 1;

		//Play a game
		while(true)
		{
	
			//Machine takes its turn and updates game state
			myGameState = machineTurn(unsafe, pileA, pileB, pileC);
                        pileA = myGameState.pileA;
                        pileB = myGameState.pileB;
                        pileC = myGameState.pileC;


			//Checks for win
                        if(myGameState.gameOver)
                        {
				//Counts games completed backwards
				cout << "End of game " + to_string(trainingSets);
				cout << endl;
				//Recursive call
                                train(unsafe, trainingSets - 1, maxSize);
				return;
				
                        }
			//Re-read in file contents
			unsafe = buildUnsafe();

		}
	}
	//Base case (trainingSets has reached 0 and all games have been simulated)
	else
		return;
}
