
#include <iostream>     // cout
#include <stdlib.h>     // srand, rand
#include <time.h>       // time
#include <iomanip>      // setw
#include <vector>       // vector<>
#include <string>       // string
#include <tuple>		// tuple, make_tuple()
#include <fstream>		// ofstream: Stream class to write on files
#include <conio.h>		// getch() to capture character immediately from the standard input.
#include <algorithm>	// find()

#include "Constants.h"
using namespace std;



/* GLOBAL VARIABLES */
vector<tuple<int, int>> forbiddenPoints;
vector<tuple<int, int>> visitedPoints;
vector<tuple<int, int>> goldPoints;

int collectedGold = 0;
int catchedMonster = 0;
int rowSize, colSize;
int currentRow, currentCol;
char player;
string gameScoresFileName = "game_scores.txt";




/* GET FUNCTIONS */
void getRowAndColumn();
char getDirectionFromUser();
int getRandomNumberBetween(int, int);
int** getMazeMatrix();
int getRandomDirection(string);
int getMoveableDirection(int**);
tuple<int, int> getCurrentPosition();
vector<tuple<int, int>> createPath(tuple<int, int>, tuple<int, int>);
char whoIsPlayer();



/* CONTROL FUNCTIONS */
bool isInInterval(int, int, int);
bool isThereAWall(int**, int);
bool isForbidden(int, int);
bool canMove(int);
bool countGold();



/* SET FUNCTIONS */
void fillMazeMatrix(int**, int, int);
void drawPath(int**, vector<tuple<int, int>>, int);
tuple<int, int> move(int**, int);
void writeScoresToFile();



/* PRINT FUNCTIONS */
void printCurrentPosition();
void printDirection(char);
void explainDirections();
void explainObjects();
void explainAim();
void printMazeMatrix(int**);
void printScores();
void printFinalPath();



/* GAME FUNCTIONS */
void initializeGame(int** );
void initializeConstants();
void user(int**);
void computer(int**);



int main() {

	// initialize random seed:
	srand(time(NULL));


	getRowAndColumn();
	int** maze = getMazeMatrix();


	int randomNumberFrom = 0, randomNumberTo = 4;
	fillMazeMatrix(maze, randomNumberFrom, randomNumberTo);


	tuple<int, int> startPosition = make_tuple(0, 0);
	tuple<int, int> finalPosition = make_tuple(rowSize - 1, colSize - 1);
	vector<tuple<int, int>> path  = createPath(startPosition, finalPosition);
	drawPath(maze, path, ROAD);


	explainAim();
	explainObjects();


	player = whoIsPlayer();
	player == 'U' ? user(maze) : computer(maze);


	printScores();
	writeScoresToFile();


	system("pause");
	return 0;
}

char whoIsPlayer() {
	char player;
	cout 
		<< endl 
		<< "Who will play the game ? (U)ser or (C)omputer ?" << setw(10) << " ";

	cin >> player;
	return player;
}

void user(int** maze) {
	explainDirections();
	cout << "Press any key to start...";
	_getch();

	initializeGame(maze);

	char direction;

	while (currentRow != rowSize - 1 || currentCol != colSize - 1){
		direction = getDirectionFromUser();

		if (direction == 0)  continue;
		
		system("cls");
		
		if (canMove(direction) && !isThereAWall(maze, direction)){
			move(maze, direction);
			printMazeMatrix(maze);
			visitedPoints.push_back(getCurrentPosition());

			if (maze[currentRow][currentCol] == GOLD && countGold()) {
				printDirection(direction);
				cout << setw(10) << " " << "COLLECTED GOLD: " << collectedGold << setw(14) << " ";
				printCurrentPosition();
			}
			
			else if (maze[currentRow][currentCol] == MONSTER){
				cout << "MONSTEEEEER ! Press any key to restart...";
				_getch();
				_getch();
				initializeGame(maze);
			}

			else{
				printDirection(direction);
				cout << setw(10) << " " << "MOVED" << setw(26) << " ";
				printCurrentPosition();
			}
		}
		else {
			printMazeMatrix(maze);
			printDirection(direction);
			cout << setw(10) << " " << "CANNOT MOVE" << setw(20) << " ";
			printCurrentPosition();
		}
	}
}

