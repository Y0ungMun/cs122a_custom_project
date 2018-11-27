/* Name: Brittney Mun, NetID: bmun001, SID: 861295813
 * Custom Project
 *
 * [COMPLETED] Milestone 1: Open and close claw
 * Milestone 2: Move body with a joystick
 */ 

/*
Not entirely sure what F_CPU does, but changing the clock speed also affects how much the claw is able
to open and close. 8 MHz opens too far and doesn't close far enough.
4 MHz is perfect, since it opens quite a bit and closes enough to pick up small objects.
It also doesn't close all the way, just like real-life claw machines, haha ECKS DEE
	*/
#ifndef F_CPU
#define F_CPU 8000000UL		//Changing this changes the range in which the claw opens. Somehow. Readjusted claw location in order to compensate change.
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "timer.c"

//Did not need these variables for Milestone 1

enum lrFSM{lrNeutral, Left, Right}lrState;	//Controls claw base movement.
//enum udFSM{udNeutral, Up, Down}udState;	//Don't need this quite yet.
enum clawFSM{Open, Close}clawState;			//FSM that controls claw.
unsigned char button;						//Button has to be a char, otherwise this no longer works.
unsigned short adcVal;						//ADC value is stored in this variable


//Debugging: PORTB outputs which state a state machine is currently in. Used on lrFunct.

//Function for left and right movement with joystick
//Claw base yeets itself waaaaaay too fast to the left/right. Need to implement an "increasing speed" function or slow down output.
//counter perhaps? Only output every x milliseconds?
void lrFunct(){
	switch(lrState){
		case lrNeutral:											//Neutral state: Do not move motors
			if (adcVal <= 600 && adcVal >= 400)					//Left threshold: 400. Right threshold: 600.
				lrState = lrNeutral;
			else if (adcVal < 400)								//Turn left
				lrState = Left;
			else
				lrState = Right;								//Turn right
			break;
		case Left:												//Turn claw base to the LEFT.
			if (adcVal < 400)									//Keep turning left.
				lrState = Left;
			else if (adcVal <= 600 && adcVal >= 400)			//Stay still.
				lrState = lrNeutral;
			else
				lrState = Right;								//Turn right.
			break;
		case Right:												//Turn claw base to the RIGHT.
			if (adcVal > 600)									//Keep turning Right
				lrState = Right;
			else if (adcVal <= 600 && adcVal >= 400)			//Stay still.
				lrState = lrNeutral;
			else
				lrState = Left;									//Turn left.
			break;
		default:
			lrState = lrNeutral;
			break;
	}
	switch(lrState){
		case lrNeutral:
			//PORTB = 0x01;									//For debugging
			PORTD |= 0xCF;										//Literally do nothing. Sending an empty signal was a bad idea.
			break;												
		case Left:
			//PORTB = 0x02;									//Debugging only
			PORTD |= 0x20;										//Enable PORTD5, OR-mask to ensure other ports are not affected.
			_delay_us(1000);									//Signal enabled for 1ms.
			PORTD &= 0xCF;										//Disable PORTD5.
			_delay_ms(19);										//Signal disabled for 19ms.
			break;
		case Right:
			//PORTB = 0x04;									//yawYEET
			PORTD |= 0x20;										//Enable PORTD5, OR-mask to ensure other ports are not affected.
			_delay_us(2000);									//Signal enabled for 2ms.
			PORTD &= 0xCF;										//Disable PORTD5.
			_delay_ms(18);										//Signal disabled for 18ms.
			break;
		default:
			break;
	}
}
/*
void shiftUD(){
	switch(UDState){
		case InitUD:
			row = 0x1E;
			iterUD = 0;
			UDState = NeutralUD;
			break;
		case NeutralUD:
			if (adcVal <= 600 && adcVal >= 450)					//Stay still
				UDState = NeutralUD;
			else if (adcVal < 450)					//Go left
				UDState = Down;
			else
				UDState = Up;				//Go right
			break;
		case Down:
			if (adcVal < 450)
				UDState = Down;
			else
				UDState = NeutralUD;
			break;
		case Up:
			if (adcVal > 600)
				UDState = Up;
			else
				UDState = NeutralUD;
			break;
		default:
			UDState = InitUD;
			break;
	}	
	switch(UDState){
		case NeutralUD:
			break;
		case Up:
			if (row == upDownArr[0] && iterUD == 0)				//If at left limit
				break;				
			else {
				iterUD--;
				row = upDownArr[iterUD];
			}
			break;
		case Down:
			if (row == upDownArr[4] && iterUD == 4)				//If at right limit
				break;				
			else {
				iterUD++;
				row = upDownArr[iterUD];
			}
			break;
		default:
			break;
	}
}*/

void clawFunct(){
	switch(clawState){
		case Close:								//Close state.
			if (button == 0x01) {
				clawState = Open;
				break;
			}
			clawState = Close;
			break;
		case Open:								//Open state.
			if (button == 0x01) {
				clawState = Open;
				break;
			}
			clawState = Close;
			break;
		default:
			clawState = Close;					//Stay closed by default.
			break;
	}
	switch(clawState){
		case Close:
			PORTD |= 0x40;						//Enable PORTD6, OR-mask to ensure other ports are unaffected.
			_delay_us(1000);						//Close signal up (Duty cycle 5%)
			PORTD &= 0xBF;						//Disable PORTD6
			_delay_ms(18);							//Close signal down (Down = 95%)
			break;
		case Open:
			PORTD |= 0x40;						//Enable PORTD6, OR-mask to ensure other ports are unaffected.
			_delay_us(2000);						//Open signal up (Duty cycle = 10%)
			PORTD &= 0xBF;						//Disable PORTD6
			_delay_ms(19);							//Open signal down (Down = 90%)
			break;
		default:
			break;
	}
}

void A2D_init() {												//From joystick lab. Initializes A2D
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

void Set_A2D_Pin(unsigned char pinNum) {						//From joystick lab. Allows you to change which A2D pin you use.
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); } 
}

//Up/Down on JOYSTICK controls Forward/Back. NOT ACTUALLY UP AND DOWN!!
//To control Up/Down movement: BUTTON PRESS ONLY.
//Upon button press: open claw, lower claw (U/D), close claw, raise claw (U/D)
	//NOT part of milestone 2.

//Ensure that Port D is input
//PWM on Port D, pins 3, 4, 6, 7
	//Timer counts to the length of the period.
	//Once timer reaches a certain point before the end, signal goes up to make duty cycle go up.
	//Stays down before signal is up, goes back down once timer hits end point. Timer is reset to 0.
//50Hz, so 20 ms for the period.
int main(void){
	//DDRB = 0xFF; PORTB = 0x00;		//Uncomment when you need to debug.
    DDRC = 0x00; PORTC = 0xFF;				//Set Port C to input, for button only. Was originally Port A, but Port A is needed for ADC input
	DDRD = 0xFF; PORTD = 0x00;				//Set Port D to output, for all PWM signals to the motors.
												//Be careful with the motors and A2D.
	A2D_init();								//Initialize A2D so joystick can work.
	TimerSet(50);							//Timer function provided by Jeff. Set to 50ms so everything doesn't yeet all over the place.
	TimerOn();								//Enable timer.
	while (1){
		Set_A2D_Pin(0);						//PORTA pin for Left/Right movement
		_delay_ms(1);
		adcVal = ADC;						//Define adcVal
		lrFunct();
		
		button = ~PINC;						//Invert pins for Port A so button can function properly
		clawFunct();
		while(!TimerFlag);
			TimerFlag = 0;
	}
}

