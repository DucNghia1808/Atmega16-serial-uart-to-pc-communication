
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "lib/twi_lcd.h"
#define tbi(port,bit){port^=(1<<bit);} // toggle
#define sbi(port,bit){port|=(1<<bit);}//set
#define cbi(port,bit){port&=~(1<<bit);}//clear
#define DHT11_PIN 6

uint8_t c=0,I_RH=0,D_RH=0,I_Temp=0,D_Temp=0,CheckSum; // dung cho DHT11
int NDlast=0,DAlast=0; // bien cuoi ND DA
char data[16]; // chuoi gui du lieu ra LCD
unsigned long last = 0;
unsigned long dem = 0;
char data1[3];
char data2[3];

unsigned char Receive(){
	while ((UCSRA & (1 << RXC)) == 0);
	return(UDR);
}
void Send(char data){
	while (! (UCSRA & (1<<UDRE)));
	UDR = data ;
}
void SendString(char *str){
	unsigned char i=0;
	
	while (str[i]!=0){		// Send string till null
		Send(str[i]);
		i++;
	}
}
void UARTinit(){
	UBRRH=0;
	UBRRL=51;// set baud rate
	UCSRC=(1<<URSEL)|(1<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0); // on mode use UBRRH, mode send 8bit
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);// enable tran enable receive
}
void Request()				//Microcontroller send start pulse/request  DHT 11
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	// set to low pin
	_delay_ms(20);			// wait for 20ms
	PORTD |= (1<<DHT11_PIN);	// set to high pin
	_delay_us(40);
}

void Response(){				// receive response from DHT11 and start trans bit
	DDRD &= ~(1<<DHT11_PIN); // chuyen thanh input chan PD6
	while(PIND & (1<<DHT11_PIN));// neu bang 1 dung
	while((PIND & (1<<DHT11_PIN))==0);// neu bang 0 dung (khoang 80us)
	while(PIND & (1<<DHT11_PIN));// neu bang 1 dung (khoang 80us)
	// bat dau gui...
}

uint8_t Receive_data(){			//receive data
	for (int i=0; i<8; i++){
		while((PIND & (1<<DHT11_PIN)) == 0);  //check received bit 0 or 1
		// giu o muc thap khi bat dau gui 1 bit nao do
		_delay_us(30); // sau 26-> 28 us kiem tra  1 (70us) hay 0
		if(PIND & (1<<DHT11_PIN))// if high pulse is greater than 30ms
		c = (c<<1)|(0x01);	//then its logic HIGH
		else			// otherwise its logic LOW
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));// neu bang 1 dung doi cho bit dc gui xong
	}
	return c;
}
void read_dht11(){
	Request();		// send start pulse
	Response();		//receive response
	I_RH = Receive_data();	// store first eight bit in I_RH
	D_RH = Receive_data();	// store next eight bit in D_RH
	I_Temp = Receive_data();	// store next eight bit in I_Temp
	D_Temp = Receive_data();	//store next eight bit in D_Temp
	CheckSum = Receive_data();// store next eight bit in CheckSum
	if (I_RH != DAlast||I_Temp!=NDlast){ // neu doc khác moi cap nhat ra LCD
		sprintf(data,"DA=%d",I_RH); // in do am
		i2c_goto_puts(1,1,data);
		i2c_lcd_puts("%");

		sprintf(data,"ND=%d",I_Temp); // in nhiet do
		i2c_goto_puts(1,8,data);
		i2c_lcd_put(223);// put 1 ki tu '
		i2c_lcd_puts("C");
	}
	/*if (I_Temp < 10)
	{
		sprintf(data1,"0%d",I_Temp);
		sprintf(data2,"%d",I_RH);
		//strcat("0",data1);
		//SendString(strcat(strcat("0",data1),data2));
		SendString(strcat(data1,data2));
	}
	else if(I_RH < 10){
		sprintf(data1,"%d",I_Temp);
		sprintf(data2,"0%d",I_RH);
		//data1 = strcat("0",data1);
		//SendString(strcat(data1,strcat("0",data2)));
		SendString(strcat(data1,data2));
	}
	else {
		sprintf(data1,"%d",I_Temp);
		sprintf(data2,"%d",I_RH);
		SendString(strcat(data1,data2));
	}*/
	sprintf(data1,"%d",I_Temp);
	sprintf(data2,"%d",I_RH);
	SendString(strcat(data1,data2));
	NDlast = I_Temp; // gan lay gia tri cuoi
	DAlast = I_RH;
}
void TIMER0init(){
	TCCR0=(1<<CS01)|(1<<CS00); // mode NORMAL prescaler =clock/64
	TIMSK=(1<<TOIE0);
	TCNT0=130; // set gia tri bat dau dem (su dung thach anh 8.000.000 Mhz)
}
int main(void)
{	

	PORTC=0xFC;//
	DDRC=0xFF;
	PORTA = 0xFF;
	DDRA = 0xFF;

	UARTinit();
	TIMER0init();
	i2c_init();
	i2c_lcd_init();
	sei();
    /* Replace with your application code */
    while (1) 
    {
		if((dem -last) >= 1000){// dem thoi gian hoat dong su dung timer 0 don vi (ms)
			read_dht11();//ham can thuc hien 1s 1 lan
			last = dem = 0;
		}
    }
}
ISR (TIMER0_OVF_vect){
	TCNT0 = 130;
	dem++;
}
ISR(USART_RXC_vect){
	char c = UDR;
	switch(c){
		case 'A':{
			tbi(PORTC,2);
			break;
		}
		case 'B':{
			tbi(PORTC,3);
			break;
		}
		case 'C':{
			tbi(PORTC,4);
			break;
		}
		case 'D':{
			tbi(PORTC,5);
			break;
		}
		case 'E':{
			tbi(PORTC,6);
			break;
		}
		case 'F':{
			tbi(PORTC,7);
			break;
		}
		
	}
}
