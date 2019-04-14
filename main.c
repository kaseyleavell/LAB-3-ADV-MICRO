#include <msp430.h> 


/**
 * LAB 3 PWM main.c
 */
int rowOut;
char out[4] = {0x00,0x01,0x02,0x03};
char checkBit[4] = {0x02,0x04,0x08,0x10};
int keypad[4][4] = {{1,65,129,0},{193,257,321,0},{384,448,511,0},{0,0,0,0}};

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1DIR |= 0x01;                            // P2.2 output
    P2DIR |= 0xFF;
    P1REN |= 0x1E;              //initialize pulling resistors
    P1OUT |= 0x1E;              //initialize resistors as pull up
    P1IE |=0x1E;                //set the interrupt to activate for
    P1IES |= 0x1E;              //Pins hi to lo edge triggering
    P1IFG &=~ 0xFF;             //clear the interrupt flagP1DIR |= 0x41;
    P1SEL |= 0x01;                            // P2.2 TA1.1 option
    P1SEL2 &= ~0x01;                        // P2.2 TA1.1 option
    TA1CCR0 = 512;                             // PWM Period
    TA1CCTL1 = OUTMOD_6;                         // TA1CCR1 reset/set
    TA1CTL = TASSEL_2 + MC_3;                  // SMCLK, up mode

    __enable_interrupt();
    while(1)
    {
        for(rowOut = 0;rowOut < 4;rowOut++)
        {
             P2OUT &=~ 0xFF;     //clear the P2.X outputs. This method will also momentarily check the first button again
             P2OUT |= out[rowOut];
             __delay_cycles(10000);
        }
    }
    return 0;
}
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    __disable_interrupt();
    __delay_cycles(300000);
    int poll = P1IN;
    while(~poll & P1IFG)
    {
        poll = P1IN;
    }
    int inputValue;
    char dutyStor;
    inputValue = checkInput();
    dutyStor = int2Duty(inputValue);
    TA1CCR1 = dutyStor;
    P1IFG &=~ checkBit[inputValue];
    __enable_interrupt();
}
int checkInput()
{
    int lowBit;
    for(lowBit = 0;lowBit < 4;lowBit++)
    {
        if(P1IFG & (checkBit[lowBit]))
        {
            break;                      //locate the bit that is low and break on that bit
        }
    }
    P1IN &=~ 0xFF;
    return lowBit;
}
int int2Duty(int input)
{
    /*This function is specific to the LCD and keypad lab
     * it will not work in a scenerio where the int 2 char is needed
     */
    int numOut = 0;
    char output;
    if(P2OUT & 0x01)
    {
        numOut |= 0x01;
    }if(P2OUT & 0x02)
    {
        numOut |= 0x02;
    }
    output = keypad[numOut][input];
    return output;
}