bool countGold() {
	
	tuple<int, int> currentPosition = make_tuple(currentRow, currentCol);
	
	//	Reference: https://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object
	if (find(goldPoints.begin(), goldPoints.end(), currentPosition) != goldPoints.end())
		return false;


	collectedGold++;
	goldPoints.push_back(currentPosition);
	return true;
}

void computer(int** maze) {

	initializeGame(maze);
	int direction = getMoveableDirection(maze);

	cout << "Computer is trying to find a path..." << endl;

	while (currentRow != rowSize - 1 || currentCol != colSize - 1) {
		if (direction == 0) {    //  stucked
			forbiddenPoints.push_back(getCurrentPosition());

			try{
				tuple<int, int> previousPosition = visitedPoints.back();
				currentRow = get<0>(previousPosition);
				currentCol = get<1>(previousPosition);
				visitedPoints.pop_back();
			}catch (const exception&){}

		}
		else {
			move(maze, direction);
			visitedPoints.push_back(getCurrentPosition());

			if (maze[currentRow][currentCol] == GOLD) {
				collectedGold++;
			}
			else if( maze[currentRow][currentCol] == MONSTER )
			{	
				forbiddenPoints.push_back(getCurrentPosition());
				catchedMonster++;
				initializeConstants();
				visitedPoints.clear();
			}
		}

		direction = getMoveableDirection(maze);
	}
}

//	Reference: http://www.cplusplus.com/doc/tutorial/files/
void writeScoresToFile() {
	ofstream gameScoresFile;

	try
	{
		gameScoresFile.open(gameScoresFileName);

		gameScoresFile
			<< "Number of collected  gold:" << string(10, ' ') << collectedGold << endl;

		if (player != 'U')
			gameScoresFile << "Number of catched monster:" << string(10, ' ') << catchedMonster << endl << endl;

		gameScoresFile
			<< string((colSize - 4) * 2 + 1, '-') << " FINAL PATH " << string((colSize - 4) * 2 + 1, '-') << endl;


		vector<tuple<int, int>>::iterator it;
		int row, col;

		for (it = visitedPoints.begin(); it != visitedPoints.end(); it++) {
			row = get<0>(*it);
			col = get<1>(*it);
			gameScoresFile << string((colSize - 4) * 2 + 3, ' ') << "[ " << row << " , " << col << "]" << endl;
		}
	}
	catch (const std::exception&)
	{
		cout << "Error writing to " + gameScoresFileName << endl;
	}

	gameScoresFile.close();

}

void printScores() {


	cout 
		<< endl
		<< "Number of collected  gold:" << setw(10) << collectedGold << endl;

	if (player != 'U')
		cout<< "Number of catched monster:" << setw(10) << catchedMonster << endl;

	//	The starting point is added to the beginning of the list
	visitedPoints.insert(visitedPoints.begin(), make_tuple(0, 0));	

	printFinalPath();
}

void printFinalPath() {

	cout << endl << string((colSize - 4) * 2 + 1, '-') << " FINAL PATH " << string((colSize - 4) * 2 + 1, '-') << endl;

	vector<tuple<int, int>>::iterator it;		//	Iterator

	int row, col;
	for (it = visitedPoints.begin(); it != visitedPoints.end(); it++) {
		row = get<0>(*it);
		col = get<1>(*it);
		cout << string((colSize - 4) * 2 + 3, ' ') << "[ " << row << " , " << col << "]" << endl;
	}
}

void initializeGame(int** maze) {
	system("cls");
	visitedPoints.clear();
	goldPoints.clear();
	initializeConstants();
	printMazeMatrix(maze);
}

void initializeConstants() {
	collectedGold = 0; 
	currentRow = 0; 
	currentCol = 0;
}

tuple<int, int> getCurrentPosition() {
	return make_tuple(currentRow, currentCol);
}

