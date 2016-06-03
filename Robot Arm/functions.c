#include "robotarm.h"

void moveServo(int servoNumber, int direction){

	servoPosition[servoNumber] += direction;

	if(direction == UP && servoPosition[servoNumber] > 2000)
		servoPosition[servoNumber] = 2000; // Max 10% Duty Cycle
	if(direction == DOWN && servoPosition[servoNumber] < 1000)
		servoPosition[servoNumber] = 1000; // Min 5% Duty Cycle

}//moveServo()

void testInput(char inChar){

	switch(inChar){
	case 'w': //Arm forward
		moveServo(0, UP);
		break;
	case 's': //Arm back
		moveServo(0,DOWN);
		break;
	case 'd': //Rotate CCW
		moveServo(1, LEFT);
		break;
	case 'a': //Rotate CW
		moveServo(1, RIGHT);
		break;

	case 'i': //Arm up
		moveServo(2, UP);
		break;
	case 'k': //Arm down
		moveServo(2, DOWN);
		break;
	case 'l': //Close claw
		moveServo(3, UP);
		break;
	case 'j': //Open claw
		moveServo(3, DOWN);
		break;

	case 'r': //Reset position
		servoPosition[0] = 1500;
		servoPosition[1] = 1500;
		servoPosition[2] = 1500;
		servoPosition[3] = 1500;
		break;

	default:  //Not a valid Key, Display possible keys
		messageOut("----- Invalid Key -----",23);
		messageOut("Left/Right: 'a' + 'd'",21);
		messageOut("Up/Down: 'i' + 'k'",18);
		messageOut("Forward/Back: 'w' + 's'",23);
		messageOut("Open/Close: 'l' + 'j'",21);
		messageOut("Reset Position: 'r'",19);
		messageOut("",0); //Newline
		break;
	}//switch

}//testInput()

void messageOut(char messageIn[], int length){

	volatile int i;
	for(i = 0; i < length; i++){	// Loop through each character
		while (!(IFG2&UCA0TXIFG));  // USCI_A0 TX buffer ready?
		UCA0TXBUF = messageIn[i];	// Send character
	}//for

	while (!(IFG2&UCA0TXIFG));      // USCI_A0 TX buffer ready?
	UCA0TXBUF = 0x0A;				// Send Line feed
	while (!(IFG2&UCA0TXIFG));      // USCI_A0 TX buffer ready?
	UCA0TXBUF = 0x0D;				// Send Carriage return

}//outputMessage()

