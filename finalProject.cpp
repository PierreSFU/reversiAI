// Pierre Drego 301301848

#include <iostream>
#include <string>
using namespace std;

//
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       // For rand and timer

// Memcpy
#include <cstring>

// Sleep
#include <unistd.h>

// Queue
#include <queue>
#include <list>

// Set precision
#include <iomanip>

// Font size


/*
    Notes: Black starts first! Black is 1, white is 2

*/
// Grid data
int const xLength = 8;
int const yLength = 8;

int static gridArray[xLength*yLength]; // Tracks current grid

list<int> static pieceLocations; // Location everything is placed
list<int> static emptySpotLocations; // Spots that nothing has been placed
list<int> static validPieceLocations; // Spots that you can play in

int numberEmptySquares = (xLength*yLength);

// AI self Playing Data
int static winner = -1;
int static pointsAI = 0;
int static pointsPlayer = 0;
int static winningPiece = 0;

int static numberIterations = 5000; // Number of tests to run

double static avgPlayoutsPerSecond = 0;
double static maxTimeAllowed = 5;

// Points for manhattan
int static winValue(3), drawValue(1), loseValue(-4); // How many points you get

// Heuristics
int static cornerValue = 20; // ****Extra points for being in the corner
float static heuristicWeightMultiplier = 1; // How much a given points value increases due to this

int static heuristicsEnabled = 1; // 0 we don't want heuristics with manhattan, 1 we want it with heuristics
int static flipPiecesHeuristic = 1;  // Extra points for flipped pieces
int static cornerPiecesHeuristic = 1;  // Extra points for corner pieces
int static relativeScoreHeuristic = 1; // More points if you have more points after the move relative to the other person
int static adjacentHeuristic = 1; // More points for more adjacent pieces

double static changeRatio = 0; // If the heuristic changes overtime or not


// Respective game pieces
int static playerPiece = 0;
int static aiPiece = 0;

// Respective turns
int static currentTurn = 0; // Tracks whose turn it is
int static notTestingAI = 1; // Are we just running tests?

// Error Checking
int static stopTrying = 100000000;
int static debugTracker = 0;
int static errorCounter = 0;
int static noMovesLeft = 0;


// Prints game over messages AND returns winning piece if someone wins
// else returns 0 if it's a draw, also
int gameOver()
{
    pointsPlayer = 0;
    pointsAI = 0;
    // Get total points
    for(int i = 0; i < xLength*yLength; i++)
    {
        if(gridArray[i] == playerPiece){
            pointsPlayer++;
        }
        else if(gridArray[i] == aiPiece){
            pointsAI++;
        }
    }
    if(pointsPlayer > pointsAI){
        // printf("Player won! %d\n", pointsPlayer);
        winningPiece = playerPiece;
    }
    else if (pointsAI > pointsPlayer){
        // printf("AI won! %d\n", pointsAI);
        winningPiece = aiPiece;
    }
    else{
        // printf("It's a draw!\n");
        winningPiece = 0;
    }
    // printf("Score: Player vs AI | %d vs %d\n", pointsPlayer, pointsAI);
    
    // displayGrid();
    // usleep(1000000);
    // printf("Bye!\n\n\n\n");

    return winningPiece;
}

