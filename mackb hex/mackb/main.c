/*
 * mackb.c
 *
 * Created: 26/11/2017 13:04:13
 * Author : Thon Okocha
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define F_CPU 8000000ul
#define TX_NEWLINE		{transmitByte(0x0d); transmitByte(0x0a);}
#define FOSC 8000000 // Clock Speed
#define BAUD 9600
#define UBRR FOSC/16/BAUD-1

#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define update_eeprom_word(address,value) eeprom_update_word ((uint16_t*)address,(uint16_t)value)




#define rs PE5
#define rw PE6
#define e PE7
#define bit(x) 1 << x

unsigned char data; // variable to store value to enter in serial
uint8_t count = 0, seconds = 0;
unsigned char *str;
unsigned char option;
unsigned char option1;
unsigned char option2;

int ini_orange[4]= {0,0,0,0};
	int ini_mango[4]= {0,0,0,0};
		int ini_guava[4]= {0,0,0,0};
			int ini_pineapple[4]= {0,0,0,0};
				int ini_apple[4]= {0,0,0,0};
			
					 
					int ini_applewhole;
					   int ini_guavawhole;
					  int ini_mangowhole;
				int ini_pineapplewhole;
					  int ini_orangewhole;
int orangespercentage[2]= {0,0};	
int mangoespercentage[2]= {0,0};	
	int guavaspercentage[2]= {0,0};	
			int pineapplespercentage[2]= {0,0};	
				int applespercentage[2]= {0,0};
					int orangespercentagewhole;
					     int mangoespercentagewhole;
					       int guavaspercentagewhole;
					          int applespercentagewhole;
					           int pineapplespercentagewhole;
		
				                     int	currentoranges;
				             int currentpineapples;
				int currentguavas;
				int currentmangoes;
				int currentapples;	
				
				 int	currentoranges1;
				 int currentpineapples1;
				 int currentguavas1;
				 int currentmangoes1;
				 int currentapples1;
				 
				int storagecontainer;
				int storagecontainer1;
				int volume2[3]={0,0,0};
				
				int	currentorangesarray[4]={0,0,0,0};//converted to an array so that it can be printed into serial communications
				   int	currentmangoesarray[4]={0,0,0,0};
					   int	currentapplesarray[4]={0,0,0,0};
						   int	currentpineapplesarray[4]={0,0,0,0};
							   int	currentguavasarray[4]={0,0,0,0};
								 float selectedvolume;
								 int dispensevolume[3]={0,0,0};
								 int dispensevolumewhole;
								 int dispensedvolume;
								 int dispensedvolume1;
								 int numberofbottles;
								 int numberofbottles1;
								 int choicearray[1]={0};
								 int volume[3]={0,0,0};
							int numberofbottlesarray[3]={0,0,0};
								int numberofbottlesarraywhole;
								int numberofbottlesarr[3]={0,0,0};
								int numberofbottlesarrwhole;
								int numberofbottlesarrwhole1;
								int volumeleft;
								int volume1[3]={0,0,0};
									int bottles_filled;
									int bottles_filled2;
									int bottles_filled1[3]={0,0,0};
										int time_on, time_off;
									int time_off_arr[3]={0,0,0};
										int time_on_arr[3]={0,0,0};
										int time_off1;
										int time_on1;
								
								
									   
								   
								
					
void enable(){
	PORTE |= (1<<e);
	_delay_ms(1);
	PORTE &= ~(1<<e);
	_delay_ms(1);
	

}

void initialise(){
	DDRF=0xFF;
	PORTE=0x00;
	//set on/off display,cursor,blink
	PORTF=0x0F;
	enable();
	
	//shift cursor direction
	PORTF=0x1F;
	enable();
	
	//set interface data length
	PORTF=0x3F;
	enable();
	
	//clear display
	PORTF=0x01;
	enable();

}

void clearscreen(){
	//clear screen
	PORTF= 0x01;
	enable();
}

void lcd_position(uint8_t pos){
	//address
	if (pos==1)
	{
		PORTF = 0x80;
		enable();
	}else if (pos == 2)
	{
		PORTF = 0xC0;
		enable();
	}else if (pos == 3){
		
		PORTF = 0x94;
		enable();
	}else {
		PORTF = 0xD4;
		enable();
	}
	
}

void lcd_write_string(const char x[])
{
	uint8_t i = 0;
	while(x[i] != 0)
	{
		
		write_char(x[i]);
		i++;
		
	}
}

void write_char(unsigned char A){
	PORTE |= bit(rs);
	PORTF = A;
	enable();
	PORTE &=~ bit(rs);
}


void motor(){
	PORTD = 0x0C;
	_delay_ms(1000);
	PORTD = 0x06;
	_delay_ms(1000);
	PORTD = 0x03;
	_delay_ms(1000);
	PORTD = 0x09;
	_delay_ms(1000);
	

}

///////////////////////////////////////////////////////////////

ISR(TIMER0_OVF_vect){
	count++;
	if (count >= 112)
	{
		// 1 second
		seconds++;
		TCNT0 = 0; //reset counter
		count = 0; //reset count
	}
}

void init_timer0(){
	cli();
	// set up timer with prescaler = 256
	TCCR0 |= (1 << CS02);
	// initialize counter
	TCNT0 = 0;
	// enable overflow interrupt
	TIMSK |= (1 << TOIE0);
	// enable global interrupts
	sei();
}
////////////////////////////////////////////////////////////////////////////////


void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address and data registers */
	EEAR = uiAddress;
	
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
}
void EEPROM_write_string(unsigned char address,unsigned char *number){
	while(*number){
		EEPROM_write(address,*number++);
	}
}
unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////

void USART_Init( unsigned int ubrr )
{
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN)|(1<<TXEN) | (1 << RXCIE);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS)|(3<<UCSZ0);
}

//**************************************************
//Function to receive a single byte
//*************************************************
unsigned char receiveByte( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR0;
}

//***************************************************
//Function to transmit a single byte
//***************************************************
void transmitByte( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;}

//***************************************************
//Function to transmit a string in Flash
//***************************************************
void transmitString_F(char* string)
{
  while (pgm_read_byte(&(*string)))
   transmitByte(pgm_read_byte(&(*string++)));
}

//***************************************************
//Function to transmit a string in RAM
//***************************************************
void transmitString(char* string)
{
    while (*string)
		transmitByte(*string++);
}

