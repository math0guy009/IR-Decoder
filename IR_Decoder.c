#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>

#pragma config FOSC = HS // Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // RA5/MCLR pin function select (RA5/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOD Reset disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Code Protection bits (Program memory code protection off)

//crystal frequency
#define _XTAL_FREQ 8000000
int resultH = 0;
int resultM = 0;
int resultL = 0;
int toggle = 0;

/***************************************************************************   
 *                          MAIN ROUTINE
 * *************************************************************************
 * *************************************************************************
*/
void main (void){
    
    init ();
     __delay_ms (3500); //allow for SUU LCD to boot up with splash screen
    while(1){           //Main Home Screen
        printf("IR Decoder")     ;
        __delay_ms(200);
        clear_lcd();
   }
}
/***************************************************************************   
 *                          INITIALIZATION ROUTINE
 * *************************************************************************
*/
int init()//initialize pins and ports to correct configuration
{
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    //I/O set up for UART pins for PIC16F886 AND 887
    TRISA = 0b11100011;
    TRISB = 0b00000001;
    TRISC = 0b01101111;
    RC4 = 0;        //
    GIE = 1;        //Global interrupt bit
    INTE = 1;       //enables Interrupt RB0
    INTF = 0;
    T0CS = 0;       //timer 0 clock source set to Internal instruction cycle clock (FOSC/4)
    T0SE = 0;       // TMR0 Source Edge Select bit 0 = Increment on low-to-high transition on T0CKI pin
    PSA = 1;        // Prescaler Assignment bit turned to a 1 for a 1:1 Ratio for TIMER0
    RBIE = 1;       //Port B interrupt enabled
    IOCB0 = 1;      //Interrupt on Change Bit enabled
    PEIE = 1;       //Periferial interrupt enabled on page 221 of data sheet
       //SETS THE  Internal Oscillator Frequency TO 8 MHz instead of the default 4MHz
    IRCF2 = 1;     
    IRCF1 = 1;     // this is the max frequency the chip can handle without going to a external oscillator 
    IRCF0 = 1;     // like a 20 KHz crystal
       
      //Setting up baudrate and RX configuration
    SPBRG = 12;  //9600 baud rate with 0.16% error rate
    BRGH  = 0;   //High baud rate generator
    BRG16 = 0;   //16-BIT baud rate generator
    SYNC  = 0;   //Eusart asynchronous mode
    SPEN  = 1;   //Serial port enable
    SREN  = 0;   //Single receive enable bit
    CREN  = 1;   //Continuous receive enable bit
    TXEN  = 1;
}
void detectCode(int data[])//writes decoded string onto lcd
{
    clear_lcd();
    top_line();
   for(int i = 0; i < 24; i++)
    {
        if(data[i] == 0)
           printf("0");
        else
           printf("1");
        if(i==9) next_line();
   }
    
    //**********************************************************************
                        //REMOTE CONTROLS CONFIGURATION
    //**********************************************************************
    __delay_ms(1000);
    clear_lcd();
    resultH = (data[0]<<9 | data[1]<<8 | data[2]<<7 | data[3]<<6 | data[4]<<5 | data[5]<<4 | data[6]<<3 | data[7]<<2 | data[8]<<1 | data[9] );
    resultM = (data[10]<<9 | data[11]<<8 | data[12]<<7 | data[13]<<6 | data[14]<<5 | data[15]<<4 | data[16]<<3 | data[17]<<2 | data[18]<<1 | data[19]<<0 | data[20] );//| data[21]<<3 | data[22]<<2 | data[23]<<1 | data[24]);
    resultL = (data [20]<<3 | data[21]<<2 |data[22]<<1 | data[23]);
    int addr = ((resultM <<4) | resultL);
    
    printf("addr:%x",addr);
    next_line();
    printf("cmd:%x",resultH);
    
    if (resultH == 0x3cb && addr == 0x10d2){        //UP CHANNEL
        next_line();
        printf("cmd:%x    up ", resultH);
        __delay_ms(2000);
        printf(resultH,addr);
        }
    
    if (resultH == 0x3cb && addr == 0xd3){          //DOWN CHANNEL
        next_line();    
        printf("cmd:%x    down ", resultH);
        }
    
    if (resultH == 0x3cb && addr == 0x30d0){        //VOLUME UP (RIGHT ARROW)
        next_line();
        printf("cmd:%x   right ", resultH);
        }
    if (resultH == 0x3cb && addr == 0x20d1){        //VOLUME DOWN (LEFT ARROW)
        next_line();
        printf("cmd:%x   left ", resultH);
        }
    if (resultH == 0x3cc && addr == 0x10de){        //BUTTON ONE
        next_line();
        printf("cmd:%x     B1 ", resultH);
        }
    if (resultH == 0x3cc && addr == 0x20dd){        //BUTTON TWO
        next_line();
        printf("cmd:%x     B2", resultH);
        }
    if (resultH == 0x3cc && addr == 0x30dc){        //BUTTON THREE
        next_line();
        printf("cmd:%x     B3", resultH);
        }
    if (resultH == 0x385 && addr == 0x11fa){        //PLAY BUTTON
        next_line();
        printf("cmd:%x   PLAY", resultH);
        }
    if (resultH == 0x387 && addr == 0x31e0){        //STOP BUTTON
        next_line();
        printf("cmd:%x   STOP", resultH);
        }
    if (resultH == 0x3cd && addr == 0xdb){        //BUTTON FOUR
        next_line();
        printf("cmd:%x   B4", resultH);
        }
    if (resultH == 0x3cd && addr == 0x10da){        //BUTTON FIVE
        next_line();
        printf("cmd:%x   B5", resultH);
        }
    if (resultH == 0x3cd && addr == 0x20d9){        //BUTTON SIX
        next_line();
        printf("cmd:%x   B6", resultH);
        }
    if (resultH == 0x3cd && addr == 0x30d8){        //BUTTON SEVEN
        next_line();
        printf("cmd:%x   B7", resultH);
        }
    if (resultH == 0x3ce && addr == 0xc7){        //BUTTON EIGHT
        next_line();
        printf("cmd:%x   B8", resultH);
        }
     if (resultH == 0x3ce && addr == 0x10c6){        //BUTTON NINE
        next_line();
        printf("cmd:%x   B9", resultH);
        }
     if (resultH == 0x3cc && addr == 0xdf){        //BUTTON ZERO
        next_line();
        printf("cmd:%x   B0", resultH);
        __delay_ms(250);
        clear_lcd();
        printf("BOOM BABY");
        RB2 = 1;
        __delay_ms(500);
        RB2 = 0;
        }
    //////////////////////////////////////////////////////////////////////////
    //POWER BUTTON
    //////////////////////////////////////////////////////////////////////////
    if (resultH == 0x3ca && addr == 0x20d5){        //POWER BUTTON
        next_line();
        printf("cmd:%x  POWER", resultH);
        clear_lcd();
        
        if (toggle==0) {
            RC4 =1;
            RB1 = 1;
            toggle = 1;
            printf("cmd:%x POWER ON", resultH);
            addr = 0;
        }
        else{
            RC4 = 0;
            RB1 = 0;
            toggle = 0;
            printf("cmd:%x PWR OFF", resultH);
            }
    }
    
}