// Flips pieces based on new piece's position, also checks if pieces are available to flip
int flipPieces(int pieceLocation, int thePiece, int justChecking, int countPiecesFlipped = 0)
{
    int myPiece = thePiece;
    int otherPiece = -1;
    if(myPiece == 1){
        otherPiece = 2;
    }
    else{
        otherPiece = 1;
    }

    int currentLocation;
    int validMove(0); // Determines if a move is valid
    int totalCount(0); // Counts number of pieces flipped

    int tempGridArray[xLength*yLength];
    for(int i = 0; i < xLength*yLength; i++){
        tempGridArray[i] = 0;
    }
    
    currentLocation = pieceLocation;
    // Upperleft Diagonal
    while(gridArray[(currentLocation-1)-xLength] == otherPiece )
    {
        if(((currentLocation/xLength)<1) || (currentLocation+xLength)%xLength == 0){ // Upper wall // Left wall
            break;
        }
        currentLocation = (currentLocation-1)-xLength;
        // printf("Upperleft column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if(((currentLocation/xLength)<1) || (currentLocation+xLength)%xLength == 0){ // Upper wall // Left wall
            break;
        }
        
        if(gridArray[(currentLocation-1)-xLength] == myPiece){
            currentLocation = pieceLocation;

            while(gridArray[(currentLocation-1)-xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation-1-xLength] = myPiece;
                currentLocation = (currentLocation-1)-xLength;
                validMove = 1;
                totalCount++;
            }
        }
    }

    currentLocation = pieceLocation;
    // Lowerleft Diagonal
    while(gridArray[(currentLocation-1)+xLength] == otherPiece)
    {
        if(((currentLocation/xLength)>=(xLength-1)) || (currentLocation+xLength)%xLength == 0){ // Lower wall // Left wall
            break;
        }
        currentLocation = (currentLocation-1)+xLength;
        // printf("Lowerleft column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if(((currentLocation/xLength)>=(xLength-1)) || (currentLocation+xLength)%xLength == 0){ // Lower wall // Left wall
            break;
        }

        if(gridArray[(currentLocation-1)+xLength] == myPiece){
            currentLocation = pieceLocation;
            
            while(gridArray[(currentLocation-1)+xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation-1+xLength] = myPiece;
                currentLocation = (currentLocation-1)+xLength;
                validMove = 1;
                totalCount++;
            }
        }

    }

    currentLocation = pieceLocation;
    // UpperRight Diagonal
    while(gridArray[(currentLocation+1)-xLength] == otherPiece )
    { 
        if(((currentLocation/xLength)<1) || ((currentLocation+xLength+1)%xLength == 0)){ // Upper wall // Right wall
            break;
        }
        currentLocation = (currentLocation+1)-xLength; 
        // printf("UpperRight column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if(((currentLocation/xLength)<1) || ((currentLocation+xLength+1)%xLength == 0)){ // Upper wall // Right wall
            break;
        }
               

        if(gridArray[(currentLocation+1)-xLength] == myPiece){
            currentLocation = pieceLocation;

            while(gridArray[(currentLocation+1)-xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation+1-xLength] = myPiece;
                currentLocation = (currentLocation+1)-xLength;
                validMove = 1;
                totalCount++;
            }
        }

    }

    currentLocation = pieceLocation;
    // LowerRight Diagonal
    while(gridArray[(currentLocation+1)+xLength] == otherPiece)
    {
        if(((currentLocation/xLength)>=(xLength-1)) || ((currentLocation+xLength+1)%xLength == 0)){ // Lower wall // Right wall
            break;
        }
        currentLocation = (currentLocation+1)+xLength;
        // printf("LowerRight column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if(((currentLocation/xLength)>=(xLength-1)) || ((currentLocation+xLength+1)%xLength == 0)){ // Lower wall // Right wall
            break;
        }
        

        if(gridArray[(currentLocation+1)+xLength] == myPiece){
            currentLocation = pieceLocation;
            while(gridArray[(currentLocation+1)+xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation+1+xLength] = myPiece;
                currentLocation = (currentLocation+1)+xLength;
                validMove = 1;
                totalCount++;
            }
        }

    }

    currentLocation = pieceLocation;
    // Horizontal Left
    while(gridArray[currentLocation-1] == otherPiece)
    {
        if((currentLocation+xLength)%xLength == 0){ // Left wall
            break;
        }
        currentLocation = currentLocation-1;
        // printf("Horizontal Left column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if((currentLocation+xLength)%xLength == 0){ // Left wall
            break;
        }
        
        if(gridArray[currentLocation-1] == myPiece){
            currentLocation = pieceLocation;
            
            while(gridArray[currentLocation-1] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                if((currentLocation+xLength)%xLength == 0){ // Left wall
                    // errorChecker("Horizontal Left column:row");
                    break;
                }
                tempGridArray[currentLocation-1] = myPiece;
                currentLocation = currentLocation-1;
                validMove = 1;
                totalCount++;
            }
        }
    }
  
    currentLocation = pieceLocation;
    // Horizontal Right
    while((gridArray[currentLocation+1] == otherPiece))
    {
        if((currentLocation+xLength+1)%xLength == 0){ // Right wall
            break;
        }
        currentLocation = currentLocation+1;
        // printf("Horizontal Right column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if((currentLocation+xLength+1)%xLength == 0){ // Right wall
            break;
        }
        
        
        if(gridArray[currentLocation+1] == myPiece){
            currentLocation = pieceLocation;
            
            while(gridArray[currentLocation+1] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                if((currentLocation+xLength+1)%xLength == 0){ // Right wall
                    // errorChecker("Horizontal Right column:row");
                    break;
                }
                tempGridArray[currentLocation+1] = myPiece;
                currentLocation = currentLocation+1;
                validMove = 1;
                totalCount++;
            }
        }

    }
    
    currentLocation = pieceLocation;
    // Up
    while(gridArray[(currentLocation)-xLength] == otherPiece)
    {
        if((currentLocation/xLength)<1){ // Upper wall
            break;
        }
        currentLocation = (currentLocation)-xLength;
        // printf("Up column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if((currentLocation/xLength)<1){ // Upper wall
            break;
        }
        

        if(gridArray[(currentLocation)-xLength] == myPiece){
            currentLocation = pieceLocation;
            
            while(gridArray[(currentLocation)-xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation-xLength] = myPiece;
                currentLocation = (currentLocation)-xLength;
                validMove = 1;
                totalCount++;
            }
        }
    }

    currentLocation = pieceLocation;
    // Down
    while(gridArray[(currentLocation)+xLength] == otherPiece)
    {
        if((currentLocation/xLength)>=(xLength-1)){ // Lower wall
            break;
        }
        currentLocation = (currentLocation)+xLength;
        // printf("Down column:row | %d:%d\n", pieceLocation%xLength, pieceLocation/xLength);
        if((currentLocation/xLength)>=(xLength-1)){ // Lower wall
            break;
        }

        if(gridArray[(currentLocation)+xLength] == myPiece){
            currentLocation = pieceLocation;
    
            while(gridArray[(currentLocation)+xLength] == otherPiece)
            {
                if(justChecking == 1){
                    return 1;
                }
                tempGridArray[currentLocation+xLength] = myPiece;
                currentLocation = (currentLocation)+xLength;
                validMove = 1;
                totalCount++;
            }
        }
    }

    if(countPiecesFlipped == 0)
    {
        int tempChanges = 1;
        for(int i = 0; i < xLength*yLength; i++)
        {
            if(tempGridArray[i] == myPiece){
                tempChanges++;
                // printf("Test: %d ", tempChanges);
                gridArray[i] = myPiece;
            }
        }
    }
    else{
        return totalCount; // Return number of pieces flipped
    }
    

    //usleep(90000);
    //displayGrid();
    if(validMove == 1){
        //displayGrid();
        return 1;
    }
    else if((validMove == 0) & (justChecking == 2)){
        printf("FlipPieces has detected the game is over!\n");
        gameOver();
        noMovesLeft = 1;
    }
    return 0;
}