void transmitInt(uint8_t in){
	itoa(in,str,10);
	transmitString(str);
}
  //////////////////////////////////////////////////////////////////////////////////////
  
  void bottle_sizes(){
	  if (ini_applewhole != NULL || ini_guavawhole != NULL || ini_mangowhole != NULL || ini_pineapplewhole != NULL || ini_orangewhole != NULL)
	  {
	  ///////////////////////////////////////////////////////////////////
	  TX_NEWLINE;
	  transmitString_F(PSTR(" PLEASE SELECT BOTTLE SIZE."));
	  TX_NEWLINE;
	  transmitString_F(PSTR(" 1 :300ml ."));
	  TX_NEWLINE;
	  transmitString_F(PSTR(" 2 :500ml."));
	  TX_NEWLINE;
	  transmitString_F(PSTR("  Select Option ( 1 - 2 ): "));
	  TX_NEWLINE;
	  transmitByte(' ');
	  option2 = receiveByte();
	  TX_NEWLINE;
	  switch (option2)
	  {
		  case '1':TX_NEWLINE;
		  selectedvolume=0.3;
		  transmitString_F(PSTR("The machine will be dispensing 300ml bottles."));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Choose 0 to enter the volume to dispense"));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Choose 1 to enter the number of bottles to fill"));
		  TX_NEWLINE;
		  transmitByte(' ');
		  data= receiveByte(); transmitByte(data);
		  TX_NEWLINE;
		  choicearray[0] = (data & 0x0f); //
		  TX_NEWLINE;
		  transmitString_F(PSTR("your choice is "));
		  transmitInt(choicearray[0]);
		  TX_NEWLINE;
		  if ((choicearray[0])==0)
		  {
			  transmitString_F(PSTR("Enter the volume you want to dispense in litres ..max=150"));//this should be out of 50
			  TX_NEWLINE;
			  transmitByte(' ');
			  data= receiveByte(); transmitByte(data);
			  dispensevolume[0] = (data & 0x0f); //
			  
			  transmitByte(' ');
			  data = receiveByte(); transmitByte(data);
			  
			  dispensevolume[1] = (data & 0x0f);
			  transmitByte(' ');
			  data = receiveByte(); transmitByte(data);
			  dispensevolume[2] = (data & 0x0f);
			  TX_NEWLINE;
			  for (int i=0;i<3;i++)
			  {
				  dispensevolumewhole=10*dispensevolumewhole+dispensevolume[i];
			  }
			  numberofbottles=dispensevolumewhole/selectedvolume;
			  numberofbottles1=numberofbottles;
			  
			  for (int i = 2; i >= 0; i--)
			  {
				  numberofbottlesarray[i] = numberofbottles% 10;
				  numberofbottles = numberofbottles/ 10;
			  }
			  volumeleft=150-dispensevolumewhole;
			  for (int i = 2; i >= 0; i--)
			  {
				  volume1[i] = volumeleft% 10;
				  volumeleft = volumeleft/ 10;
			  }
			  
			  TX_NEWLINE;
			  transmitString_F(PSTR("NUMBER OF BOTTLES THAT CAN BE FILLED WITH THE SELECTED VOLUME:"));
			  transmitInt(numberofbottlesarray[0]);
			  transmitInt(numberofbottlesarray[1]);
			  transmitInt(numberofbottlesarray[2]);
			  transmitString_F(PSTR("bottles"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("THE VOLUME IN LITRES THAT WILL BE LEFT AFTER DISPENSING IS  :"));
			  transmitInt(volume1[0]);
			  transmitInt(volume1[1]);
			  transmitInt(volume1[2]);
			  transmitString_F(PSTR("litres"));
			  TX_NEWLINE;
			  TX_NEWLINE;
			  transmitString_F(PSTR("PLANT CONFIGURATION COMPLETE PRESS BUTTON ON THE PLANT TO DISPENSE IN BOTTLES :"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PRESS ENTER TO CHECK MENU.."));
		  }
		  else if(choicearray[0]==1){
			  transmitString_F(PSTR("Enter the number of bottles you want to fill:"));//this should be out of 50
			  TX_NEWLINE;
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[0] = (data & 0x0f); //
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[1] = (data & 0x0f);
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[2] = (data & 0x0f);
			  for (int i=0;i<3;i++)
			  {
				  numberofbottlesarrwhole=10*numberofbottlesarrwhole+numberofbottlesarr[i];
			  }
			  numberofbottles1=numberofbottlesarrwhole;
			  
			  if(numberofbottlesarrwhole>500){
				  dispensedvolume=500*selectedvolume;
				  numberofbottles=500;
				  for (int i = 2; i >= 0; i--)
				  {
					  numberofbottlesarray[i] = numberofbottles% 10;
					  numberofbottles = numberofbottles/ 10;
				  }
				  
				  TX_NEWLINE;
				  transmitString_F(PSTR("SORRY THE BOTTLES YOU HAVE INPUT ARENT ENOUGH"));
				  TX_NEWLINE;
				  transmitString_F(PSTR("BOTTLES THAT CAN BE FILLED WITH THE AVAILABLE DRINK:"));
				  transmitInt(numberofbottlesarray[0]);
				  transmitInt(numberofbottlesarray[1]);
				  transmitInt(numberofbottlesarray[2]);
				  transmitString_F(PSTR("bottles"));
				  break;
				  TX_NEWLINE;
				  } else if(numberofbottlesarrwhole<=500){
				  dispensedvolume=numberofbottlesarrwhole*selectedvolume;
				  numberofbottlesarrwhole1=numberofbottlesarrwhole;
			  }
			  
			  
			  dispensedvolume1=dispensedvolume;
			  for (int i = 2; i >= 0; i--)
			  {
				  volume[i] = dispensedvolume% 10;
				  dispensedvolume = dispensedvolume/ 10;
			  }
			  
			  ///////////////////////////////////////////////////////////////////
			  
			  volumeleft=150-dispensedvolume1;
			  for (int i = 2; i >= 0; i--)
			  {
				  volume1[i] = volumeleft% 10;
				  volumeleft = volumeleft/ 10;
			  }
			  
			  TX_NEWLINE;
			  transmitString_F(PSTR(" VOLUME OF BOTTLES INPUT IS:"));
			  transmitInt(volume[0]);
			  transmitInt(volume[1]);
			  transmitInt(volume[2]);
			  transmitString_F(PSTR("litres"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("THE VOLUME THAT WILL BE LEFT:"));
			  transmitInt(volume1[0]);
			  transmitInt(volume1[1]);
			  transmitInt(volume1[2]);
			  transmitString_F(PSTR("litres"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PLANT CONFIGURATION COMPLETE PRESS BUTTON ON THE PLANT TO DISPENSE IN BOTTLES :"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PRESS ENTER TO CHECK MENU.."));
			  
			  /////////////////////////////////////////////////////////////////////
		  }
		  else if(choicearray[0]!=(1|0)){
			  transmitString_F(PSTR("invalid input"));
			  transmitString_F(PSTR("press enter to continue go back to menu.."));
		  }
		  
		  break;
		  
		  case '2':TX_NEWLINE;
		  selectedvolume=0.5;
		  transmitString_F(PSTR("The machine will be dispensing 500ml bottles."));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Choose 0 to enter the volume to dispense"));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Choose 1 to enter the number of bottles to fill"));
		  TX_NEWLINE;
		  transmitByte(' ');
		  data= receiveByte(); transmitByte(data);
		  TX_NEWLINE;
		  choicearray[0] = (data & 0x0f); //
		  TX_NEWLINE;
		  transmitString_F(PSTR("your choice is "));
		  transmitInt(choicearray[0]);
		  TX_NEWLINE;
		  if ((choicearray[0])==0)
		  {
			  transmitString_F(PSTR("Enter the volume you want to dispense in litres... max=150"));//this should be out of 50
			  TX_NEWLINE;
			  transmitByte(' ');
			  data= receiveByte(); transmitByte(data);
			  dispensevolume[0] = (data & 0x0f); //
			  
			  transmitByte(' ');
			  data = receiveByte(); transmitByte(data);
			  
			  dispensevolume[1] = (data & 0x0f);
			  transmitByte(' ');
			  data = receiveByte(); transmitByte(data);
			  dispensevolume[2] = (data & 0x0f);
			  TX_NEWLINE;
			  for (int i=0;i<3;i++)
			  {
				  dispensevolumewhole=10*dispensevolumewhole+dispensevolume[i];
			  }
			  numberofbottles=dispensevolumewhole/selectedvolume;
			  numberofbottles1=numberofbottles;
			  
			  for (int i = 2; i >= 0; i--)
			  {
				  numberofbottlesarray[i] = numberofbottles% 10;
				  numberofbottles = numberofbottles/ 10;
			  }
			  volumeleft=150-dispensevolumewhole;
			  for (int i = 2; i >= 0; i--)
			  {
				  volume1[i] = volumeleft% 10;
				  volumeleft = volumeleft/ 10;
			  }
			  
			  TX_NEWLINE;
			  transmitString_F(PSTR("NUMBER OF BOTTLES FROM INPUT VOLUME IS :"));
			  transmitInt(numberofbottlesarray[0]);
			  transmitInt(numberofbottlesarray[1]);
			  transmitInt(numberofbottlesarray[2]);
			  transmitString_F(PSTR("  bottles"));
			  
			  TX_NEWLINE;
			  transmitString_F(PSTR("THE VOLUME THAT WILL BE LEFT AFTER DISPENSING IS:"));
			  transmitInt(volume1[0]);
			  transmitInt(volume1[1]);
			  transmitInt(volume1[2]);
			  transmitString_F(PSTR(" litres"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PLANT CONFIGURATION COMPLETE PRESS BUTTON ON THE PLANT TO DISPENSE IN BOTTLES :"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PRESS ENTER TO CHECK MENU.. :"));
		  }
		  else if(choicearray[0]==1){
			  transmitString_F(PSTR("Enter the number of bottles you want to fill:"));//this should be out of 50
			  TX_NEWLINE;
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[0] = (data & 0x0f); //
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[1] = (data & 0x0f);
			  data= receiveByte(); transmitByte(data);
			  numberofbottlesarr[2] = (data & 0x0f);
			  for (int i=0;i<3;i++)
			  {
				  numberofbottlesarrwhole=10*numberofbottlesarrwhole+numberofbottlesarr[i];
			  }
			  numberofbottles1=numberofbottlesarrwhole;
			  
			  if(numberofbottlesarrwhole>300){
				  dispensedvolume=300*selectedvolume;
				  numberofbottles=300;
				  for (int i = 2; i >= 0; i--)
				  {
					  numberofbottlesarray[i] = numberofbottles% 10;
					  numberofbottles = numberofbottles/ 10;
				  }
				  TX_NEWLINE;
				  transmitString_F(PSTR("SORRY.. LIQUID IS NOT ENOUGH"));
				  TX_NEWLINE;
				  transmitString_F(PSTR("BOTTLES THAT CAN BE FILLED WITH THE AVAILABLE DRINK:"));
				  transmitInt(numberofbottlesarray[0]);
				  transmitInt(numberofbottlesarray[1]);
				  transmitInt(numberofbottlesarray[2]);
				  transmitString_F(PSTR("bottles"));
				  TX_NEWLINE;
			  }
			  
			  else if(numberofbottlesarrwhole<=300){
				  dispensedvolume=numberofbottlesarrwhole*selectedvolume;
				  numberofbottlesarrwhole1=numberofbottlesarrwhole;
			  }
			  
			  dispensedvolume1=dispensedvolume;
			  
			  for (int i = 2; i >= 0; i--)
			  {
				  volume[i] = dispensedvolume% 10;
				  dispensedvolume = dispensedvolume/ 10;
			  }
			  volumeleft=150-dispensedvolume1;
			  for (int i = 2; i >= 0; i--)
			  {
				  volume1[i] = volumeleft% 10;
				  volumeleft = volumeleft/ 10;
			  }
			  TX_NEWLINE;
			  transmitString_F(PSTR("VOLUME OF BOTTLES INPUT IS:"));
			  transmitInt(volume[0]);
			  transmitInt(volume[1]);
			  transmitInt(volume[2]);
			  transmitString_F(PSTR("litres"));
			  TX_NEWLINE;
			  
			  
			  ///////////////////////////////////////////////////////////////////
			  
			  
			  
			  TX_NEWLINE;
			  transmitString_F(PSTR("THE VOLUME THAT WILL BE LEFT AFTER DISPENSING IS:"));
			  transmitInt(volume1[0]);
			  transmitInt(volume1[1]);
			  transmitInt(volume1[2]);
			  transmitString_F(PSTR("litres"));
			  TX_NEWLINE;
			  TX_NEWLINE;
			  transmitString_F(PSTR("PLANT CONFIGURATION COMPLETE PRESS BUTTON ON THE PLANT TO DISPENSE IN BOTTLES :"));
			  TX_NEWLINE;
			  transmitString_F(PSTR("PRESS ENTER TO CHECK MENU.. :"));
			  
			  
			  /////////////////////////////////////////////////////////////////////

			  
		  }
		  else if(choicearray[0]!=(1|0)){
			  transmitString_F(PSTR("invalid input"));
		  }
		  break;
		  
		  default:
		  TX_NEWLINE;
		  transmitString_F(PSTR("Invalid Option, Try Again (1 - 4);"));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Press Enter to Continue;"));

		  break;
		  
	  }
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
  }else{
	   TX_NEWLINE;
	   transmitString_F(PSTR("PLEASE FIRST CONFIGURE THE PLANT"));
	    TX_NEWLINE;
	    transmitString_F(PSTR("PRESS ENTER TO CONTINUE"));
	  
	  
  }
  
	  }
  /////////////////////////////////////////////////////////////////////////////////////
  
  
  void menu(unsigned char opt){
   switch (opt)
   {
	   case '1': TX_NEWLINE;
			_delay_ms(200);
			   TX_NEWLINE;
		    transmitString_F(PSTR("VALUES SHOULD BE BETWEEN 0000-9999"));
			TX_NEWLINE;
		   transmitString_F( PSTR(" Enter ORANGES AVAILABLE:"));
		   TX_NEWLINE;
		  transmitByte(' ');
		   data = receiveByte(); transmitByte(data);
		   ini_orange[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   ini_orange[1] = (data & 0x0f); //
		    transmitByte(' ');	   
		   data = receiveByte(); transmitByte(data);
	       ini_orange[2] = (data & 0x0f); //
		    transmitByte(' ');
	       data = receiveByte(); transmitByte(data);
	       ini_orange[3] = (data & 0x0f);
		   for (int i=0;i<4;i++)
		   {
			  ini_orangewhole=10*ini_orangewhole+ini_orange[i];
			    }
		
			   TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES AVAILABLE ARE :"));
			   transmitInt(ini_orange[0]);
			    transmitInt(ini_orange[1]);
				 transmitInt(ini_orange[2]);
				 transmitInt(ini_orange[3]);
				 		  
	TX_NEWLINE;
	///////////////////////////
	
	 TX_NEWLINE;
			_delay_ms(200);  
			TX_NEWLINE;
		    transmitString_F(PSTR("VALUES SHOULD BE BETWEEN 0000-9999"));
			TX_NEWLINE;
		   transmitString_F( PSTR(" Enter MANGOES AVAILABLE:"));
		   TX_NEWLINE;
		  transmitByte(' ');
		   data = receiveByte(); transmitByte(data);
		   ini_mango[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   ini_mango[1] = (data & 0x0f); //
		    transmitByte(' ');	   
		   data = receiveByte(); transmitByte(data);
	       ini_mango[2] = (data & 0x0f); //
		    transmitByte(' ');
	       data = receiveByte(); transmitByte(data);
	       ini_mango[3] = (data & 0x0f);
		    for (int i=0;i<4;i++)
		   {
			  ini_mangowhole=10*ini_mangowhole+ini_mango[i];
			    }
			   TX_NEWLINE;
			   transmitString_F(PSTR("MANGOES AVAILABLE ARE :"));
			   transmitInt(ini_mango[0]);
			    transmitInt(ini_mango[1]);
				 transmitInt(ini_mango[2]);
				 transmitInt(ini_mango[3]);		  
	TX_NEWLINE;
	//////////////////////////
	 TX_NEWLINE;
			_delay_ms(200);
			TX_NEWLINE;
		    transmitString_F(PSTR("VALUES SHOULD BE BETWEEN 0000-9999"));
			TX_NEWLINE;
		   transmitString_F( PSTR(" Enter PINEAPPLES AVAILABLE:"));
		   TX_NEWLINE;
		  transmitByte(' ');
		   data = receiveByte(); transmitByte(data);
		   ini_pineapple[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   ini_pineapple[1] = (data & 0x0f); //
		    transmitByte(' ');	   
		   data = receiveByte(); transmitByte(data);
	       ini_pineapple[2] = (data & 0x0f); //
		    transmitByte(' ');
	       data = receiveByte(); transmitByte(data);
	       ini_pineapple[3] = (data & 0x0f);
		    for (int i=0;i<4;i++)
		   {
			  ini_pineapplewhole=10*ini_pineapplewhole+ini_pineapple[i];
			    }
			   TX_NEWLINE;
			   transmitString_F(PSTR("PINEAPPLES AVAILABLE ARE :"));
			   transmitInt(ini_pineapple[0]);
			    transmitInt(ini_pineapple[1]);
				 transmitInt(ini_pineapple[2]);
				 transmitInt(ini_pineapple[3]);		  
	TX_NEWLINE;
	
	
	 TX_NEWLINE;
			_delay_ms(200);
			TX_NEWLINE;
		    transmitString_F(PSTR("VALUES SHOULD BE BETWEEN 0000-9999"));
			TX_NEWLINE;
		   transmitString_F( PSTR(" Enter APPLES AVAILABLE:"));
		   TX_NEWLINE;
		  transmitByte(' ');
		   data = receiveByte(); transmitByte(data);
		   ini_apple[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   ini_apple[1] = (data & 0x0f); //
		    transmitByte(' ');	   
		   data = receiveByte(); transmitByte(data);
	       ini_apple[2] = (data & 0x0f); //
		    transmitByte(' ');
	       data = receiveByte(); transmitByte(data);
	       ini_apple[3] = (data & 0x0f);
		    for (int i=0;i<4;i++)
		   {
			  ini_applewhole=10*ini_applewhole+ini_apple[i];
			    }
			   TX_NEWLINE;
			   transmitString_F(PSTR("APPLES AVAILABLE ARE :"));
			   transmitInt(ini_apple[0]);
			    transmitInt(ini_apple[1]);
				 transmitInt(ini_apple[2]);
				 transmitInt(ini_apple[3]);		  
	TX_NEWLINE;
	
	 TX_NEWLINE;
			_delay_ms(200);
			TX_NEWLINE;
		    transmitString_F(PSTR("VALUES SHOULD BE BETWEEN 0000-9999"));
			TX_NEWLINE;
		   transmitString_F( PSTR(" Enter GUAVAS AVAILABLE:"));
		   TX_NEWLINE;
		  transmitByte(' ');
		   data = receiveByte(); transmitByte(data);
		   ini_guava[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   ini_guava[1] = (data & 0x0f); //
		    transmitByte(' ');	   
		   data = receiveByte(); transmitByte(data);
	       ini_guava[2] = (data & 0x0f); //
		    transmitByte(' ');
	       data = receiveByte(); transmitByte(data);
	       ini_guava[3] = (data & 0x0f);
		    for (int i=0;i<4;i++)
		   {
			  ini_guavawhole=10*ini_guavawhole+ini_guava[i];
			    }
			   TX_NEWLINE;
			   transmitString_F(PSTR("GUAVAS AVAILABLE ARE :"));
			   transmitInt(ini_guava[0]);
			    transmitInt(ini_guava[1]);
				 transmitInt(ini_guava[2]);
				 transmitInt(ini_guava[3]);		  
	TX_NEWLINE;
	
	
	
	
	
	///////////////////////////////////////////////////////////////////
	
	
	TX_NEWLINE;
	transmitString_F(PSTR(" PLEASE SELECT FRUITS TO MIX."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 1 :Oranges + Mangoes + Pineapple + Apples."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 2 :Oranges + Mangoes + Pineapple + Guavas."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 3 :Oranges + Mangoes + Apples + Guavas."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 4 :Oranges + Pineapple + Apples + Guavas."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 5 :Mangoes + Pineapple + Apples + Guavas."));
	TX_NEWLINE;
	transmitString_F(PSTR("  Select Option ( 1 - 5 ): "));
	TX_NEWLINE;
	storagecontainer=150;
	transmitByte(' ');
	option1 = receiveByte();
	TX_NEWLINE;
	
	switch(option1){
		case '1':TX_NEWLINE;
		_delay_ms(200);
		currentguavas1=ini_guavawhole;
		currentguavas=currentguavas1;
		 for (int i = 3; i >= 0; i--)
		 {
			 currentguavasarray[i] = currentguavas% 10;
			 currentguavas = currentguavas/ 10;
		 }
		TX_NEWLINE;
		    transmitString_F(PSTR("PERCENTAGES FOR THE FOUR FRUITS SHOULD TOTAL TO 100"));
			TX_NEWLINE;
		transmitString_F(PSTR("You have selected the first category of fruits"));
		TX_NEWLINE;
		transmitString_F(PSTR("Enter the percentage of oranges"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   orangespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   orangespercentage[1] = (data & 0x0f); //
		   for (int i=0;i<2;i++)
		   {
			   orangespercentagewhole=10*orangespercentagewhole+orangespercentage[i];
		   }
		   currentoranges1=ini_orangewhole-(orangespercentagewhole*30);
		   currentoranges=currentoranges1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentorangesarray[i] = currentoranges % 10;
    currentoranges = currentoranges/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES REMAINING ARE :"));
			   transmitInt(currentorangesarray[0]);
			    transmitInt(currentorangesarray[1]);
				 transmitInt(currentorangesarray[2]);
				 transmitInt(currentorangesarray[3]);		  
	TX_NEWLINE;
		   
		   
		   ////////////////////////////////
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Orange percentage selected:"));
			   transmitInt(orangespercentage[0]);
			    transmitInt(orangespercentage[1]);
				//////////////////////////////////////////////////////////////////
				 TX_NEWLINE;
				transmitString_F(PSTR("Enter the percentage of mangoes"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   mangoespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   mangoespercentage[1] = (data & 0x0f); //
		    for (int i=0;i<2;i++)
		   {
			   mangoespercentagewhole=10*mangoespercentagewhole+mangoespercentage[i];
		   }
		   	   currentmangoes1=ini_mangowhole-(mangoespercentagewhole*30);
				  currentmangoes=currentmangoes1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentmangoesarray[i] = currentmangoes % 10;
    currentmangoes = currentmangoes/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("MANGOES REMAINING ARE :"));
			   transmitInt(currentmangoesarray[0]);
			    transmitInt(currentmangoesarray[1]);
				 transmitInt(currentmangoesarray[2]);
				 transmitInt(currentmangoesarray[3]);		  
	TX_NEWLINE;
		 TX_NEWLINE;
			   transmitString_F(PSTR("Mangoes percentage selected:"));
			   transmitInt(mangoespercentage[0]);
			    transmitInt(mangoespercentage[1]);
				
				//////////////////////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of pineapples"));
		 TX_NEWLINE; 
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   pineapplespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   pineapplespercentage[1] = (data & 0x0f); //
		     for (int i=0;i<2;i++)
		   {
			   pineapplespercentagewhole=10*pineapplespercentagewhole+pineapplespercentage[i];
		   }
		   	   currentpineapples1=ini_pineapplewhole-(pineapplespercentagewhole*30);
				  currentpineapples=currentpineapples1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentpineapplesarray[i] = currentpineapples% 10;
    currentpineapples = currentpineapples/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("PINEAPPLES REMAINING ARE :"));
			   transmitInt(currentpineapplesarray[0]);
			    transmitInt(currentpineapplesarray[1]);
				 transmitInt(currentpineapplesarray[2]);
				 transmitInt(currentpineapplesarray[3]);		  
				 
		 TX_NEWLINE;
			   transmitString_F(PSTR("Pineapples percentage selected:"));
			   transmitInt(pineapplespercentage[0]);
			    transmitInt(pineapplespercentage[1]);
				//////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of apples"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   applespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   applespercentage[1] = (data & 0x0f); //
		     for (int i=0;i<2;i++)
		   {
			   applespercentagewhole=10*applespercentagewhole+applespercentage[i];
		   }
		   	   currentapples1=ini_applewhole-(applespercentagewhole*30);
				  currentapples=currentapples1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentapplesarray[i] = currentapples % 10;
    currentapples = currentapples/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("APPLES REMAINING ARE :"));
			   transmitInt(currentapplesarray[0]);
			    transmitInt(currentapplesarray[1]);
				 transmitInt(currentapplesarray[2]);
				 transmitInt(currentapplesarray[3]);		  
	TX_NEWLINE;
		 TX_NEWLINE;
			   transmitString_F(PSTR("apples percentage selected:"));
			   transmitInt(applespercentage[0]);
			    transmitInt(applespercentage[1]);
				
				//////////////////////////
				
				
				
				
				//////////////////////////
				
		break;
		///////////////////////////////////////////////////////
		case '2':TX_NEWLINE;
		_delay_ms(200);
		currentapples1=ini_applewhole;
		currentapples=currentapples1;
		for (int i = 3; i >= 0; i--)
		{
			currentapplesarray[i] = currentapples % 10;
			currentapples = currentapples/ 10;
		}
		TX_NEWLINE;
		    transmitString_F(PSTR("PERCENTAGES FOR THE FOUR FRUITS SHOULD TOTAL TO 100"));
			TX_NEWLINE;
		transmitString_F(PSTR("You have selected the second category of fruits"));
		TX_NEWLINE;
		transmitString_F(PSTR("Enter the percentage of oranges"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   orangespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   orangespercentage[1] = (data & 0x0f); //
		    for (int i=0;i<2;i++)
		   {
			   orangespercentagewhole=10*orangespercentagewhole+orangespercentage[i];
		   }
		   currentoranges1=ini_orangewhole-(orangespercentagewhole*30);
		   currentoranges=currentoranges1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentorangesarray[i] = currentoranges % 10;
    currentoranges = currentoranges/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES REMAINING ARE :"));
			   transmitInt(currentorangesarray[0]);
			    transmitInt(currentorangesarray[1]);
				 transmitInt(currentorangesarray[2]);
				 transmitInt(currentorangesarray[3]);		 
		   
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Orange percentage selected:"));
			   transmitInt(orangespercentage[0]);
			    transmitInt(orangespercentage[1]);
				//////////////////////////////////////////////////////////////////
				 TX_NEWLINE;
				transmitString_F(PSTR("Enter the percentage of mangoes"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   mangoespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   mangoespercentage[1] = (data & 0x0f); //array to store 
		      for (int i=0;i<2;i++)
		   {
			   mangoespercentagewhole=10*mangoespercentagewhole+mangoespercentage[i];
		   }
		   	   currentmangoes1=ini_mangowhole-(mangoespercentagewhole*30);
				 currentmangoes=currentmangoes1; 
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentmangoesarray[i] = currentmangoes % 10;
    currentmangoes = currentmangoes/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("MANGOES REMAINING ARE :"));
			   transmitInt(currentmangoesarray[0]);
			    transmitInt(currentmangoesarray[1]);
				 transmitInt(currentmangoesarray[2]);
				 transmitInt(currentmangoesarray[3]);		  
	TX_NEWLINE;
		 TX_NEWLINE;
			   transmitString_F(PSTR("Mangoes percentage selected:"));
			   transmitInt(mangoespercentage[0]);
			    transmitInt(mangoespercentage[1]);
				
				//////////////////////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of pineapples"));
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   pineapplespercentage[0] = (data & 0x0f); //array to store 
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   pineapplespercentage[1] = (data & 0x0f); //array to store 
		        for (int i=0;i<2;i++)
		   {
			   pineapplespercentagewhole=10*pineapplespercentagewhole+pineapplespercentage[i];
		   }
		   	   currentpineapples1=ini_pineapplewhole-(pineapplespercentagewhole*30);
				  currentpineapples=currentpineapples1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentpineapplesarray[i] = currentpineapples% 10;
    currentpineapples = currentpineapples/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("PINEAPPLES REMAINING ARE :"));
			   transmitInt(currentpineapplesarray[0]);
			    transmitInt(currentpineapplesarray[1]);
				 transmitInt(currentpineapplesarray[2]);
				 transmitInt(currentpineapplesarray[3]);		  
				 
		 TX_NEWLINE;
			   transmitString_F(PSTR("Pineapples percentage selected:"));
			   transmitInt(pineapplespercentage[0]);
			    transmitInt(pineapplespercentage[1]);
				//////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of guavas"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   guavaspercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   guavaspercentage[1] = (data & 0x0f); //
		        for (int i=0;i<2;i++)
		   {
			   guavaspercentagewhole=10*guavaspercentagewhole+guavaspercentage[i];
		   }
		   	   currentguavas1=ini_guavawhole-(guavaspercentagewhole*30);
				  currentguavas=currentguavas1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentguavasarray[i] = currentguavas% 10;
    currentguavas = currentguavas/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("GUAVAS REMAINING ARE :"));
			   transmitInt(currentguavasarray[0]);
			    transmitInt(currentguavasarray[1]);
				 transmitInt(currentguavasarray[2]);
				 transmitInt(currentguavasarray[3]);		  
	TX_NEWLINE;
		 TX_NEWLINE;
			   transmitString_F(PSTR("Guavas percentage selected:"));
			   transmitInt(guavaspercentage[0]);
			    transmitInt(guavaspercentage[1]);
		
		break;
		///////////////////////////////////////////////
		case '3':TX_NEWLINE;
		_delay_ms(200);
		currentpineapples1=ini_pineapplewhole;
		currentpineapples=currentpineapples1;
		for (int i = 3; i >= 0; i--)
		{
			currentpineapplesarray[i] = currentpineapples% 10;
			currentpineapples = currentpineapples/ 10;
		}
		TX_NEWLINE;
		    transmitString_F(PSTR("PERCENTAGES FOR THE FOUR FRUITS SHOULD TOTAL TO 100"));
			TX_NEWLINE;
		transmitString_F(PSTR("You have selected the third category of fruits"));
		TX_NEWLINE;
		transmitString_F(PSTR("Enter the percentage of oranges"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   orangespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   orangespercentage[1] = (data & 0x0f); //
		   //////////////////////////////////
		      for (int i=0;i<2;i++)
		   {
			   orangespercentagewhole=10*orangespercentagewhole+orangespercentage[i];
		   }
		   currentoranges1=ini_orangewhole-(orangespercentagewhole*30);
		   currentoranges=currentoranges1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentorangesarray[i] = currentoranges % 10;
    currentoranges = currentoranges/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES REMAINING ARE :"));
			   transmitInt(currentorangesarray[0]);
			    transmitInt(currentorangesarray[1]);
				 transmitInt(currentorangesarray[2]);
				 transmitInt(currentorangesarray[3]);		 
		   
		   
		   
		   
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Orange percentage selected:"));
			   transmitInt(orangespercentage[0]);
			    transmitInt(orangespercentage[1]);
				//////////////////////////////////////////////////////////////////
				 TX_NEWLINE;
				transmitString_F(PSTR("Enter the percentage of mangoes"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   mangoespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   mangoespercentage[1] = (data & 0x0f); //
		   /////////////////////////////////////////////
		        for (int i=0;i<2;i++)
		   {
			   mangoespercentagewhole=10*mangoespercentagewhole+mangoespercentage[i];
		   }
		   	   currentmangoes1=ini_mangowhole-(mangoespercentagewhole*30);
				  currentmangoes=currentmangoes1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentmangoesarray[i] = currentmangoes % 10;
    currentmangoes = currentmangoes/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("MANGOES REMAINING ARE :"));
			   transmitInt(currentmangoesarray[0]);
			    transmitInt(currentmangoesarray[1]);
				 transmitInt(currentmangoesarray[2]);
				 transmitInt(currentmangoesarray[3]);		  
	TX_NEWLINE;
		   
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Mangoes percentage selected:"));
			   transmitInt(mangoespercentage[0]);
			    transmitInt(mangoespercentage[1]);
				
				//////////////////////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of apples"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   applespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   applespercentage[1] = (data & 0x0f); //
		   ///////////////////////////////////
		   
		   	     for (int i=0;i<2;i++)
		   {
			   applespercentagewhole=10*applespercentagewhole+applespercentage[i];
		   }
		   	   currentapples1=ini_applewhole-(applespercentagewhole*30);
				  currentapples=currentapples1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentapplesarray[i] = currentapples % 10;
    currentapples = currentapples/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("APPLES REMAINING ARE :"));
			   transmitInt(currentapplesarray[0]);
			    transmitInt(currentapplesarray[1]);
				 transmitInt(currentapplesarray[2]);
				 transmitInt(currentapplesarray[3]);	
		 TX_NEWLINE;
			   transmitString_F(PSTR("Pineapples percentage selected:"));
			   transmitInt(applespercentage[0]);
			    transmitInt(applespercentage[1]);
				//////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of guavas"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   guavaspercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   guavaspercentage[1] = (data & 0x0f); //
		          for (int i=0;i<2;i++)
		   {
			   guavaspercentagewhole=10*guavaspercentagewhole+guavaspercentage[i];
		   }
		   	   currentguavas1=ini_guavawhole-(guavaspercentagewhole*30);
				  currentguavas=currentguavas1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentguavasarray[i] = currentguavas% 10;
    currentguavas = currentguavas/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("GUAVAS REMAINING ARE :"));
			   transmitInt(currentguavasarray[0]);
			    transmitInt(currentguavasarray[1]);
				 transmitInt(currentguavasarray[2]);
				 transmitInt(currentguavasarray[3]);		  
	TX_NEWLINE;
		 TX_NEWLINE;
			   transmitString_F(PSTR("Guavas percentage selected:"));
			   transmitInt(guavaspercentage[0]);
			    transmitInt(guavaspercentage[1]);
		
		break;
		/////////////////////////////////////////////////
		case '4':TX_NEWLINE;
		_delay_ms(200);
		currentmangoes1=ini_mangowhole;
		currentmangoes=currentmangoes1;
		for (int i = 3; i >= 0; i--)
		{
			currentmangoesarray[i] = currentmangoes % 10;
			currentmangoes = currentmangoes/ 10;
		}
		TX_NEWLINE;
		    transmitString_F(PSTR("PERCENTAGES FOR THE FOUR FRUITS SHOULD TOTAL TO 100"));
			TX_NEWLINE;
		transmitString_F(PSTR("You have selected the four category of fruits"));
		TX_NEWLINE;
		transmitString_F(PSTR("Enter the percentage of oranges"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   orangespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   orangespercentage[1] = (data & 0x0f); //
		   /////////////////////////////
		         for (int i=0;i<2;i++)
		   {
			   orangespercentagewhole=10*orangespercentagewhole+orangespercentage[i];
		   }
		   currentoranges1=ini_orangewhole-(orangespercentagewhole*30);
		   currentoranges=currentoranges1;
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentorangesarray[i] = currentoranges % 10;
    currentoranges = currentoranges/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES REMAINING ARE :"));
			   transmitInt(currentorangesarray[0]);
			    transmitInt(currentorangesarray[1]);
				 transmitInt(currentorangesarray[2]);
				 transmitInt(currentorangesarray[3]);		 
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Orange percentage selected:"));
			   transmitInt(orangespercentage[0]);
			    transmitInt(orangespercentage[1]);
				//////////////////////////////////////////////////////////////////
				 TX_NEWLINE;
				transmitString_F(PSTR("Enter the percentage of pineapples"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   pineapplespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   pineapplespercentage[1] = (data & 0x0f); //
		   //////////////////////////////////////
		   	   for (int i=0;i<2;i++)
		   	   {
			   	   pineapplespercentagewhole=10*pineapplespercentagewhole+pineapplespercentage[i];
		   	   }
		   	   currentpineapples1=ini_pineapplewhole-(pineapplespercentagewhole*30);
		   	   currentpineapples=currentpineapples1;
				  ////////////////////////////////
		   	   for (int i = 3; i >= 0; i--)
		   	   {
			   	   currentpineapplesarray[i] = currentpineapples% 10;
			   	   currentpineapples = currentpineapples/ 10;
		   	   }
		   	   TX_NEWLINE;
		   	   transmitString_F(PSTR("PINEAPPLES REMAINING ARE :"));
		   	   transmitInt(currentpineapplesarray[0]);
		   	   transmitInt(currentpineapplesarray[1]);
		   	   transmitInt(currentpineapplesarray[2]);
		   	   transmitInt(currentpineapplesarray[3]);		  
		 TX_NEWLINE;
			   transmitString_F(PSTR("pineapples percentage selected:"));
			   transmitInt(pineapplespercentage[0]);
			    transmitInt(pineapplespercentage[1]);
				
				//////////////////////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of apples"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   applespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   applespercentage[1] = (data & 0x0f); //
		       for (int i=0;i<2;i++)
		   {
			   applespercentagewhole=10*applespercentagewhole+applespercentage[i];
		   }
		   	   currentapples1=ini_applewhole-(applespercentagewhole*30);
              currentapples=currentapples1;		   
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentapplesarray[i] = currentapples % 10;
    currentapples = currentapples/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("APPLES REMAINING ARE :"));
			   transmitInt(currentapplesarray[0]);
			    transmitInt(currentapplesarray[1]);
				 transmitInt(currentapplesarray[2]);
				 transmitInt(currentapplesarray[3]);		  
	TX_NEWLINE;
	
		 TX_NEWLINE;
			   transmitString_F(PSTR("apples percentage selected:"));
			   transmitInt(applespercentage[0]);
			    transmitInt(applespercentage[1]);
				//////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of guavas"));

		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   guavaspercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   guavaspercentage[1] = (data & 0x0f); //
		   //////////////////////////////////////////////////////////
		             for (int i=0;i<2;i++)
		   {
			   guavaspercentagewhole=10*guavaspercentagewhole+guavaspercentage[i];
		   }
		   	   currentguavas1=ini_guavawhole-(guavaspercentagewhole*30);
				currentguavas=currentguavas1;  
		   ////////////////////////////////
		   for (int i = 3; i >= 0; i--)
{
   currentguavasarray[i] = currentguavas% 10;
    currentguavas = currentguavas/ 10;
}
  TX_NEWLINE;
			   transmitString_F(PSTR("GUAVAS REMAINING ARE :"));
			   transmitInt(currentguavasarray[0]);
			    transmitInt(currentguavasarray[1]);
				 transmitInt(currentguavasarray[2]);
				 transmitInt(currentguavasarray[3]);		  
	TX_NEWLINE;
		   
		   
		   
		   
		   
		   
		   
		   
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Guavas percentage selected:"));
			   transmitInt(guavaspercentage[0]);
			    transmitInt(guavaspercentage[1]);
		break;
		/////////////////////////////////////////////
		case '5':TX_NEWLINE;
		_delay_ms(200);
		currentoranges1=ini_orangewhole;
		currentoranges=currentoranges1;
		for (int i = 3; i >= 0; i--)
		{
			currentorangesarray[i] = currentoranges % 10;
			currentoranges = currentoranges/ 10;
		}
		TX_NEWLINE;
		    transmitString_F(PSTR("PERCENTAGES FOR THE FOUR FRUITS SHOULD TOTAL TO 100"));
			TX_NEWLINE;
		transmitString_F(PSTR("You have selected the fifth category of fruits"));
		TX_NEWLINE;
		transmitString_F(PSTR("Enter the percentage of mangoes"));
		TX_NEWLINE;
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   mangoespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   mangoespercentage[1] = (data & 0x0f); //
		   ////////////////////////////////
		       for (int i=0;i<2;i++)
		       {
			       mangoespercentagewhole=10*mangoespercentagewhole+mangoespercentage[i];
		       }
		       currentmangoes1=ini_mangowhole-(mangoespercentagewhole*30);
			   currentmangoes=currentmangoes1;
		       ////////////////////////////////
		       for (int i = 3; i >= 0; i--)
		       {
			       currentmangoesarray[i] = currentmangoes% 10;
			       currentmangoes = currentmangoes/ 10;
		       }
		       TX_NEWLINE;
		       transmitString_F(PSTR("MANGOES REMAINING ARE :"));
		       transmitInt(currentmangoesarray[0]);
		       transmitInt(currentmangoesarray[1]);
		       transmitInt(currentmangoesarray[2]);
		       transmitInt(currentmangoesarray[3]);
		       TX_NEWLINE;
		       TX_NEWLINE;
		  
		 TX_NEWLINE;
			   transmitString_F(PSTR("Mango percentage selected:"));
			   transmitInt(mangoespercentage[0]);
			    transmitInt(mangoespercentage[1]);
				//////////////////////////////////////////////////////////////////
				 TX_NEWLINE;
				transmitString_F(PSTR("Enter the percentage of pineapples"));
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   pineapplespercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   pineapplespercentage[1] = (data & 0x0f); //
		   ///////////////////////////////////////////////
		    	        for (int i=0;i<2;i++)
		    	        {
			    	        pineapplespercentagewhole=10*pineapplespercentagewhole+pineapplespercentage[i];
		    	        }
		    	        currentpineapples1=ini_pineapplewhole-(pineapplespercentagewhole*30);
						currentpineapples=currentpineapples1;
		    	        ////////////////////////////////
		    	        for (int i = 3; i >= 0; i--)
		    	        {
			    	        currentpineapplesarray[i] = currentpineapples% 10;
			    	        currentpineapples = currentpineapples/ 10;
		    	        }
		    	        TX_NEWLINE;
		    	        transmitString_F(PSTR("PINEAPPLES REMAINING ARE :"));
		    	        transmitInt(currentpineapplesarray[0]);
		    	        transmitInt(currentpineapplesarray[1]);
		    	        transmitInt(currentpineapplesarray[2]);
		    	        transmitInt(currentpineapplesarray[3]);
		    	        TX_NEWLINE;
		    	        TX_NEWLINE;
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("pineapples percentage selected:"));
			   transmitInt(pineapplespercentage[0]);
			    transmitInt(pineapplespercentage[1]);
				//////////////////////////////////////////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of apples"));
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   applespercentage[0] = (data & 0x0f); //
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   applespercentage[1] = (data & 0x0f); //
		   TX_NEWLINE;
		       for (int i=0;i<2;i++)
		       {
			       applespercentagewhole=10*applespercentagewhole+applespercentage[i];
		       }
		       currentapples1=ini_applewhole-(applespercentagewhole*30);
			   currentapples=currentapples1;
		       ////////////////////////////////
		       for (int i = 3; i >= 0; i--)
		       {
			       currentapplesarray[i] = currentapples % 10;
			       currentapples = currentapples/ 10;
		       }
		       TX_NEWLINE;
		       transmitString_F(PSTR("APPLES REMAINING ARE :"));
		       transmitInt(currentapplesarray[0]);
		       transmitInt(currentapplesarray[1]);
		       transmitInt(currentapplesarray[2]);
		       transmitInt(currentapplesarray[3]);
		   
		 TX_NEWLINE;
			   transmitString_F(PSTR("Pineapples percentage selected:"));
			   transmitInt(applespercentage[0]);
			    transmitInt(applespercentage[1]);
				///////////////////////////
				 TX_NEWLINE;
						transmitString_F(PSTR("Enter the percentage of guavas"));
		 TX_NEWLINE;
		  transmitByte(' ');
		   data= receiveByte(); transmitByte(data);
		   guavaspercentage[0] = (data & 0x0f); //
		  
		    transmitByte(' ');	
			data = receiveByte(); transmitByte(data);
		   guavaspercentage[1] = (data & 0x0f); //
		        TX_NEWLINE;
		        for (int i=0;i<2;i++)
		        {
			        guavaspercentagewhole=10*guavaspercentagewhole+guavaspercentage[i];
		        }
		        currentguavas1=ini_guavawhole-(guavaspercentagewhole*30);
				
				currentguavas=currentguavas1;
		        ////////////////////////////////
		        for (int i = 3; i >= 0; i--)
		        {
			        currentguavasarray[i] = currentguavas% 10;
			        currentguavas = currentguavas/ 10;
		        }
		        TX_NEWLINE;
		        transmitString_F(PSTR("GUAVAS REMAINING ARE :"));
		        transmitInt(currentguavasarray[0]);
		        transmitInt(currentguavasarray[1]);
		        transmitInt(currentguavasarray[2]);
		        transmitInt(currentguavasarray[3]);
		        TX_NEWLINE;
		         TX_NEWLINE;
			   transmitString_F(PSTR("Guavas percentage selected:"));
			   transmitInt(guavaspercentage[0]);
			    transmitInt(guavaspercentage[1]);
				
				  
				
		break;
		  /////////////////////////////////////////////////////////////////////////////////////
		  default:
		  TX_NEWLINE;
		  transmitString_F(PSTR("Invalid Option, Try Again (1 - 4);"));
		  TX_NEWLINE;
		  transmitString_F(PSTR("Press Enter to Continue;"));

		  break;
	}  
	
	///////////////////////////////////////////choosing bottle sizes/////////////////////////////////////////////////////////////////
	  eeprom_write_byte(5,currentoranges1);
	  eeprom_write_byte(6,currentpineapples1);
	  eeprom_write_byte(7,currentguavas1);
	  eeprom_write_byte(8,currentmangoes1);
	  eeprom_write_byte(9,currentapples1);
	  
	  
	  
	
	bottle_sizes();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                         
			
		   break;
	   
	   //////////////////////////////////////////////////////////////////////////////////////
	   case '2': TX_NEWLINE;//
			_delay_ms(200);
			
		   transmitString_F( PSTR(" Amount of drink in liters in the storage container:"));
		   
	
	 ///////////////////////////////////////////////////////////////////
	 storagecontainer1=storagecontainer;
	 
	
			  for (int i = 2; i >= 0; i--)
			  {
				  volume2[i] = storagecontainer1% 10;
				  storagecontainer1 = storagecontainer1/ 10;
			  }
			  
			  TX_NEWLINE;
			  transmitInt(volume2[0]);
			  transmitInt(volume2[1]);
			  transmitInt(volume2[2]);
			   transmitString_F( PSTR(" litres"));
			  TX_NEWLINE;
			 TX_NEWLINE;
			 transmitString_F(PSTR("Press Enter to Continue;"));
			 
			 /////////////////////////////////////////////////////////////////////
	
		   TX_NEWLINE;
		  transmitByte(' ');
		   
	   break;
	  ///////////////////////////////////////////////////////////////////////////////////// 
	    case '3': TX_NEWLINE;//
			_delay_ms(200);
		   transmitString_F( PSTR(" Bottles that are filled with  the drink :"));
		   TX_NEWLINE;
		   //////////////////////////////////
		   bottles_filled2=bottles_filled;
		    for (int i = 2; i >= 0; i--)
			 {
				 bottles_filled1[i] = bottles_filled2% 10;
				 bottles_filled2 = bottles_filled2/ 10;
			 }
			 
			 transmitInt(bottles_filled1[0]);
			 transmitInt(bottles_filled1[1]);
			 transmitInt(bottles_filled1[2]);
			 TX_NEWLINE;
			 TX_NEWLINE;
			 transmitString_F(PSTR("Press Enter to Continue;"));
		   
		   ////////////////////////////////////
		   
		   
		  transmitByte(' ');
		   
	   break;
	  ///////////////////////////////////////////////////////////////////////////////////// 
	 
	  case '4': TX_NEWLINE;//
			_delay_ms(200);
			currentoranges=currentoranges1;
		   transmitString_F( PSTR(" Amount of fruit left after mixing the drink :"));
		   for (int i = 3; i >= 0; i--)
		   {
			   currentorangesarray[i] = currentoranges % 10;
			   currentoranges = currentoranges/ 10;
		   }
		    TX_NEWLINE;
			   transmitString_F(PSTR("ORANGES REMAINING ARE :"));
			   transmitInt(currentorangesarray[0]);
			    transmitInt(currentorangesarray[1]);
				 transmitInt(currentorangesarray[2]);
				 transmitInt(currentorangesarray[3]);	
				  TX_NEWLINE;
				  currentmangoes=currentmangoes1;
				  for (int i = 3; i >= 0; i--)
				  {
					  currentmangoesarray[i] = currentmangoes% 10;
					  currentmangoes = currentmangoes/ 10;
				  }
			   transmitString_F(PSTR("MANGOES REMAINING ARE :"));
			   transmitInt(currentmangoesarray[0]);
			    transmitInt(currentmangoesarray[1]);
				 transmitInt(currentmangoesarray[2]);
				 transmitInt(currentmangoesarray[3]);		  
	  TX_NEWLINE;
	  currentpineapples=currentpineapples1;
	   for (int i = 3; i >= 0; i--)
	   {
		   currentpineapplesarray[i] = currentpineapples% 10;
		   currentpineapples = currentpineapples/ 10;
	   }
			   transmitString_F(PSTR("PINEAPPLES REMAINING ARE :"));
			   transmitInt(currentpineapplesarray[0]);
			    transmitInt(currentpineapplesarray[1]);
				 transmitInt(currentpineapplesarray[2]);
				 transmitInt(currentpineapplesarray[3]);	
		    TX_NEWLINE;
			currentguavas=currentguavas1;
			  for (int i = 3; i >= 0; i--)
			  {
				  currentguavasarray[i] = currentguavas% 10;
				  currentguavas = currentguavas/ 10;
			  }
			   transmitString_F(PSTR("GUAVAS REMAINING ARE :"));
			   transmitInt(currentguavasarray[0]);
			    transmitInt(currentguavasarray[1]);
				 transmitInt(currentguavasarray[2]);
				 transmitInt(currentguavasarray[3]);		  
		    TX_NEWLINE;
			currentapples=currentapples1;
			 for (int i = 3; i >= 0; i--)
			 {
				 currentapplesarray[i] = currentapples % 10;
				 currentapples = currentapples/ 10;
			 }
		       transmitString_F(PSTR("APPLES REMAINING ARE :"));
		       transmitInt(currentapplesarray[0]);
		       transmitInt(currentapplesarray[1]);
		       transmitInt(currentapplesarray[2]);
		       transmitInt(currentapplesarray[3]);
			   
			   TX_NEWLINE;
			   transmitString_F(PSTR("Press Enter to Continue;"));
		   
				 		  
	TX_NEWLINE;
		  transmitByte(' ');
		   
	   break;
	  ///////////////////////////////////////////////////////////////////////////////////// 
	   case '5': TX_NEWLINE;//
			_delay_ms(200);
		   transmitString_F( PSTR("Time the plant has worked:"));
		  ///////////////////////////
		  time_on1=time_on;
		  for (int i = 2; i >= 0; i--)
		  {
			  time_on_arr[i] = time_on1% 10;
			  time_on1 = time_on1/ 10;
		  }
		  
		  transmitInt(time_on_arr[0]);
		  transmitInt(time_on_arr[1]);
		  transmitInt(time_on_arr[2]);
		  transmitString_F( PSTR(" seconds"));
		  TX_NEWLINE;
		  TX_NEWLINE;
		  transmitString_F(PSTR("Press Enter to Continue;"));
		  
		  
		  //////////////////////////////
		  TX_NEWLINE;
		  transmitByte(' ');
		   
	   break;
	  ///////////////////////////////////////////////////////////////////////////////////// 
	   case '6': TX_NEWLINE;//
		_delay_ms(200);
		   transmitString_F( PSTR(" Time the plant had rested  is:"));
		   ///////////////////////////
		   time_off1=time_off;
		    for (int i = 2; i >= 0; i--)
		    {
			    time_off_arr[i] = time_off1% 10;
			    time_off1 = time_off1/ 10;
		    }
		    
		    transmitInt(time_off_arr[0]);
		    transmitInt(time_off_arr[1]);
		    transmitInt(time_off_arr[2]);
			transmitString_F( PSTR(" seconds"));
		    TX_NEWLINE;
			TX_NEWLINE;
			transmitString_F(PSTR("Press Enter to Continue;"));
		   
		   
		   //////////////////////////////
		   TX_NEWLINE;
		  transmitByte(' ');
		   
	   break;
	   //////////////////////////////////////////////////////////////////////////////////
	   
	   
	     case '7': TX_NEWLINE;//
	     _delay_ms(200);
	     transmitString_F( PSTR(" Change the volume to be dispensed:"));
	     ///////////////////////////
	    
	     bottle_sizes();
	     //////////////////////////////
	     TX_NEWLINE;
	     transmitByte(' ');
	     
	     break;
	   
	   
	   
	  ///////////////////////////////////////////////////////////////////////////////////// 
	   //////////////////////////////////////////////////////////////////////////////////
	   
	   
	   case '8': TX_NEWLINE;//
	   _delay_ms(200);
	   transmitString_F( PSTR(" Clearing Existing Setting"));
	   ///////////////////////////
	   numberofbottles1=0;
	   bottles_filled=0;
	   time_off=0;
	   time_on=0;
	   storagecontainer=0;
	   
	   currentoranges=0;
	   currentpineapples=0;
	   currentguavas=0;
	   currentmangoes=0;
	   currentapples=0;
	   
	   
	   _delay_ms(2000);
	   eeprom_write_byte(0,numberofbottles1);
	  eeprom_write_byte(1,bottles_filled);
	  eeprom_write_byte(2,time_off);
	   eeprom_write_byte(3,time_on);
	   eeprom_write_byte(4,storagecontainer);
	     eeprom_write_byte(5,currentoranges1);
	     eeprom_write_byte(6,currentpineapples1);
	     eeprom_write_byte(7,currentguavas1);
	     eeprom_write_byte(8,currentmangoes1);
	     eeprom_write_byte(9,currentapples1);
	   
	 
	   
	   TX_NEWLINE;
	   transmitString_F(PSTR("Settings cleared successfully"));
	   TX_NEWLINE;
	   transmitString_F(PSTR("Press Enter to Continue;"));
	 
	   //////////////////////////////
	   TX_NEWLINE;
	   transmitByte(' ');
	   
	   break;
	   
	   
	   
	   /////////////////////////////////////////////////////////////////////////////////////
	  
	  
		default:
		TX_NEWLINE;
			transmitString_F(PSTR("Invalid Option, Try Again (1 - 8);"));
			TX_NEWLINE;
			transmitString_F(PSTR("Press Enter to Continue;"));

			break;
   }
	 
}
  
  
  
  
   ////////////////////////////////////////////////////////////////////////////////////////


//USART ISR
ISR(USART0_RX_vect){
	char dataIn;
	option = UDR0;	
	TX_NEWLINE;
	transmitString_F(PSTR(" 1 : Configure the plant."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 2 :CHECK Amount of drink in liters in the storage container."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 3 :CHECK How many bottles are filled with  the drink."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 4 :CHECK Amount of fruit left after mixing the drink."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 5 :CHECK How much time the plant has worked."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 6 :CHECK How much time the plant had rested."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 7 :Change the volume to be dispensed."));
	TX_NEWLINE;
	transmitString_F(PSTR(" 8 :Clear Existing Settings"));
	TX_NEWLINE;
	transmitString_F(PSTR("  Select Option ( 1 - 8 ): "));
	TX_NEWLINE;
	transmitByte(' ');
	option = receiveByte();
	TX_NEWLINE;
	menu(option);
}





////////////////////////////////////////////////
int main(void)

{	
	DDRA =  0xFF;
	DDRB =  0x01;
	DDRE = 0xFF;
	DDRE = 0b11111110;
	
	USART_Init(UBRR);
	init_timer0();
	TX_NEWLINE;
	TX_NEWLINE;
	transmitString_F(PSTR("|*****************************************   |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|    WELCOME TO MACK BEVERAGES               |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|    GROUP MEMBERS                           |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|1: SSEBBAALE PAUL             15/U/12712/EVE|"));
	TX_NEWLINE;
	transmitString_F(PSTR("|2: KASINGABALYA JEROME	        15/U/5873/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|3: WATASA VICTOR CHRIS	        15/U/13762/EVE|"));
	TX_NEWLINE;
	transmitString_F(PSTR("|4: MUBOKU EMMANUEL	            15/U/7738/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|5: MURINGI REMULUS	            15/U/8324/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|6: KABWAMA RONALD	             15/U/5398/EVE |"));
	TX_NEWLINE;  
	transmitString_F(PSTR("|7: OWOT JONATHAN	              15/U/12361/EVE|"));
	TX_NEWLINE;
	transmitString_F(PSTR("|8: KATONGOLE DRAKE	            15/U/6012/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|9: ASIGA STANLEY	              15/U/6012/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|10: KABOGYOZA STEPHEN	         15/U/5366/EVE |"));
	TX_NEWLINE;
	transmitString_F(PSTR("|*****************************************   |"));
	TX_NEWLINE;
	transmitString_F(PSTR("                                           "));
	TX_NEWLINE;
	
	transmitString("PRESS ANY KEY to proceed...");
	
	
	
	initialise();
	
	numberofbottles1=eeprom_read_byte(0);
	bottles_filled=eeprom_read_byte(1);
	time_off=eeprom_read_byte(2);
	time_on=eeprom_read_byte(3);
	 storagecontainer=eeprom_read_byte(4); 
	 currentoranges1=eeprom_read_byte(5);
	 currentpineapples1=eeprom_read_byte(6);
	 currentguavas1=eeprom_read_byte(7);
	 currentmangoes1=eeprom_read_byte(8);
	 currentapples1=eeprom_read_byte(9);
	 
	
	
	
	PORTB |=(1<<1);
	
	
	
		while(1)
    {
			if((PINB &(1<<1))==0){
				if (storagecontainer != 0){
					
				
				if (numberofbottles1 != 0 )
				{ 
				
				
			PORTB=0x02;
	clearscreen();
	lcd_position(1);
	lcd_write_string("**STATUS*");
	lcd_position(2);
	lcd_write_string(" SENDING BOTTLE ");
	lcd_position(3);
	lcd_write_string(" TO STAGE");
	_delay_ms(2000);
	motor();
	time_on++;
	eeprom_write_byte(3,time_on);
	
	
	
	
	clearscreen();
	lcd_position(1);
	lcd_write_string("**STATUS*");
	lcd_position(2);
	lcd_write_string(" FILLING BOTTLE");
	PORTA=2;
	_delay_ms(1000);
	PORTA=1;
	_delay_ms(1000);
	PORTA=0;
	
	////////////////////////////////DECREMENT BOTTLES
					bottles_filled++;
					numberofbottles1--;
					eeprom_write_byte(0,numberofbottles1);
					eeprom_write_byte(1,bottles_filled);
					 storagecontainer=storagecontainer-(selectedvolume);
					 eeprom_write_byte(4,storagecontainer);
/////////////////////////////////////////////////	
	
	
	///////////////////////
	
	
	
	clearscreen();
	lcd_position(1);
	lcd_write_string("**STATUS*");
	lcd_position(2);
	lcd_write_string("MOVING BOTTLE TO");
	lcd_position(3);
	lcd_write_string("SEALING STAGE");
	_delay_ms(2000);
	motor();
	
	
	clearscreen();
	lcd_position(1);
	lcd_write_string("**STATUS*");
	lcd_position(2);
	lcd_write_string("SEALING BOTTLE");
	PORTA=1;
	_delay_ms(1000);
	PORTA=0;
	
	
	clearscreen();
	lcd_position(1);
	lcd_write_string("**STATUS*");
	lcd_position(2);
	lcd_write_string("MOVING BOTTLE");
	lcd_position(3);
	lcd_write_string("TO STORAGE");
		 motor();

		 motor();
		 
		}
		
		
		
		else{
			PORTB=0x01;
			clearscreen();
			lcd_position(1);
	lcd_write_string("*COMPLETED");
	_delay_ms(1000);
	lcd_position(2);
	lcd_write_string(" RE-CONFIGURE");
	_delay_ms(1000);
	lcd_position(3);
	lcd_write_string("THE MACHINE");
			_delay_ms(1000);
			
				numberofbottles=0;
				bottles_filled=0;
				time_off=0;
				time_on=0;
				
			
		} 
		
		} else{	
				
			
				clearscreen();
				lcd_position(1);
				lcd_write_string("*PLEASE");
				_delay_ms(1000);
				lcd_position(2);
				lcd_write_string("1ST CONFIGURE");
				_delay_ms(1000);
				lcd_position(3);
				lcd_write_string("THE MACHINE");
				_delay_ms(1000);
				
				
				
		
		}
		 
		 
		 
		} else{	
		if (time_on != 0 )
		{PORTB=0x01;
			clearscreen();
			lcd_position(1);
			lcd_write_string("*STATUS*");
			_delay_ms(1000);
			lcd_position(2);
			lcd_write_string("PLANT");
			_delay_ms(1000);
			lcd_position(3);
			lcd_write_string("RESTING");
			_delay_ms(1000);
		time_off++;
		eeprom_write_byte(2,time_off);
}else{
	PORTB=0x01;
	clearscreen();
	
	}
	}
	
    }
		
	
}
