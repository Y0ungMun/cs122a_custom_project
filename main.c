/* Name: Brittney Mun, NetID: bmun001, SID: 861295813
 * Custom Project
 * Milestone 1: Open and close claw
 */ 

/*
Not entirely sure what F_CPU does, but changing the clock speed also affects how much the claw is able
to open and close. 8 MHz opens too far and doesn't close far enough.
4 MHz is perfect, since it opens quite a bit and closes enough to pick up small objects.
It also doesn't close all the way, just like real-life claw machines, haha ECKS DEE
	*/
#ifndef F_CPU
#define F_CPU 4000000UL		//4 MHz clock speed. Changing this changes the range in which the claw opens. Somehow.
#endif

#include <avr/io.h>
#include <util/delay.h>
//#include "timer.c"			Not needed for now.

//Did not need these variables for Milestone 1
/*
//enum pwmOpen{Init, Disable, On, Off}openState;
//enum pwmClose{Init2, Disable2, On2, Off2}closeState;
enum clawFSM{Open, Close}clawState;
unsigned char button;
unsigned char count;
unsigned char count2;
unsigned char pwmVal;
unsigned char flag;		//1 for Open, 0 for close
*/

//Debugging: PORTC displays which PWM signal is being sent.

//Runs without needing user input.
//2 PWM FSMs. One is always enabled when one is disabled.
//Open PWM signal, duty cycle = 5%
//PORTC = 0x01 when disabled
	//Debugging LED to verify correct PWM State machine was entered.
/*
void pwmOpenFunct(){
	switch(openState){
		case Init:
			count = 0;
			openState = Disable;
			break;
		case Disable:
			if (flag == 1){
				openState = On;
				break;
			}
			openState = Disable;
			break;
		case On:
			//Immediately stop if flag to close is on.
			if (flag == 0){
				openState = Disable;
				break;
			}
			if (count > 1) {		//On for 1ms
				openState = Off;
				break;
			}
			openState = On;
			break;
		case Off:
			//Immediately stop if flag to close is on.
			if (flag == 0){
				openState = Disable;
				break;
			}
			if (count > 20){		//Off for 19ms
				count = 0;
				openState = On;
				break;
			}
			openState = Off;
			break;
		default:
			openState = Init;
			break;
	}
	switch(openState){
		case Init:
			count = 0;
			break;
		case Disable:
			PORTC = 0x01;
			count = 0;
			break;
		case On:
			pwmVal = 0x08;		//count faster depending on clawState. x2 on On state only when opening
			count++;
			break;
		case Off:
			pwmVal = 0x00;
			count++;
			break;
		default:
			break;
	}
}
*/
//Closing PWM signal, Duty cycle = 10%
//PORTC = 0x02 when disabled
	//Debugging LED to check PWM state machines
/*
void pwmCloseFunct(){
	switch(closeState){
		case Init2:
			count2 = 0;
			closeState = Disable2;//change to Disabled by default
			break;
		case Disable2:
			//Immediately stop if flag to close is on.
			if (flag == 0){
				closeState = On2;
				break;
			}
			closeState = Disable2;
			break;
		case On2:
			//Immediately stop if flag to close is on.
			if (flag == 1){
				closeState = Disable2;
				break;
			}
			if (count2 > 2) {		//On for 2ms
				closeState = Off2;
				break;
			}
			closeState = On2;
			break;
		case Off2:
			//Immediately stop if flag to close is on.
			if (flag == 1){
				closeState = Disable2;
				break;
			}
			if (count2 > 20){		//Off for 18ms
				count2 = 0;
				closeState = On2;
				break;
			}
			closeState = Off2;
			break;
		default:
			closeState = Init2;
			break;
	}
	switch(closeState){
		case Init2:
			count2 = 0;
			break;
		case Disable2:
			PORTC = 0x02;
			count2 = 0;
			break;
		case On2:
			pwmVal = 0x08;		//count faster depending on clawState. x2 on On state only when opening
			count2++;
			break;
		case Off2:
			pwmVal = 0x00;
			break;
		default:
			break;
	}
}
*/
//Tried to make 2 PWM state machines and 1 claw state machine, but was unable to close claw properly.
/*
void clawFunct(){
	switch(clawState){
		case Close:
			if (button == 0x01) {
				clawState = Open;
				break;
			}
			clawState = Close;
			break;
		case Open:
			if (button == 0x01) {
				clawState = Open;
				break;
			}
			clawState = Close;
			break;
		default:
			clawState = Close;
			break;
	}
	switch(clawState){
		case Close:
			PORTB = pwmVal;
			flag = 0;
			break;
		case Open:
			PORTB = pwmVal;
			flag = 1;
			break;
		default:
			break;
	}
}
*/

//Ensure that Port D is input
//PWM on Port D, pins 3, 4, 6, 7
	//Timer counts to the length of the period.
	//Once timer reaches a certain point before the end, signal goes up to make duty cycle go up.
	//Stays down before signal is up, goes back down once timer hits end point. Timer is reset to 0.
//50Hz, so 20 ms for the period.
int main(void){
    DDRA = 0x00; PORTA = 0xFF;				//Set Port A to input, for button only.
	DDRD = 0xFF; PORTD = 0x00;				//Set Port D to output, for all PWM signals to the motors.
											//Be careful with the motors and ADC.
	unsigned char button;					//Button has to be a char, otherwise this no longer works.
	while (1){
		button = ~PINA;						//Invert pins for Port A so button can function properly
		if (button == 0x01) {					//When button is pressed
			PORTD = 0x40;						//Enable PORTD6
			_delay_us(2000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(19);							//Open signal down (Down = 90%)
		} else {								//When button is not pressed
			PORTD = 0x40;						//Enable PORTD6
			_delay_us(1000);						//Close signal up (Duty cycle 5%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(18);							//Close signal down (Down = 95%)
		}
	}
}