// Checks all the valid moves available
void validMoves()
{
    validPieceLocations.clear();
    list<int> copyEmptySpot = emptySpotLocations;

    int tempLocation(0);
    while(!copyEmptySpot.empty())
    {
        tempLocation = copyEmptySpot.front();
        copyEmptySpot.pop_front();
        if(flipPieces(tempLocation, currentTurn, 1) == 1){
            //printf("Available Locations %d\n", validPieceLocations.front());
            validPieceLocations.push_front(tempLocation);
        }
    }
}

// Translates decimal to xy coordinates
void translateToXy(int digits)
{
    int x(0),y(0);
    y = digits/yLength;
    x = digits%xLength;

    printf("X,Y: %d,%d\n", x,y);
}

// Creates grid
void createGrid()
{
    for(int i = 0; i < xLength*yLength; i++){
                gridArray[i] = 0;
    }
    // (xlength*ylength/2) is half the board, + xlength/2 means go to the middle, 
    // -xlength means go up a vertical level
    gridArray[((xLength*yLength)/2) + (xLength/2) - xLength - 1] = 2;
    gridArray[((xLength*yLength)/2) + (xLength/2) - xLength] = 1;
    gridArray[((xLength*yLength)/2) + (xLength/2) - 1] = 1;
    gridArray[((xLength*yLength)/2) + (xLength/2)] = 2;

    // Add all the empty spots, and non empty spots
    for(int i = 0; i < xLength*yLength; i++)
    {
        if((i != (((xLength*yLength)/2) + (xLength/2) - xLength - 1)) &
           (i != (((xLength*yLength)/2) + (xLength/2) - xLength)) &
           (i != (((xLength*yLength)/2) + (xLength/2) - 1)) &
           (i != (((xLength*yLength)/2) + (xLength/2))))
        {
            emptySpotLocations.push_front(i);
        }
        else
        {
            pieceLocations.push_back(i);
        }
    }

    numberEmptySquares -= 4;
}

