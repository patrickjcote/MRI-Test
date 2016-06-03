//Includes
#include  <msp430.h>

//Defines
#define PWM_PERIOD 20000
#define UP 50
#define DOWN -50
#define LEFT 50
#define RIGHT -50

//Prototypes
void initialize();
void testInput(char inChar);
void messageOut(char messageIn[], int length);
void moveServo(int servoNumber, int direction);

//Global
extern volatile int servoPosition[4];


//robotarm.h
