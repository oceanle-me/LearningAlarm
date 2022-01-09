/*
 * learning_alarm.c
 *
 * Created: 08/01/2022 9:18:35 CH
 * Author : OceanLe
 * Email: oceanle.me@gmail.com
 */ 
//***IMPORTANT NOTES***//
/*	Just learn smartly, not only hardly.*/

/*	Port D2 is audio out, that sound is 1KHz and in 100ms
*	After "PERIOD_LEARNING" minutes learning,
*	our arduino will repeat alarm infinitely
*	each "PERIOD_ALARM_REPEAT" minutes until reset system,
*	just press reset button
*/ 

//***IMPORTANT DEFINES***//
#define PERIOD_LEARNING 20//minutes
#define PERIOD_ALARM_REPEAT 3 //minutes
#define F_CPU 16000000


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void RepeatAlarm(void);


uint32_t time_learn = 0;

int main(void){
//	DDRB=0xFF;          //PORTB la output PORT
//	PORTB=0x00;
	DDRD=0b100; //D2 is output

	TCCR2A=0; //no compare mode
	TIMSK2=(1<<TOIE2);//timer2 overflow interrupt enable
	TCCR2B=(1<<CS22)|(1<<CS21)|(1<<CS20); //dividing 1024 from pre-scaler,
	//so with 8bit, timer2 overflows after 1/(16M/(1024*256))s
	sei(); 
	while (1); //do nothing
	return 0;
}


ISR (TIMER2_OVF_vect ){
	SMCR = 0;//disable sleep mode
	if (time_learn < (73240)){ //3662= (60*16e6) / (1024*256)
		//PORTB^=0xff; //reverse B2 LED
		++time_learn;
		SMCR= (1<<SM1)|(1<<SM0)|(1<<SE);//go to power-save mode, wake up with timer 2 overflows
	}
	else {

		TCCR2B =0;//stop timer2
		RepeatAlarm();//repeat infinitely until reset
	}

}

void RepeatAlarm(void){
	uint32_t  counter_In1_Alarm =0; //ms
	uint32_t  counter_In2_Alarm =0;

	while(1){//repeat infinitely until reset
	if (counter_In1_Alarm  < 1000){
		if ( counter_In2_Alarm <100){
			PORTD^=0b100; //reverse B2
		}
		else if (counter_In2_Alarm <1000){
			PORTD=0b000;
		}
		else{
			counter_In2_Alarm=0;
		}
		counter_In2_Alarm++;
	}
	else if (counter_In1_Alarm  < (180000))/*ms*/{
		PORTD=0b000;
	}
	else{
		counter_In1_Alarm=0;
		counter_In2_Alarm=0;
	}
	counter_In1_Alarm++;
	_delay_ms(1);
	}
}