bool isThereAWall(int** maze, int direction) {
	int row = currentRow, col = currentCol;

	if (direction == UP)
		row--;
	else if (direction == LEFT)
		col--;
	else if (direction == DOWN)
		row++;
	else if (direction == RIGHT)
		col++;

	return maze[row][col] == WALL;
}

bool canMove(int direction) {

	if (currentRow == 0 && currentCol == 0)                     //  top left
		return (direction == DOWN || direction == RIGHT) ? true : false;

	if (currentRow == 0 && currentCol == colSize - 1)           //  top right
		return (direction == DOWN || direction == LEFT) ? true : false;

	if (currentRow == rowSize - 1 && currentCol == 0)           //  bottom left
		return (direction == UP || direction == RIGHT) ? true : false;

	if (currentRow == rowSize - 1 && currentCol == colSize - 1)  //  bottom right
		return (direction == UP || direction == LEFT) ? true : false;

	if (currentCol == 0)                                        //  the most left column
		return (direction != LEFT) ? true : false;

	if (currentCol == colSize - 1)                              //  the most right column
		return (direction != RIGHT) ? true : false;

	if (currentRow == 0)                                        //  top row
		return (direction != UP) ? true : false;

	if (currentRow == rowSize - 1)                              //  bottom row
		return (direction != DOWN) ? true : false;

	return true;
}

tuple<int, int> move(int** maze, int direction) {

	if (direction == DOWN)
		currentRow++;
	else if (direction == RIGHT)
		currentCol++;
	else if (direction == UP)
		currentRow--;
	else if (direction == LEFT)
		currentCol--;

	return make_tuple(currentRow, currentCol);
}

int getRandomDirection(string possibleDirections) {
	int possibleDirectionCount = (int)possibleDirections.length();
	int randomPositionInString = getRandomNumberBetween(0, possibleDirectionCount);
	return possibleDirections.at(randomPositionInString);
}

int getMoveableDirection(int** maze) {

	int row = currentRow, col = currentCol;
	int randomNumber = getRandomNumberBetween(0, 2);

	if (randomNumber == 0) {
		if (canMove(RIGHT) && !isThereAWall(maze, RIGHT) && !isForbidden(row, col++))
			return RIGHT;
		if (canMove(DOWN) && !isThereAWall(maze, DOWN) && !isForbidden(row++, col))
			return DOWN;
	}
	else {
		if (canMove(DOWN) && !isThereAWall(maze, DOWN) && !isForbidden(row++, col))
			return DOWN;
		if (canMove(RIGHT) && !isThereAWall(maze, RIGHT) && !isForbidden(row, col++))
			return RIGHT;
	}

	return 0;
}

bool isForbidden(int row, int col) {

	tuple<int, int> point = make_tuple(row, col);
	return find(forbiddenPoints.begin(), forbiddenPoints.end(), point) != forbiddenPoints.end();
}

vector<tuple<int, int>> createPath(tuple<int, int> beginningPoint, tuple<int, int> endPoint) {

	int startingRow = get<0>(beginningPoint), startingCol = get<1>(beginningPoint);
	int endRow = get<0>(endPoint), endCol = get<1>(endPoint);

	vector<tuple<int, int>> path;
	path.push_back(beginningPoint);


	int currentRow = startingRow, currentCol = startingCol, randomDirection;

	while (currentRow != endRow || currentCol != endCol) {

		if (currentRow != endRow && currentCol != endCol)
			randomDirection = getRandomDirection("sd");
		else if (currentRow != endRow && currentCol == endCol)
			randomDirection = getRandomDirection("s");
		else // ( currentRow == endRow && currentCol != endCol)
			randomDirection = getRandomDirection("d");

		if (randomDirection == DOWN && currentRow <= endRow) {
			currentRow++;
			path.push_back(make_tuple(currentRow, currentCol));
		}
		else if (randomDirection == RIGHT && currentCol <= endCol) {
			currentCol++;
			path.push_back(make_tuple(currentRow, currentCol));
		}
	}

	path.push_back(endPoint);
	return path;
}




//  Reference: http://www.math.uaa.alaska.edu/~afkjm/csce211/handouts/ReadingLineOfText
//  Reference: https://stackoverflow.com/questions/35098211/how-do-i-return-two-values-from-a-function

