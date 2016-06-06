// Setup  --------------------------------------------------------------------- 

// Includes
#include "robotarm.h"

// Global Variables
    // Set all servos to neutral position
    volatile int servoPosition[4] = {1500, 1500, 1500, 1500};

//----------------------------------------------------------------------------- 


    // Testing github for CCS
    //test 2

    // Test 3, commit and push from 6

    // Test 4, push from 5, pull from 6
    // Test 5 show bryce

    // Test 7, merge conflicts? (Edit from ccs5)

    // Test 7, merge conflicts, (Edit from 6)

    // Test 9, dont work on same code at the same time

int main(void)
{

    initialize();                   // Initialize registers

	__bis_SR_register(LPM3 + GIE);  // Global Interrupt Enable, Low power mode

}//main()


void initialize(){

	// C

	//Testing the rebase


}//initialize()

//main.c