// Displays grid
int initializeDisplayGrid = 0;
void displayGrid(int theGrid[xLength*yLength] = gridArray)
{
    if((validPieceLocations.empty()) && (initializeDisplayGrid == 0)){
        validMoves();
        initializeDisplayGrid++;

        if(playerPiece == 1){
            cout << "\033[1;33m"<< "This is your color\n" << "\033[0m";
        }
        else if(playerPiece == 2){
            cout << "\033[1;35m"<< "This is your color\n" << "\033[0m";
        }
    }

    int validMovesCopyArray[xLength*yLength];
    for(int i = 0; i < xLength*yLength; i++){
        validMovesCopyArray[i] = -1;
    }
    if(currentTurn == playerPiece){
        while(!validPieceLocations.empty()){
            validMovesCopyArray[validPieceLocations.front()] = 3;
            validPieceLocations.pop_front();
        }
    }

    cout << "  ";
    // Display guidenumbers
    for(int i = 0; i < yLength; i++)
    {
        // cout << i+1 << " ";
         cout << "\033[1;34m"<< i << "\033[0m" << " ";
    }
    //

    int yGuideline = 0;
    //Display grid
    for(int i = 0; i < xLength*yLength; i++){
        // Display guidenumbers
        if(i%xLength == 0){
            cout <<  endl;
            cout << "\033[1;34m"<< yGuideline++ << "\033[0m" << " ";
        }       
        //-- End Display guidenumbers
        if(theGrid[i] == 1){
            cout << "\033[1;33m"<< 'o' << "\033[0m" << " ";
            // cout << "\033[1;33m"<< 'o' << "\033[0m" << " ";
        }
        else if(theGrid[i] == 2){
            cout << "\033[1;35m"<< 'o' << "\033[0m" << " ";
            // cout << "\033[1;35m"<< 'o' << "\033[0m" << " ";
        }
        else if(validMovesCopyArray[i] == 3){
            cout << "\033[1;51m"<< 'x' << "\033[0m" << " ";
        }
        else{
            cout << "\033[1;90m"<< 'o' << "\033[0m" << " ";
        }  
    }

    cout << endl;
}

// Displays <int>queue
void displayQueue(queue<int> myQueue)
{
    queue<int> tempQueue = myQueue;

    while(!(tempQueue.empty())){
        printf("%i:", tempQueue.front());
        tempQueue.pop();
    }
    printf("\n");
}

// Displays last error location
void errorChecker(string errorMsg = "Default error")
{
    errorCounter++;
    if(errorCounter >= stopTrying)
    {
        cout << "Error: " << errorMsg << "occured!";
        displayGrid();
        exit(0);
    }
}

// Checks if a piece is Adjacent, returns number of pieces adjacent
int isAdjacent(int location, int theGrid[xLength*yLength] = gridArray)
{
    int numberAdjacent = 0;

    // Checks if there is a wall on the left
    if((location%xLength)!=0)
    {
        // Check Diagonally left upper
        if((location/xLength)>=1){ // Checks if there's an upper wall
            if(theGrid[(location-1) - (xLength)] != 0){ // Check lower left
                // printf("upperleft! %d:%d \n", (location-1) - (xLength),location);
                // return 1;
                numberAdjacent++;
            }

            // Check adjacent vertically up        
            if(theGrid[location-(xLength)] != 0){
                // printf("Above! %d:%d \n", location-(xLength),location);
                // return 1;
                numberAdjacent++;
            }  
        }

        if((location/xLength)<(xLength-1)){ // Checks if there's a lower wall

            if(theGrid[(location-1) + (xLength)] != 0){
                // printf("lowerleft! %d:%d \n", (location-1) + (xLength),location);
                //return 1;
                numberAdjacent++;
            }
        }

        // Check adjacent horizontally left
        if(theGrid[location-1] != 0){
            // printf("Left! %d:%d \n", location-1,location);
            // return 1;
            numberAdjacent++;
        } 


    }

    // Checks if there is a wall on the right
    if(((location+1)%xLength)!=0)
    {
        if((location/xLength)<(xLength-1)){ // Checks if there's a lower wall
            // Check Diagonally right lower      
            if(theGrid[(location+1) + (xLength)] != 0){
                // printf("Lower right! %d:%d \n", (location+1) + (xLength),location);
                // return 1;
                numberAdjacent++;
            }

            // Check adjacent vertically Lower        
            if(theGrid[location+(xLength)] != 0){
                // printf("Below! %d:%d \n", location+(xLength),location);
                // return 1;
                numberAdjacent++;
            }  
        }

        if((location/xLength)>=1){ // Checks if there's an upper wall       
            if(theGrid[(location+1) - (xLength)] != 0){
                // printf("Upper Right! %d:%d \n", (location+1) - (xLength),location);
                // return 1;
                numberAdjacent++;
            }
        }

        // Check adjacent horizontally right
        if(theGrid[location+1] != 0){
            // printf("Right! %d:%d \n", location+1,location);
            // return 1;
            numberAdjacent++;
        }
    }    
   
    return numberAdjacent;
}

// Swaps two variables
void aiSwap(int a, int b)
{
    int temp = a;
    a = b;
    b = temp;
}

// Checks if the game is over, sets noMovesLeft to 1 if the game is over
int checkGameOver(){
    validPieceLocations.clear();
    validMoves();
    if(validPieceLocations.size() == 0)
    {
        gameOver();
        noMovesLeft = 1;
        return 1;
    }
    return 0;
}

