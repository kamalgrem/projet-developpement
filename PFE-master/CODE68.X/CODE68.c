#include "config.c"        

#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low Voltage In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#include <xc.h>
#include<pic16f877a.h>
#define _XTAL_FREQ 4000000
#define Baud_rate 9600

char voltage,current, temp,channel,conversion,data;
int rpt;
unsigned short int somme ;

void ADC_Init();
void Initialize_UART(void);
unsigned int ADC_Read( unsigned int channel);
void ADC();

void ADC_Init()
{
  ADCON0 = 0x81;               //Turn ON ADC and Clock Selection
  ADCON1 = 0x40;               //All pins as Analog Input and setting Reference Voltages
}

unsigned int ADC_Read( unsigned int channel)
{
  if(channel > 7)              //Channel range is 0 ~ 7
    return 0;

  ADCON0 &= 0x85;              //Clearing channel selection bits
  ADCON0 |= channel<<3;

  //Setting channel selection bits
  
  __delay_ms(2);               //Actmaxuisition time to charge hold capacitor
  GO_nDONE = 1;                //Initializ1es A/D conversion
  while(GO_nDONE);             //Waiting for conversion to complete
  return (ADRESH); //Return result
}
void ADC()
{
    
   somme=0;
   conversion=0;
   data=0;
   
   __delay_ms(10);
    for(int i=0; i<40;i++)
    {
         conversion=ADC_Read(channel);
         __delay_ms(10);
        somme=somme+conversion;
   
    
    }
    data=somme/40;
    
    somme=0;
}


void Initialize_UART(void)
{

    TRISC6 = 0; // TX Pin set as output
    TRISC7 = 1; // RX Pin set as input
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required int rpt,m;

    baud rate and set BRGH for fast baud_rate**/
    SPBRG = ((_XTAL_FREQ/16)/Baud_rate) - 1;
    BRGH  = 1;  // for high baud_rate
  
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
   

    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception

    
 
    TX9   = 0;    
    RX9   = 0;   
       
}

//_____
void UART_send_char(char bt)  
{
   
    TXREG = bt; 
}

void main (void )
{
    
    TRISB=0x00;
    TRISD=0x00;
    TRISA=0xff;
    TRISE=0xff;
    TRISC=0x00;
    
    Initialize_UART();    //Initialize UART module   
    ADC_Init();

    __delay_ms(600);
       

 
    while(1)
    {
     
        
  
        for(rpt=0;rpt<5;rpt++)
        {
            
           
            __delay_ms(100);
            channel=0;  
            ADC();
            temp=data;
            
            __delay_ms(10);
             
             UART_send_char('p');
           __delay_ms(10);
             UART_send_char('m');
            __delay_ms(10);
            UART_send_char(temp);
           __delay_ms(10);
          
          
           channel=1;
           ADC();
           voltage=data;
           __delay_ms(10);
           UART_send_char('s');
           __delay_ms(10);
           UART_send_char('k');
          
           __delay_ms(10);
           
           UART_send_char(voltage);
       
          
         
           __delay_ms(10);
           channel=2;
            ADC();
           current=data;
           __delay_ms(10);
           UART_send_char('f');
          __delay_ms(10);
           UART_send_char('h');     
          __delay_ms(10);
           UART_send_char(current);
          __delay_ms(10);
         
        }
        if(voltage> 230) // if the charge voltage reach 13.5 V 
            RB0=1;       // CIRCUIT CHARGE 
        else 
            if(voltage <161  ) // if the decharge voltage reach 9.5 V
     
                RB0=0;   // CIRCUIT DECHARGE 
    }
        
            
    
        
       
        

}