int data[24] = 0;      //holds data while code is being decoded
int starting_bit = 0;         //holds whether decoding has starting_bit
int bits_recieved = 0;  //bits that have been received
int time_count = 0;   //the current time since the last bit was received

/***************************************************************************   
 *                  INTERRUPT ROUTINE & DECODING PROCESSES
 * *************************************************************************
 * * *************************************************************************
*/

void interrupt interruptRoutine()//handles interrupts
{
    
    if (RBIF)           //on positive change of RB0 'raises' the flag, set in the Init()
    {
       if(RB0)                 //on ir signal received
        {
            T0IE = 1;           //turn on counting
            TMR0 = 255;         //preset counter
            time_count = 0;   //reset time
        }
        else                    //on falling edge
        {
            T0IE = 0;           //disable timer
            
            if(!starting_bit)          //wait for initial starting code
            {
                if(time_count >= 1 && time_count <= 100)
                {
                    starting_bit = 1;
                }
                else
                {
                    starting_bit = 0;
                    bits_recieved = 0;
                }
            }
            else //decode a bit in the series of bits
            {
                if(time_count >0 && time_count <22)
                {
                    data[bits_recieved] = 0;
                }
                else if(time_count >=23 && time_count <= 42)
                {
                    data[bits_recieved] = 1;
                }
                else
                {
                    starting_bit = 0;
                    bits_recieved = 0;
                }

                if(bits_recieved == 23)//when code is fully decoded print result
                {
                    detectCode(data);
                    starting_bit = 0;
                    bits_recieved = 0;
                }
                else
                {
                   bits_recieved++;//increment bit count
                  
                }
            }
        }

        RBIF = 0;                   //reset interrupt flag
    }
    else if(T0IF)
    {
        //RC4 = 1;
        time_count = time_count + 1;       //increase clock time
        //RC4 = 0;
        TMR0 = 179;                        //preset timer will give me 50 uS
        T0IF = 0;                          //reset interrupt flag 
     
    if(time_count>150)                     //abort after timeout
        {
            T0IE = 0;               //turn off timer interrupt
            starting_bit = 0;       //reset 'starting_bit' to zero
            bits_recieved = 0;      //back to 0 bits
        }
    }
}
/***************************************************************************   
 *                  STANDARD LCD SCREEN COMMANDS AND CONTROLS
 * *************************************************************************
 * * *************************************************************************
*/
clear_lcd()
    {
    putchar (0xFE);
    putchar (0x01);
    __delay_ms (10);
    }
void putch (unsigned char byte)
{
    while (!TRMT); //Needed to make 'printf' statement work
    TXREG = byte;   //wait until TX buffer is empty
    }
next_line ()
{
    //SUU LCD 
    putchar (0xFE);
    putchar (0xC0); //codes to locate cursor at position 0 on line 2

}
top_line ()
{
    //SUU LCD 
    putchar (0xFE);
    putchar (0x80); //codes to locate cursor at position 0 on line 1
    
} 