// Returns 1 if piece is in a corner, else 0
int cornerPieceHeuristic(int possibleLocation)
{
    if((possibleLocation == 0) || (possibleLocation == (xLength-1)) || (possibleLocation == (xLength*yLength-1) || (possibleLocation == xLength*yLength-xLength+1)) ){
        return 1;
    }
    return 0;
}

// Determines the values received from the heuristic
int heuristicBonus(int theLocation)
{
    double extraPoints(0);
    if((cornerPiecesHeuristic) & (cornerPieceHeuristic(theLocation) == 1)){
        extraPoints += cornerValue;
    }
    if(flipPiecesHeuristic == 1){
        extraPoints += (flipPieces(theLocation, currentTurn, 0, 1)); // Adds points for number of pieces flipped
    }
    if(adjacentHeuristic == 1){
        extraPoints += isAdjacent(theLocation)/2;
    }
    extraPoints = extraPoints*heuristicWeightMultiplier;
    return extraPoints;
}


// Determines a VALID random move, links to validMoves, also ends the game
int randomMoveHeuristic()
{
    if(checkGameOver()){
        return -1;
    }
    
    double pointArray[xLength*yLength+1];
    for(int i = 0; i < xLength*yLength; i++){
        pointArray[i] = -1;
    }

    int numberValidSpots(0);
    // Gives meaning to valid locations
    while(!validPieceLocations.empty()) // -1 because we want the nth element
    {
        pointArray[validPieceLocations.front()] = 1;
        validPieceLocations.pop_front();
        numberValidSpots++;
    }
    
    int tempSum(0);
    int numberChosen(0), bestLocation(64);

    // Gives each bonus as needed
    for(int i = 0; i < xLength*yLength; i++){
        if(pointArray[i] != -1)
        {
            pointArray[i] += heuristicBonus(i);
            tempSum += pointArray[i];
        }
    }
    
    // Makes higher values more likely to be picked than lower ones
    numberChosen = rand() % tempSum;
    tempSum = 0;
    int i = 0;
    while(tempSum <= numberChosen){
        if(pointArray[i] != -1){
            tempSum += pointArray[i];
        }
        i++;
    }
    bestLocation = i-1; 

    if(bestLocation == 64){ // Error checker, won't happen... probably
        //exit(0);
        return -1;
    }
    return(bestLocation);
}

// Determines a VALID random move, pure manhattan
int randomMove()
{
    if(checkGameOver()){
        return -1;
    }

    int temp = -1;
    temp = rand() % validPieceLocations.size();

    // Pops elements until we reach the 'temp' element we want
    for(int i = 0; i < temp; i++) // -1 because we want the nth element
    {
        validPieceLocations.pop_front();
    }
    temp = validPieceLocations.front();

    if(temp == 64){
        printf("Temp64, Fatal error!\n");
        exit(0);
    }
    return(temp);
}

// Places piece IF possible
int placeValidPiece(int theLocation, int thePiece)
{
    if(gridArray[theLocation] == 0){
        gridArray[theLocation] = thePiece;
        pieceLocations.push_back(theLocation);
        emptySpotLocations.remove(theLocation);
        numberEmptySquares--;
        
        //translateToXy(theLocation);
        // Makes sure everything is flipped if needed
        //flipPieces(theLocation, thePiece, 1); // The 1 means we're just checking and not changing any values
        debugTracker = 0;
        return 1;
    }
    else{ // Error checker
        debugTracker++;
        if(debugTracker >= 10){
            exit(0);
        }
        checkGameOver();
        // printf("---------------------------GameOver------------------------------- %d\n", theLocation);
        validMoves();
        placeValidPiece(validPieceLocations.front(),thePiece);
        displayGrid();
        // sleep(5);
        return 0;
    }
    
}

queue<int> genRandQueue()
{
    int randomNumbers[xLength*yLength];
    int randomNumbersCount = 0;
    queue<int> qEmptySpots;
    
    printf("%ld\n", qEmptySpots.size());

    // Generate availabe spots
    for(int i = 0; i < xLength; i++){
        for(int j = 0; j < yLength; j++){
            if(gridArray[i+(j*4)] == 0){
                //printf("Pushing: %ld\n", i+(j*xLength));
                //qEmptySpots.push(j+(i*xLength));
                randomNumbers[randomNumbersCount++] = (j+(i*xLength));

            }
        }
    }

    // Shuffle array to make it random
    int a(0), b(0), temp(0);
    for(int i = 0; i < randomNumbersCount*2; i++){
        a = (rand() % randomNumbersCount); 
        b = (rand() % randomNumbersCount);

        temp = randomNumbers[a];
        randomNumbers[a] = randomNumbers[b];
        randomNumbers[b] = temp;
    }

    // Copy array over to queue
    for(int i = 0; i < randomNumbersCount; i++){
        qEmptySpots.push(randomNumbers[i]); 
    }

    int length = qEmptySpots.size();
    for(int i = 0; i < length; i++){
        printf("%d:", qEmptySpots.front());
        qEmptySpots.pop();
    }

    return qEmptySpots;
}


