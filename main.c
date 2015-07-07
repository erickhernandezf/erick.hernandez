/*
@author:        Walter Gonzalez Domenzain
@description:   8020 Group
@functions:     None
@environment:   KL25Z
@date:          27/05/2015
@comments:
- Board red led 	connected to B18 	(turns on with 0) 
- Board green led 	connected to B19 	(turns on with 0) 
- Board blue led 	connected to D1  	(turns on with 0) 
- Push button 1 	connected to B3		(sends 0 when pressed)      
@version:       1.0 - Initial
*/
 
#include "derivative.h" 
//Ports
#define setPortB(x)     (GPIOB_PDOR |= (1 << x))
#define clearPortB(x)   (GPIOB_PDOR &= ~(1 << x))
#define setPortD(x)     (GPIOD_PDOR |= (1 << x))
#define clearPortD(x)   (GPIOD_PDOR &= ~(1 << x))
 
//LCD Signals
#define portLCD             GPIOC_PDOR
#define output_RS(x)        if(x){GPIOB_PDOR |= (0x00000001);}else{GPIOB_PDOR &= (0xFFFFFFFE);}
#define output_Enable(x)    if(x){GPIOB_PDOR |= (0x00000002);}else{GPIOB_PDOR &= (0xFFFFFFFD);}
 
#define high_RS             (GPIOB_PDOR |= (0x00000001))    
#define low_RS              (GPIOB_PDOR &= (0xFFFFFFFE))
#define high_Enable         (GPIOB_PDOR |= (0x00000002))    
#define low_Enable          (GPIOB_PDOR &= (0xFFFFFFFD))
 
#define nIns                0
#define nData               1
 
#define readPortB(x)        ((GPIOD_PDIR &= (1 << x)) >> x)
#define nButtonPressed      0
#define nButtonFree         1 
 
#define btn1                3
 
//Leds
#define turnOnRedLed    clearPortB(18)
#define turnOffRedLed   setPortB(18)
#define turnOnGreenLed  clearPortB(19)
#define turnOffGreenLed setPortB(19)
#define turnOnBlueLed   clearPortD(1)
#define turnOffBlueLed  setPortD(1)
 
//Time
#define n1sec           2000000  
#define n2sec           4000000
#define n500msec        1000000 
#define n15msec         35000
#define n40usec         3500 
//--------------------------------------------------------------
//Declare Prototypes
void cfgPorts(void);
void delay(long ltime);
void initLCD(void);
void cfgPWM(void);
void cfgADC(void);
void sendCode(int Code, int Data);
    
//Cursor Blink off initialization
const unsigned char InitializeLCD[5] = {0x38, 0x38, 0x38, 0x0C, 0x01};
    
int main(void)
{   
    cfgPorts();
    initLCD();
    cfgPWM();
    cfgADC();
    
    int iData = 0;
    int iUnidades;
    int iDecenas;
    int iCentenas;
 
    for(;;)
    {
        if((ADC0_SC1A & ADC_SC1_COCO_MASK) == ADC_SC1_COCO_MASK)
        {
            iData = ADC0_RA;
            iCentenas = iData/100;
            iData %= 100;
            iDecenas = iData/10;
           	iUnidades = iData % 10;
           	sendCode(nIns, 0xC6);
            sendCode(nData, iCentenas + 0x30);
            sendCode(nData, iDecenas + 0x30);
            sendCode(nData, iUnidades + 0x30);
            sendCode(nIns, 0x80);
            sendCode(nData, 'T');
            sendCode(nData, 'E');
            sendCode(nData, 'M');
            sendCode(nData, 'P');
            sendCode(nData, 'E');
            sendCode(nData, 'R');
            sendCode(nData, 'A');
            sendCode(nData, 'T');
            sendCode(nData, 'U');
            sendCode(nData, 'R');
            sendCode(nData, 'E');
            sendCode(nData, 0x20);
            sendCode(nData, 'I');
            sendCode(nData, 'S');
            sendCode(nData, ':');
            delay(n1sec);
            cfgADC();
        }
         
    } 
    return 0;
}
 //--------------------------------------------------------------
