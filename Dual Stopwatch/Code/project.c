#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int sec1=0, sec0=0;
unsigned int min1=0, min0=0;
unsigned int hour1=0, hour0=0;
unsigned char pause_flag=0;
unsigned char timer_mode_flag=1;
//timer mode flag states counting up
unsigned char ISR_countUp_flag=0;
unsigned char ISR_countDown_flag=0;

void show_segments(void){

	/*Function to show all 7 segments simultaneously it isn't dependent on timer mode
	illusion done by enabling one segment after the other with 5ms delay between */

	//second units
	PORTA|=(1<<0);
	PORTC=(PORTC&0xF0)|(sec0&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F; //e2fely all

	//seconds tens
	PORTA|=(1<<1);
	PORTC=(PORTC&0xF0)|(sec1&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F;

	//minutes units
	PORTA|=(1<<2);
	PORTC=(PORTC&0xF0)|(min0&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F;

	//minutes tens
	PORTA|=(1<<3);
	PORTC=(PORTC&0xF0)|(min1&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F;

	//hours units
	PORTA|=(1<<4);
	PORTC=(PORTC&0xF0)|(hour0&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F;

	//hours tens
	PORTA|=(1<<5);
	PORTC=(PORTC&0xF0)|(hour1&0x0F);
	_delay_ms(5);
	PORTA&=~0x3F;

}

void Timer_countUp(void){
	//mode 1: timer/counting up
	if(sec0!=9)
	{
		sec0++;
	}
	else if(sec0==9 && sec1!=5)
	{
		sec1++;
		sec0=0;
	}
	else if(sec0==9 && sec1==5)
	{
		sec0=0;
		sec1=0;

		//full second passed, check mins
		if(min0!=9)
		{
			min0++;
		}
		else if(min0==9 &&min1!=5)
		{
			min0=0;
			min1++;
		}
		else if(min0==9 && min1==5)
		{
			min0=0;
			min1=0;

			//full min (and second) passed, check hours
			if(hour0!=9)
			{
				hour0++;
			}
			else if(hour0==9 && hour1!=5)
			{
				hour0=0;
				hour1++;
			}
			else if(hour0==9 &&hour1==5)
			{
				hour0=0;
				hour1=0;
				//keda segment all values equal 0, timer will count from the start.
			}
		}
	}
}

void Timer_countDown(void){
	//mode 2: counting down
	if(sec0!=0)
	{
		sec0--;
	}
	else if(sec0==0 && sec1!=0)
	{
		sec0=9;
		sec1--;
	}
	else if(sec0==0 && sec1==0)  //check if mins exist, if not check hours
	{
		sec0=9;
		sec1=5;

		//check minutes
		if(min0!=0)
		{
			min0--;
		}
		else if(min0==0 && min1!=0)
		{
			min0=9;
			min1--;
		}
		else if(min0==0 && min1==0)   //keda both seconds and mins are zeroed
		{
			min0=9;
			min1=5;

			if(hour0!=0)
			{
				hour0--;
			}
			else if(hour0==0 && hour1!=0)
			{
				hour0=9;
				hour1--;
			}
			else if(hour0==0 && hour1==0)
			{
				//no more minutes and hours to deduct, so stop seconds and minutes at 00
				sec0=0;
				sec1=0;
				min0=0;
				min1=0;

				//trigger the buzzer
				PORTD|=(1<<0);
			}
		}

	}

}

//setting up Timer1, Fcpu=16Mhz, prescaler=1024
void Timer1_init(void){
	TCCR1A=(1<<FOC1A);
	TCCR1B=(1<<CS10)|(1<<CS12)|(1<<WGM12)|(1<<WGM13);
	TCNT1=0;
	ICR1=15625;
	TIMSK|=(1<<OCIE1A);
}

//An interrupt is generated each second.
ISR(TIMER1_COMPA_vect){

	if(timer_mode_flag)
	{
		ISR_countUp_flag=1;
	}
	else if(!timer_mode_flag)
	{
		ISR_countDown_flag=1;
	}

}

//set up timer resetting interrupt
void INT0_reset_init(void){
	GICR|=(1<<INT0);
	MCUCR|=(1<<ISC01);
}

ISR(INT0_vect){
	//reset timer values
	sec0=0;
	sec1=0;
	min0=0;
	min1=0;
	hour0=0;
	hour1=0;
}

//set up timer pause interrupt
void INT1_pause_init(void){
	GICR|=(1<<INT1);
	MCUCR|=(1<<ISC11)|(1<<ISC10);
}

ISR(INT1_vect){
	//disable timer from clk select table
	_delay_ms(30);
	if(PIND&(1<<3))
	{
		TCCR1B&=~(1<<CS12)&~(1<<CS10);
		pause_flag=1;
	}

	//turn off buzzer
	PORTD&=~(1<<0);
}

//set up timer resuming interrupt
void INT2_resume_init(void){
	GICR|=(1<<INT2);
	MCUCSR&=~(1<<ISC2);
}

ISR(INT2_vect){
	pause_flag=0;
	//resume timer
	TCCR1B|=(1<<CS12)|(1<<CS10);
}


int main(void){

	//first 4 PORTC pins o/p into decoder, initially set to zero
	DDRC|=0x0F;
	PORTC&=0xF0;

	//first 6 pins o/p for 7 segment enable/disable pins, initially enable all
	DDRA|=0x3F;
	PORTA|=0x3F;

	//push button takes input trigger for pause interrupt (INT1 at PD3)
	DDRD&=~(1<<PD3);

	//0->7 pins in PORTB are push button inputs with internal pullup
	DDRB=0x00;
	PORTB=0xFF;

	//push button for reset interrupt (INT0 at PD2)
	DDRD&=~(1<<PD2);
	PORTD|=(1<<PD2);

	//2 LEDs describing mode at PD4,PD5
	DDRD|=0x30;
	PORTD&=~0x30;

	//buzzer alarm, initially silent
	DDRD|=(1<<0);
	PORTD&=~(1<<0);

	//Initialise timer1 and interrupts
	Timer1_init();
	INT0_reset_init();
	INT1_pause_init();
	INT2_resume_init();

	//enable global interrupt pin for timer1 and interrupts
	SREG|=(1<<7);


	while(1)
	{
		//Function to display all 7 segments at once
		show_segments();

		if(ISR_countUp_flag)
		{
			Timer_countUp();
			ISR_countUp_flag=0;
		}

		if(ISR_countDown_flag)
		{
			Timer_countDown();
			ISR_countDown_flag=0;
		}

		//toggle mode button, timer must be paused
		if((!(PINB&(1<<7))) && pause_flag)
		{
			timer_mode_flag^=1;  //mode flag toggled
			while(!(PINB&(1<<7)))
			{
				show_segments();
			}
		}

		//TIMER mode red LED
		if(timer_mode_flag==1)
		{
			PORTD|=(1<<4);
			PORTD&=~(1<<5);
		}

		//COUNTDOWN mode yellow LED
		if(timer_mode_flag==0)
		{
			PORTD|=(1<<5);
			PORTD&=~(1<<4);
		}


		//All manual timer adjustments.
		//Timer must be paused while adjusting.


		//decrementing hours PB0
		if((!(PINB&(1<<0))) && pause_flag)
		{
			if(hour0!=0)
			{
				hour0--;
			}
			else if(hour0==0 && hour1!=0)
			{
				hour0=9;
				hour1--;
			}
			else if(hour0==0 && hour1==0)
			{
				hour0=9;
				hour1=5;
			}

			while(!(PINB&(1<<0)))
			{
				show_segments();
			}

		}


		//incrementing hours PB1
		if((!(PINB&(1<<1))) && pause_flag)
		{

			if(hour0!=9)
			{
				hour0++;
			}
			else if(hour0==9 && hour1!=5)
			{
				hour0=0;
				hour1++;
			}
			else
			{
				hour0=0;
				hour1=0;
			}

			while(!(PINB&(1<<1)))
			{
				show_segments();
			}
		}


		//decrementing minutes PB3
		if((!(PINB&(1<<3))) && pause_flag)
		{
			if(min0!=0)
			{
				min0--;
			}
			else if(min0==0 && min1!=0)
			{
				min0=9;
				min1--;
			}
			else if(min0==0 && min1==0)
			{
				min0=9;
				min1=5;
			}

			while(!(PINB&(1<<3)))
			{
				show_segments();
			}
		}


		//incrementing minutes PB4
		if((!(PINB&(1<<4))) && pause_flag)
		{
			if(min0!=9)
			{
				min0++;
			}
			else if(min0==9 && min1!=5)
			{
				min0=0;
				min1++;
			}
			else
			{
				min0=0;
				min1=0;
			}

			while(!(PINB&(1<<4)))
			{
				show_segments();
			}
		}


		//decrementing seconds PB5
		if((!(PINB&(1<<5))) && pause_flag)
		{
			if(sec0!=0)
			{
				sec0--;
			}
			else if(sec0==0 && sec1!=0)
			{
				sec0=9;
				sec1--;
			}
			else if(sec0==0 && sec1==0)
			{
				sec0=9;
				sec1=5;
			}

			while(!(PINB&(1<<5)))
			{
				show_segments();
			}
		}


		//incrementing seconds PB6
		if((!(PINB&(1<<6))) && pause_flag)
		{
			if(sec0!=9)
			{
				sec0++;
			}
			else if(sec0==9 && sec1!=5)
			{
				sec0=0;
				sec1++;
			}
			else
			{
				sec0=0;
				sec1=0;
			}

			while(!(PINB&(1<<6)))
			{
				show_segments();
			}
		}


	}

	return 0;
}