char getDirectionFromUser() {
	char direction = _getch();
	return (direction == RANDOM) ? getRandomDirection("wasd") : direction;
}

int** getMazeMatrix() {
	int** maze = new int*[rowSize];
	for (int i = 0; i < rowSize; i++)
		maze[i] = new int[colSize];

	return maze;
}

void fillMazeMatrix(int** maze, int randomFrom, int randomTo) {

	for (int r = 0; r < rowSize; r++)
		for (int c = 0; c < colSize; c++)
			maze[r][c] = getRandomNumberBetween(randomFrom, randomTo);
}

void printMazeMatrix(int** maze) {

	cout << endl << string((colSize - 2) * 2, '-') << " MAZE " << string((colSize - 2) * 2, '-') << endl;

	for (int r = 0; r<rowSize; r++) {
		for (int c = 0; c<colSize; c++) {

			cout << (r == currentRow && c == currentCol ? "x" : to_string(maze[r][c])) << setw(3) << " ";

		}
		cout << endl;
	}
	cout << endl;
}

int getRandomNumberBetween(int from, int to) {
	return rand() % to + from;
}

void getRowAndColumn() {
	cout
		<< "Please, enter row and column number of the maze." << endl
		<< "They both must be between " << MIN_ROW_NUMBER << " and " << MAX_ROW_NUMBER << "." << endl << endl;

	int row = -1, col = -1;
	while (1) {
		cout << "Row" << setw(33) << " ";
		cin >> row;

		cout << "Column" << setw(30) << " ";
		cin >> col;

		if ((!isInInterval(MIN_ROW_NUMBER, row, MAX_ROW_NUMBER)) ||
			(!isInInterval(MIN_COL_NUMBER, col, MAX_COL_NUMBER)))
			cout
			<< "They both must be between " << MIN_ROW_NUMBER << " and " << MAX_ROW_NUMBER << "." << endl
			<< "Please, re-enter row and column number of the maze." << endl;
		else {
			rowSize = row;
			colSize = col;
			return;
		}
	}
}

bool isInInterval(int min, int number, int max) {
	return (max >= number && number >= min) ? true : false;    //  10 > 8 > 2 true; 10 > 11 > 2 false.
}

void explainDirections() {
	cout
		<< endl
		<< string(13, '-') << " DIRECTIONS " << string(13, '-') << endl
		<< "w" << setw(37) << "UP" << endl
		<< "a" << setw(37) << "LEFT" << endl
		<< "s" << setw(37) << "DOWN" << endl
		<< "d" << setw(37) << "RIGHT" << endl
		<< "r" << setw(37) << "COMPUTER CHOICE RANDOMLY" << endl
		<< string(38, '-') << endl;
}

void explainObjects() {
	cout
		<< endl
		<< string(14, '-') << " OBJECTS " << string(15, '-') << endl
		<< ROAD << setw(37) << "ROAD" << endl
		<< WALL << setw(37) << "WALL" << endl
		<< GOLD << setw(37) << "GOLD" << endl
		<< MONSTER << setw(37) << "MONSTER" << endl
		<< string(38, '-') << endl;
}

void explainAim() {
	cout
		<< endl
		<< "Find the exit of the maze." << endl
		<< "Maze entrance:" << setw(24) << "[ 0, 0 ]" << endl
		<< "Maze exit:    " << setw(18) << "[ " << rowSize - 1 << ", " << colSize - 1 << " ]" << endl;

}

void printDirection(char direction) {
	cout << "Direction:" << setw(2) << " " << direction << setw(10) << " ";
}

void printCurrentPosition() {

	cout << "Current Position: " << "[" << currentRow << " , " << currentCol << "]" << endl;
}

void drawPath(int** maze, vector<tuple<int, int>> path, int objectId) {

	tuple<int, int> point;
	int r, c;
	for (int i = 0; i<path.size(); i++) {
		point = path[i];
		r = get<0>(point);
		c = get<1>(point);
		maze[r][c] = objectId;
	}
}
