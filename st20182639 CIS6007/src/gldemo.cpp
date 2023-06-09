//
// Array Visualisation for CIS6007 WRIT1
//

#include <iostream>
#include <string>
#include <random>
#include "gl_stuff.h"
#include "Array2D.h"
#include <array>
#include <tbb/tbb.h>
#include<thread>
#include<chrono> 



using namespace std;
using namespace tbb;


#pragma region Function Prototypes

void initPattern(Array2D<int>& myArray); // Initialise grid with "alive" and "dead" cell patterns
void update(void); // Main update function - apply Game of Life rules here

#pragma endregion


//
// Array Size
//
Array2D<int>						
myArray = Array2D<int>(64,64);

// Starting point for our program...
int main(int argc, char* argv[])
{
	// Initialise OpenGL window...
	gl_init(argc, argv, update, "CIS6007 WRIT1 - Game of Life Demo", true);

	// Initialise our Game of Life starting pattern...
	initPattern(myArray);

	// Start the main loop - this calls our update function (below) to update our game of life generations.
	// Note: We never return from glutMainLoop until we quit the application
	glutMainLoop();


    return 0;
}




// Initialise grid with "alive" and "dead" cell patterns
void initPattern(Array2D<int>& myArray) {

    // 1. First clear the array (set all cells to "dead" ie. 0)
    parallel_for(
        0,
        myArray.w * myArray.h,
        [&](int i) {

            int x = i % myArray.w;
    int y = i / myArray.h;
    myArray(x, y) = 0;
        }
    );

    // Setup some cells to be "alive" - this is the initial state
    // NOTE: *** REPLACE THIS WITH YOUR OWN CODE TO SETUP THE LIVE CELL PATTERNS

    // random number generator
    random_device rd;
    mt19937 engine(rd());

    // Randomly fill in x% of the empty cells
    for (int x = 0; x < myArray.w; x++) {
        for (int y = 0; y < myArray.h; y++) {
            if (myArray(x, y) == 0) {
                if (uniform_int_distribution<int>(0, 99)(engine) < 50){
                    myArray(x, y) = 1;
                }
            }
        }
    
    }
}


    











// Note: This is called by FreeGLUT's main event loop - YOU DON'T NEED TO CALL THIS - IT'S CALLED FOR YOU!!!
// Main update function - apply Game of Life rules here


int getliveCellCount(Array2D<int>& myArray, int x, int y) {
    int liveCellCount = 0;

    // Check the 8 sides of the cell at (x, y)
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            // Skip the current cell
            if (i == x && j == y) continue;

            // Check if the cell is within the bounds of the array
            if (i >= 0 && i < myArray.w && j >= 0 && j < myArray.h) {
                // Increment the count if the cell is alive
                if (myArray(i, j) == 1) {
                    liveCellCount++;
                }
            }
        }
    
    }

    return liveCellCount;
}

double cumulativeTime = 0;
int iterationCount = 0;

void printAverageTime() {
    double averageTime = cumulativeTime / iterationCount;
    cout << "Average time taken after " << iterationCount << " iterations = " << averageTime << " seconds" << endl;
}


void update(void) {

    // Inside update function, after updating the cumulativeTime and iterationCount

    //if (iterationCount > 1000) {
    //    return;
    //}

    if (iterationCount % 100 == 0) {
        printAverageTime();
    }



    // Temporary array to hold the next generation
    Array2D<int> newGen = Array2D<int>(myArray.w, myArray.h);



    // Apply the Game of Life rules to each cell in the grid

    //Seqeuntial Version

    auto startTime = chrono::system_clock::now();


    // Apply the Game of Life rules to each cell in the grid
    for (int x = 0; x < myArray.w; x++) {
        for (int y = 0; y < myArray.h; y++) {
            int liveCellCount = getliveCellCount(myArray, x, y);

            // Any live cell with fewer than two or more than three live neighbours dies
            if ((myArray(x, y) == 1 && liveCellCount < 2) || (myArray(x, y) == 1 && liveCellCount > 3)) {
                newGen(x, y) = 0;
            }
            // Any live cell with two or three live neighbours lives on to the next generation,
            // and any dead cell with exactly three live neighbours becomes a live cell
            else if ((myArray(x, y) == 1 && (liveCellCount == 2 || liveCellCount == 3)) || (myArray(x, y) == 0 && liveCellCount == 3)) {
                newGen(x, y) = 1;
            }
            // Otherwise, the cell stays the same
            else {
                newGen(x, y) = myArray(x, y);
            }
        }
    }


        auto endTime = chrono::system_clock::now();


        // Copy the new generation to the current generation
        myArray = newGen;




        chrono::duration<double> duration = endTime - startTime;
        /* cout << "Sequential Version: Time taken = " << duration.count() << " seconds" << endl;*/


        // Inside update function, after calculating the duration
        cumulativeTime += duration.count();
        iterationCount++;





    ////Parallel For Version
    // 
    //    auto startTime = chrono::system_clock::now();
    // 
    //parallel_for(
    //    0,
    //    myArray.w * myArray.h,
    //    [&](int i) {

    //        int x = i % myArray.w;
    //int y = i / myArray.h;

    //int liveCellCount = getliveCellCount(myArray, x, y);

    //// Any live cell with fewer than two or more than three live neighbours dies
    //if ((myArray(x, y) == 1 && liveCellCount < 2) || (myArray(x, y) == 1 && liveCellCount > 3)) {
    //    newGen(x, y) = 0;
    //}
    //// Any live cell with two or three live neighbours lives on to the next generation,
    //// and any dead cell with exactly three live neighbours becomes a live cell
    //else if ((myArray(x, y) == 1 && (liveCellCount == 2 || liveCellCount == 3)) || (myArray(x, y) == 0 && liveCellCount == 3)) {
    //    newGen(x, y) = 1;
    //}
    //// Otherwise, the cell stays the same
    //else {
    //    newGen(x, y) = myArray(x, y);
    //}
    //    }
    //);
    //     auto endTime = chrono::system_clock::now();


    //     //Copy the new generation to the current generation
    //    myArray = newGen;




    //    chrono::duration<double> duration = endTime - startTime;


    //    // Inside update function, after calculating the duration
    //    cumulativeTime += duration.count();
    //    iterationCount++;





    // Redraw the screen to show latest generation

    renderLatestGeneration(myArray);





    //this_thread::sleep_for(chrono::milliseconds(1000));



}