// Tests every possible outcome given the board layout, returns spot to move to
int determineNextMoveAiPure(int heuristics = 0, int myPiece = -1)
{
    if(checkGameOver() || myPiece == -1){
        printf("Games over!2\n");
        //exit(0);
        return -1;
    }

    // Storage for our old data
    int copyGridArray[xLength*yLength];
    list<int> copyPieceLocations, copyEmptySpotLocations, copyValidPieceLocations;
    int copyNumberEmptySquares;
    int copyCurrentTurn;
    int copyPlayerPiece;
    int copyAiPiece;
    int copyWinningPiece;

    // Save data
    memcpy(copyGridArray, gridArray, sizeof(gridArray));
    copyPieceLocations = pieceLocations;
    copyEmptySpotLocations = emptySpotLocations;
    copyValidPieceLocations = validPieceLocations;
    copyNumberEmptySquares = numberEmptySquares;
    copyCurrentTurn = currentTurn;
    copyPlayerPiece = playerPiece;
    copyAiPiece = aiPiece;
    copyWinningPiece = winningPiece;

    // Data collection
    float bestMoveArray[xLength*yLength+1]; // Stores best move
    int firstMove(0), nextMove(0), initialize(0); // Location we will test

    for(int i = 0; i < xLength*yLength; i++){
        bestMoveArray[i] = 0;
    }

    int timerIterations = 0;
    time_t first, last;
    first = clock();
    last = clock();

    // The number of tests we want to run
    for(int i = 0; i < numberIterations; i++)
    {
        // Plays game
        while(noMovesLeft == 0)
        {
            if(heuristics == 0){
                nextMove = randomMove();
            }
            else{
                nextMove = randomMoveHeuristic();
            }

            // game is over
            if(nextMove == -1){ 
                break;
            }

            placeValidPiece(nextMove, currentTurn);
            flipPieces(nextMove, currentTurn, 0);

            // Changes turn
            if(currentTurn == 1){
                currentTurn = 2;
            }
            else{
                currentTurn = 1;
            }
            if(initialize == 0){
                firstMove = nextMove;
                initialize++;
            }
        }

        // Determine winner
        if(myPiece == winningPiece){
            bestMoveArray[firstMove] += winValue;

            // Add relativeScore if enabled
            if(((relativeScoreHeuristic) && (heuristics)) && (myPiece == copyAiPiece)){
                bestMoveArray[firstMove] += (pointsAI - pointsPlayer);
            }
            else{
                bestMoveArray[firstMove] += (pointsPlayer - pointsAI);
            }
        }
        else if(myPiece == winningPiece){
            bestMoveArray[firstMove] += loseValue;

            // Add relativeScore if enabled
            if(((relativeScoreHeuristic) && (heuristics)) && (myPiece == copyAiPiece)){
                bestMoveArray[firstMove] -= (pointsAI - pointsPlayer);
            }
            else{
                bestMoveArray[firstMove] -= (pointsPlayer - pointsAI);
            }
        }
        else{
           bestMoveArray[firstMove] += drawValue;
        }
        
        // Reset grid to original format
        memcpy(gridArray, copyGridArray, sizeof(copyGridArray));
        pieceLocations = copyPieceLocations;
        emptySpotLocations = copyEmptySpotLocations;
        validPieceLocations = copyValidPieceLocations;
        numberEmptySquares = copyNumberEmptySquares; 
        currentTurn = copyCurrentTurn;
        playerPiece = copyPlayerPiece;
        aiPiece = copyAiPiece;
        noMovesLeft = 0;
        initialize = 0;
        pointsAI = 0;
        pointsPlayer = 0;

        // Timer, exit early if needed
        last = clock();
        timerIterations++;
        if(((last-first)/double(CLOCKS_PER_SEC)) > maxTimeAllowed){
            break;
        }
    }
    
    // Calculates avgPlayoutsPerSecond
    double timeitTakes = ((last-first)/double(CLOCKS_PER_SEC));
    avgPlayoutsPerSecond = ((( (timerIterations/timeitTakes) + avgPlayoutsPerSecond)) / 2);

    // Get best moves
    int bestLocation(xLength*yLength);
    bestMoveArray[bestLocation] = -1000000000000000000;

    for(int i = 0; i < xLength*yLength; i++){
        if((bestMoveArray[i] != 0) && (bestMoveArray[bestLocation] < bestMoveArray[i])){
            bestLocation = i;
        }
    }
    
    // Reset grid to original format
    memcpy(gridArray, copyGridArray, sizeof(copyGridArray));
    pieceLocations = copyPieceLocations;
    emptySpotLocations = copyEmptySpotLocations;
    validPieceLocations = copyValidPieceLocations;
    numberEmptySquares = copyNumberEmptySquares; 
    currentTurn = copyCurrentTurn;
    playerPiece = copyPlayerPiece;
    aiPiece = copyAiPiece;
    winningPiece= copyWinningPiece;
    
    if(bestLocation == 64){
        printf("Games over!1\n");
        gameOver();
        displayGrid();
        noMovesLeft = 1;
    }

    return bestLocation;
}

