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
enum fbFSM{fbNeutral, Forward, Back}fbState;	//Forward/back movement for claw.
enum clawFSM{Neutral, Open, Lower, Close, Raise, MoveToDrop, Drop, Finish}clawState;			//FSM that controls claw.
unsigned char button;						//Button has to be a char, otherwise this no longer works.
unsigned short adcVal;						//ADC value is stored in this variable
unsigned char count;

//Debugging: PORTB outputs which state a state machine is currently in. Used on lrFunct.

//Function for left and right movement with joystick
//Claw base yeets itself waaaaaay too fast to the left/right. That's just how servos be sometimes.
void lrFunct(){
	switch(lrState){
		case lrNeutral:											//Neutral state: Do not move motors
			if (adcVal <= 600 && adcVal >= 450)					//Left threshold: 450. Right threshold: 600.
				lrState = lrNeutral;
			else if (adcVal < 450)								//Turn left
				lrState = Left;
			else
				lrState = Right;								//Turn right
			break;
		case Left:												//Turn claw base to the LEFT.
			if (adcVal < 450)									//Keep turning left.
				lrState = Left;
			else if (adcVal <= 600 && adcVal >= 450)			//Stay still.
				lrState = lrNeutral;
			else
				lrState = Right;								//Turn right.
			break;
		case Right:												//Turn claw base to the RIGHT.
			if (adcVal > 600)									//Keep turning Right
				lrState = Right;
			else if (adcVal <= 600 && adcVal >= 450)			//Stay still.
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
			PORTD &= 0xCF;										//Literally do nothing. Sending an empty signal was a bad idea.
			break;												
		case Left:
			//PORTB = 0x02;									//Debugging only
			PORTD |= 0x20;										//Enable PORTD5, OR-mask to ensure other ports are not affected.
			_delay_us(2000);									//Signal enabled for 1ms.
			PORTD &= 0xCF;										//Disable PORTD5.
			_delay_ms(18);										//Signal disabled for 19ms.
			break;
		case Right:
			//PORTB = 0x04;									//yawYEET
			PORTD |= 0x20;										//Enable PORTD5, OR-mask to ensure other ports are not affected.
			_delay_us(1000);									//Signal enabled for 2ms.
			PORTD &= 0xCF;										//Disable PORTD5.
			_delay_ms(19);										//Signal disabled for 18ms.
			break;
		default:
			break;
	}
}

void fbFunct(){
	switch(fbState){
		case fbNeutral:											//Neutral state: Do not move motors
			if (adcVal <= 600 && adcVal >= 450)					//Left threshold: 450. Right threshold: 600.
				fbState = fbNeutral;
			else if (adcVal < 450)								//Turn left
				fbState = Forward;
			else
				fbState = Back;								//Turn right
			break;
		case Forward:												//Turn claw base to the LEFT.
			if (adcVal < 450)									//Keep turning left.
				fbState = Forward;
			else if (adcVal <= 600 && adcVal >= 450)			//Stay still.
				fbState = fbNeutral;
			else
				fbState = Back;								//Turn right.
			break;
		case Back:												//Turn claw base to the RIGHT.
			if (adcVal > 600)									//Keep turning Right
				fbState = Back;
			else if (adcVal <= 600 && adcVal >= 450)			//Stay still.
				fbState = fbNeutral;
			else
				fbState = Forward;									//Turn left.
			break;
		default:
			fbState = fbNeutral;
			break;
	}
	switch(fbState){
		case fbNeutral:
			//PORTB = 0x01;									//For debugging
			PORTD &= 0xF7;										//Forward and back use PORTD3.
			break;												
		case Forward:
			//PORTB = 0x02;									//Debugging only
			PORTD |= 0x08;										//Enable PORTD3, OR-mask to ensure other ports are not affected.
			_delay_us(1000);									//Signal enabled for 1ms.
			PORTD &= 0xF7;										//Disable PORTD3.
			_delay_ms(19);										//Signal disabled for 19ms.
			break;
		case Back:
			//PORTB = 0x04;									//yawYEET
			PORTD |= 0x08;										//Enable PORTD3, OR-mask to ensure other ports are not affected.
			_delay_us(2000);									//Signal enabled for 2ms.
			PORTD &= 0xF7;										//Disable PORTD3.
			_delay_ms(18);										//Signal disabled for 18ms.
			break;
		default:
			break;
	}
}

