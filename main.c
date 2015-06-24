/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "derivative.h" /* include peripheral declarations */

//Ports
#define setPortB(x) 	(GPIOB_PDOR |= (1 << x))
#define clearPortB(x) 	(GPIOB_PDOR &= ~(1 << x))
#define setPortD(x) 	(GPIOD_PDOR |= (1 << x))
#define clearPortD(x) 	(GPIOD_PDOR &= ~(1 << x))

//LCD Signals
#define portLCD			 GPIOC_PDOR
#define output_RS(x)	 if(x){GPIOB_PDOR |= (0x00000001);} else{GPIOB_PDOR &= (0xFFFFFFFE);}
#define output_Enable(x) if(x){GPIOB_PDOR |= (0x00000002);} else{GPIOB_PDOR &= (0xFFFFFFFD);}

#define high_RS			(GPIOB_PDOR |= (0x00000001))	
#define	low_RS			(GPIOB_PDOR &= (0xFFFFFFFE))
#define high_Enable		(GPIOB_PDOR |= (0x00000002))	
#define	low_Enable		(GPIOB_PDOR &= (0xFFFFFFFD))

#define	nIns			0
#define	nData			1


#define readPortB(x)	((GPIOD_PDIR &= (1 << x)) >> x)
#define	nButtonPressed  0
#define	nButtonFree     1 

#define btn1			3

//Leds
#define turnOnRedLed	clearPortB(18)
#define turnOffRedLed	setPortB(18)
#define turnOnGreenLed	clearPortB(19)
#define turnOffGreenLed	setPortB(19)
#define turnOnBlueLed	clearPortD(1)
#define turnOffBlueLed	setPortD(1)

//Time
#define	n1sec			2000000  
#define	n2sec			4000000
#define	n500msed		1000000 
#define n15msec			35000
#define n40usec			3500

//--------------------------------------------------------------
//Declare Prototypes

void cfgPorts(void);
void delay(long ltime);
void initLCD(void);
void sendCode(int Code, int Data);
   
//Cursor Blink off initialization
const unsigned char InitializeLCD[5] = {0x38, 0x38, 0x38, 0x0C, 0x01};
   
int main(void)
{	
	cfgPorts();
	initLCD();
	
	unsigned char Unidades= 0x30;
	unsigned char Decenas= 0x30;
	unsigned char Centenas= 0x30;	
	
	for(;;)
    {
		Unidades++;
		delay(n1sec);
		
		if(Unidades==0x3A)
		{
			Unidades=0x30;
			Decenas++;
		}
		
		sendCode(nIns, 0x88);
		sendCode(nData, Unidades);
		
		if(Decenas==0x3A)
		{
			Decenas=0x30;
			Centenas++;
		}
		
		sendCode(nIns, 0x87);
		sendCode(nData, Decenas);
		
		if(Centenas==0x3A)
		{
			Centenas=0x30;
		}
		
		sendCode(nIns, 0x86);
		sendCode(nData, Centenas);
    } 
    return 0;
}
 //--------------------------------------------------------------
void cfgPorts(void)
{
    //Turn on clock for portb and portd
    SIM_SCGC5 = SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;   
     
    /* Set pins of PORTB as GPIO */
    //Connected to LCD
	PORTC_PCR0 	= PORT_PCR_MUX(1);
	PORTC_PCR1 	= PORT_PCR_MUX(1);
	PORTC_PCR2 	= PORT_PCR_MUX(1);
	PORTC_PCR3 	= PORT_PCR_MUX(1);
	PORTC_PCR4 	= PORT_PCR_MUX(1);
	PORTC_PCR5 	= PORT_PCR_MUX(1);
	PORTC_PCR6 	= PORT_PCR_MUX(1);
	PORTC_PCR7 	= PORT_PCR_MUX(1);
	
	//LCD: RS and Enable
	PORTB_PCR0 	= PORT_PCR_MUX(1);  //Rs
	PORTB_PCR1 	= PORT_PCR_MUX(1);  //E
	
	//Leds on board
	PORTB_PCR18 = PORT_PCR_MUX(1);
    PORTB_PCR19 = PORT_PCR_MUX(1);
	PORTD_PCR1 	= PORT_PCR_MUX(1);
     
    //Configure all PortB as outputs
    GPIOB_PDDR = 0xFFFFFFFF; //1111 1111 1111 1111 1111 1111 1111 0111
     
    //Configure all PortD as outputs
    GPIOC_PDDR = 0xFFFFFFFF;
	
	//Configure all PortD as outputs
    GPIOD_PDDR = 0xFFFFFFFF;
}
//--------------------------------------------------------------
void delay(long ltime)
{
	while (ltime > 0)
	{
		ltime--;
	}
}
//--------------------------------------------------------------
void initLCD(void)
{
	int i;
	delay(n15msec);
	
	/* Send initialization instructions */
	/* Loop for sending each character from the array */
	for(i=0;i<5;i++)
	{										
		sendCode(nIns, InitializeLCD[i]);	/* send initialization instructions */			
	}
}
//--------------------------------------------------------------
void sendCode(int Code, int Data)
{
	//Assign a value to pin RS
	/*HINT: When RS is 1, then the LCD receives a data
	when RS is 0, then the LCD receives an instruction */
	// Initialize RS and Enable with 0
	low_RS;
	low_Enable;
	
	//Assign the value we want to send to the LCD
	portLCD = Data;	
	
	//We make the algorithm to establish if its an instruction we start with 0 on RS value, otherwise if its a data command we start with RS as 1;
	if (Code == nIns)
	{
		low_RS;
		high_Enable;
		delay(n40usec);
		low_Enable;
		low_RS;
	}		
	else if (Code == nData)
	{
		high_RS;
		high_Enable;
		delay(n40usec);
		low_Enable;
		low_RS;
	}
}