void aiTurn()
{
    while(noMovesLeft == 0)
    {
        if(currentTurn == aiPiece)
        {
            //int theRandomMove = randomMove();

            //sleep(1);
            int predictedMove = determineNextMoveAiPure(heuristicsEnabled, aiPiece);
            
            if(flipPieces(predictedMove, aiPiece, 1) == 1){
                if(placeValidPiece(predictedMove, aiPiece)){
                    flipPieces(predictedMove, aiPiece, 0);
                    break;
                }
            }
            else{
                printf("Games over!5\n");
                displayGrid();
                checkGameOver();
                //exit(0);
            }
        }
    }
}

// Gets input from keyboard, then switches turn over to AI
int getInputKeyboard()
{
    int userFirstTime(0);
    displayGrid();
    while(noMovesLeft == 0)
    {
        if(currentTurn == playerPiece)
        {
            char userInput[20]; 
            int userInputX(0), userInputY(0), userOutput(-1), invalidinput(0);
            while((userOutput < 0) || (userOutput > 63)){
                invalidinput++;
                if(invalidinput >= 2){
                    cout << "Invalid input, please try again" << endl;
                }

                if(userFirstTime){
                    cout << "Please input the location you like ex: 21 --> x-axis 2, y-axis 1" << endl;
                }
                else{
                    gameOver(); // Gets scores
                    printf("Current Score player, %d vs %d\n", pointsPlayer, pointsAI);
                }

                cin >> userInput;
                if(strlen(userInput) == 2)
                {
                    userInputX = (userInput[0] - 48); // Decimals start at 48 in ASCII
                    userInputY = (userInput[1] - 48);
                    userOutput = (userInputX + userInputY*(xLength));
                }
            }

            if(flipPieces(userOutput, playerPiece, 1) == 1){
                if(placeValidPiece(userOutput, playerPiece)){
                    flipPieces(userOutput, playerPiece, 0);
                    break;
                }
            }
            else // Error checker
            {
                printf("Oops! -- Screen may not have refreshed\n");
                checkGameOver();
                //exit(0);
            }
            
            //sleep(2);
        }
    }
    return 1;
}

// This version was used for testing the AI
// Gets input from keyboard, then switches turn over to AI
// int getInputKeyboard()
// {
//     //int xAxis(0), yAxis(0), 
//     while(noMovesLeft == 0)
//     {
//         if(currentTurn == playerPiece)
//         {
//             // cout << "Please input the x-axis of the grid you'd like to enter" << endl;
//             // cin >> xAxis;
//             // cout << "Please input the y-axis of the grid you'd like to enter" << endl;
//             // cin >> yAxis;
//             // int theRandomMove = determineNextMoveAiPure(0, playerPiece);
//             int theRandomMove = determineNextMoveAiPure(0, playerPiece);
//             if(flipPieces(theRandomMove, playerPiece, 1) == 1){
//                 if(placeValidPiece(theRandomMove, playerPiece)){
//                     flipPieces(theRandomMove, playerPiece, 0);
//                     break;
//                 }
//             }
//             else // Error checker
//             {
//                 printf("Games over!\n");
//                 displayGrid();
//                 checkGameOver();
//                 //exit(0);
//             } 
//             //sleep(2);
//         }
//     }
//     return 1;
// }

// Sets who starts first
void getStartingPositions()
{
    int input(0);
    // Check if player wants to play first
    if(currentTurn == 0)
    {
        cout << "Would you like to start first? 1 for yes, anything else for no" << endl;
        cin >> input;

        if(input == 1){
            playerPiece = 1;
            aiPiece = 2;
            currentTurn = playerPiece;
        }  
        else{
            playerPiece = 2;
            aiPiece = 1;
            currentTurn = aiPiece;
        }
    }
}

// Automatically determines turns if necessary, else lets player input it
void determineTurn(){
    if(notTestingAI == 1){
        getStartingPositions();
    }
    else{
        if(rand()%2){ // Randomly decides who starts
            playerPiece = 2;
            aiPiece = 1;
            currentTurn = aiPiece;
        }
        else{
            playerPiece = 1;
            aiPiece = 2;
            currentTurn = playerPiece;
        }
    }
}