void cfgPorts(void) {
    //Turn on clock for portb and portd
    SIM_SCGC5 = SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; 
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM_SCGC6 = SIM_SCGC6_ADC0_MASK;    
      
    /* Set pins of PORTB as GPIO */
    //Connected to LCD
    PORTC_PCR0  = PORT_PCR_MUX(1);
    PORTC_PCR1  = PORT_PCR_MUX(1);
    PORTC_PCR2  = PORT_PCR_MUX(1);
    PORTC_PCR3  = PORT_PCR_MUX(1);
    PORTC_PCR4  = PORT_PCR_MUX(1);
    PORTC_PCR5  = PORT_PCR_MUX(1);
    PORTC_PCR6  = PORT_PCR_MUX(1);
    PORTC_PCR7  = PORT_PCR_MUX(1);
     
    //LCD: RS and Enable
    PORTB_PCR0  = PORT_PCR_MUX(1);
    PORTB_PCR1  = PORT_PCR_MUX(1);
     
    //Leds on board
    PORTB_PCR18 = PORT_PCR_MUX(1);
    PORTB_PCR19 = PORT_PCR_MUX(1);
    PORTD_PCR1  = PORT_PCR_MUX(1);
     
    //PORTE as ADC0_DP0/ADC0_SE0 */
    PORTE_PCR20 = PORT_PCR_MUX(0);
     
    //PWMs
    PORTE_PCR21 = PORT_PCR_MUX(3);
    PORTE_PCR22 = PORT_PCR_MUX(3);
    PORTE_PCR23 = PORT_PCR_MUX(3);
     
    //Configure all PortB as outputs
    GPIOB_PDDR = 0xFFFFFFFF; //1111 1111 1111 1111 1111 1111 1111 1111
      
    //Configure all PortC as outputs
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
    for(i=0; i<5; i++)
    {                                       
        sendCode(nIns, InitializeLCD[i]);   /* send initialization instructions */         
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
 
void cfgPWM(void) {
    //Select the CLK for the TPM Module - page 196
    SIM_SOPT2 = SIM_SOPT2_TPMSRC(1);
     
    // Selects the MCGFLLCLK clock 
    SIM_SOPT2 &= ~(SIM_SOPT2_PLLFLLSEL_MASK);
    //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
 
    //Turn on clock for TPM0
    SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;   
 
    //Turn on clock for TPM0
    SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;  
     
    //Clear counter register - page 553
    TPM1_CNT = 0;
     
    //Set signal period to x ms
    TPM1_MOD = 1824;
     
    //See page 552 for TPMx_SC configuration
    //(freq = x) //0 1011
    //LPTPM counter increments on every LPTPM counter clock
    //CLK divided by 8
    TPM1_SC = 0x0B;         
 
    //Duty cycle E20 = 36%
    TPM1_C0V = 492;
    //Duty cycle E21 = 64%
    TPM1_C1V = 10100;
 
    //See page 556 for TPMx_CnSC configuration
    TPM1_C0SC = 0x28;       //0010 1000
    TPM1_C1SC = 0x28;       //0010 1000
}
 
void cfgADC(void){
    //ADC Configuration Register 1 (ADCx_CFG1) page 465
    ADC0_CFG1 = 0x00; //0000 0000   
     
    //ADC Configuration Register 2 (ADCx_CFG2) page 467
    //Channel A selected
    ADC0_CFG2 = 0x00;   //0 0000    
     
    //Status and Control Register 2 (ADCx_SC2) page 470
    ADC0_SC2 = 0x00;    //0000 0000     
     
    //Status and Control Register 3 (ADCx_SC3)
    ADC0_SC3 = 0x80;    //1000 0000
     
    //ADC Status and Control Registers 1 - page 462
    //This registers triggers the ADC conversion
    ADC0_SC1A = 0x00;   //000
}