//ClawFSM function.
//Neutral, Open, Lower, Close, Raise, MoveToDrop, Drop
void clawFunct(){
	switch(clawState){
		case Neutral:
			if (button == 0x01){
				clawState = Open;
				break;
			}
			clawState = Neutral;
			break;
		case Open:								//Open state.
			if (count >= 10){
				count = 0;
				clawState = Lower;
				break;
			} else {
				clawState = Open;
				count++;
			}
			break;
		case Lower:
			if (count >= 10){
				count = 0;
				clawState = Close;
				break;
			} else {
				clawState = Lower;
				count++;
			}
			break;
		case Close:								//Close state.
			if (count >= 10){
				count = 0;
				clawState = Raise;
				break;
			} else {
				clawState = Close;
				count++;
			}
			break;
		case Raise:
			if (count >= 10){
				count = 0;
				clawState = MoveToDrop;
				break;
			} else {
				clawState = Raise;
				count++;
			}
			break;
		case MoveToDrop:
			if (count >= 10){
				count = 0;
				clawState = Drop;
				break;
			} else {
				clawState = MoveToDrop;
				count++;
			}
			break;
		case Drop:
			if (count >= 10){
				count = 0;
				clawState = Finish;
				break;
			} else {
				clawState = Drop;
				count++;
			}
			break;
		case Finish:
			if (count >= 10){
				count = 0;
				clawState = Neutral;
				break;
			} else {
				clawState = Finish;
				count++;
			}
			break;
		default:
			clawState = Neutral;					//Stay closed by default.
			break;
			
	}
	switch(clawState){
		case Neutral:
			count = 0;
			break;
		case Open:
			PORTD = 0x40;						//Enable PORTD6, GRIPPER ONLY
			_delay_us(2000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(18);							//Open signal down (Down = 90%)
			break;
		case Lower:
			PORTD = 0x10;						//Enable PORTD4, UP/DOWN ONLY
			_delay_us(1000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(19);							//Open signal down (Down = 90%)
			break;
		case Close:
			PORTD = 0x40;						//Enable PORTD6, GRIPPER ONLY
			_delay_us(1000);						//Close signal up (Duty cycle 5%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(19);							//Close signal down (Down = 95%)
			break;
		case Raise:
			PORTD = 0x10;						//Enable PORTD4, UP/DOWN ONLY
			_delay_us(2000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(18);							//Open signal down (Down = 90%)
			break;
		case MoveToDrop:
			PORTD = 0x20;						//Enable PORTD5, LEFT/RIGHT ONLY
			_delay_us(1000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(19);							//Open signal down (Down = 90%)
			break;
		case Drop:
			PORTD = 0x40;						//Enable PORTD6, GRIPPER ONLY
			_delay_us(2000);						//Open signal up (Duty cycle = 10%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(18);							//Open signal down (Down = 90%)
			break;
		case Finish:
			PORTD = 0x40;						//Enable PORTD6, GRIPPER ONLY
			_delay_us(1000);						//Close signal up (Duty cycle 5%)
			PORTD = 0x00;						//Disable PORTD6
			_delay_ms(18);							//Close signal down (Down = 95%)
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
int main(void){
	//DDRB = 0xFF; PORTB = 0x00;		//Uncomment when you need to debug.
    DDRC = 0x00; PORTC = 0xFF;				//Set Port C to input, for button only. Was originally Port A, but Port A is needed for ADC input
	DDRD = 0xFF; PORTD = 0x00;				//Set Port D to output, for all PWM signals to the motors.
	
	A2D_init();								//Initialize A2D so joystick can work.
	TimerSet(50);							//Timer function provided by Jeff. Set to 50ms so everything doesn't yeet all over the place.
	TimerOn();								//Enable timer.
	while (1){
		button = ~PINC;						//Invert pins for Port A so button can function properly
		if (button == 0x01)
			do{
				clawFunct();						//Performs claw-grabbing motion.
			} while (clawState != Neutral);
		else {
		Set_A2D_Pin(0);						//PORTA pin for Left/Right movement
		_delay_ms(1);						//Delay for switching ADC ports
		adcVal = ADC;						//Define adcVal
		lrFunct();
		
		Set_A2D_Pin(1);						//PORTA pin for Forward/Back movement
		_delay_ms(1);
		adcVal = ADC;						//Define adcVal
		fbFunct();
		}
		while(!TimerFlag);
			TimerFlag = 0;
	}
}