// Resets everything necessary for a new game
void resetEverything()
{
    createGrid();

    winner = -1;
    pointsAI = 0;
    pointsPlayer = 0;
    winningPiece = 0;

    // Respective game pieces
    playerPiece = 0;
    aiPiece = 0;

    // Respective turns
    currentTurn = 0; // Tracks whose turn it is
    notTestingAI = 0; // temporary

    // Error Checking
    stopTrying = 100000000;
    errorCounter = 0;
    noMovesLeft = 0;    
}


// DEBUG: Ai currently 1, player 2
int playGame()
{
    determineTurn();
    // displayGrid();
    while(noMovesLeft == 0)
    {
        if(currentTurn == playerPiece){
            getInputKeyboard();
            currentTurn = aiPiece;
        }
        else{
            aiTurn();
            currentTurn = playerPiece;
            printf("AI done\n");
        }
        // printf("Next Turn: %d\n", currentTurn);
        // displayGrid();
        checkGameOver();
        printf("\n");
    }
    if(winningPiece == aiPiece){
        printf("AI has won! %d vs %d\n",pointsAI, pointsPlayer);
    }
    else if(winningPiece == playerPiece){
        printf("Player has won! %d vs %d\n",pointsPlayer, pointsAI);
    }
    else{
        printf("It's a draw! %d == %d\n",pointsPlayer, pointsAI);
    }
    // printf("AVG plays per second: %f\n", avgPlayoutsPerSecond);
    gameOver();
    return winningPiece;
}

// Makes the AI play against itself, you have to change aiTurn() and getInputKeyboard() to match
// match the type of input you'd like to play against
void playGameTestHeuristics()
{
    int numberGamesToPlay = 200000000;
    int numberWinsAI(0), numberWinsPlayer(0);
    int pointsAiTotal(0), pointsPlayerTotal(0);
    double avgPlayoutsPerSecondTotal(0);
    int result(0);

    time_t time1, time2;
    double totalTime(0), averageTime(0), newTime(0);
    double maxDifference(0), maxDifferenceHeuristic(0);
    
    // Plays a game numberGamesToPlay times
    for(int k = 0; k < numberGamesToPlay; k++)
    {
        time1 = clock();
        resetEverything(); // Resets game
        result = playGame(); // playGame() returns winning piece
        
        displayGrid();
        pointsPlayerTotal += pointsPlayer;
        pointsAiTotal += pointsAI;

        // Tracks total average playouts
        if(k > 0){
        avgPlayoutsPerSecondTotal = (avgPlayoutsPerSecond + avgPlayoutsPerSecondTotal)/2;
        }
        else{
            avgPlayoutsPerSecondTotal = avgPlayoutsPerSecond;
        }

        time2 = clock();
        newTime = (time2-time1)/double(CLOCKS_PER_SEC);
        totalTime += newTime;
        averageTime = (totalTime)/(k+1);

        if(pointsAI > pointsPlayer){
            numberWinsAI++;
        }
        else if(pointsPlayer > pointsAI){
            numberWinsPlayer++;
        }

        // Tracks location whether there are the biggest differences in points
        if(maxDifference < abs(pointsAI - pointsPlayer)){
            maxDifferenceHeuristic = heuristicWeightMultiplier;
            maxDifference = abs(pointsAI - pointsPlayer);
        }

        printf("%d\n", numberWinsAI);
        printf("%d\n", pointsAiTotal);
        printf("%d\n", numberWinsPlayer);
        printf("%d\n", pointsPlayerTotal);
        printf("%f\n", avgPlayoutsPerSecond);
        printf("%f\n", avgPlayoutsPerSecondTotal);
        cout << "" << fixed << setprecision(15) << heuristicWeightMultiplier << endl;
       // printf("Corner!: %d\n", cornerValue);        printf("%f\n", avgPlayoutsPerSecondTotal);
        printf("%d\n", k+1);
        printf("%f\n", averageTime);
        printf("%f\n", totalTime);
        printf("%f\n\n", maxDifferenceHeuristic);


        // Depending on points, change heuristic
        if(pointsAI > pointsPlayer){
            heuristicWeightMultiplier = (heuristicWeightMultiplier/(1-changeRatio));
        }
        else if(pointsPlayer > pointsAI){
            heuristicWeightMultiplier = (heuristicWeightMultiplier*(1-changeRatio));
        }
    }
}

int main()
{
    system("clear");
    srand(time(0)); // Random seed
    
    cout << "--Begin Main--\n\n" << endl;
    
    createGrid();
    playGame();
    // playGameTestHeuristics();

    cout << "\n\n--End Main--\n" << endl;


    return 0;
}
