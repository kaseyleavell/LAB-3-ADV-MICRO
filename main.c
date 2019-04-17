#include  <msp430g2553.h>



#include <msp430.h>


/**
 * LAB 3 PWM main.c
 */
int rowOut;
char out[4] = {0x00,0x01,0x02,0x03};
char checkBit[4] = {0x02,0x04,0x08,0x10};
int keypad[4][4] = {{2222,4444,6666,0},
                    {8888,11000,13222,0},
                    {15444,19666,20000,0},
                    {0,0,0,0}};
int checkInput();
int int2Duty(int input);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer


    P2DIR |= 0x04;                            // P2.2 output
    P2SEL |= 0x04;                            // P2.2 TA1.1 option
    P2SEL2 &= ~0x04;                        // P2.2 TA1.1 option
    TA1CCR0 = 20000;                             // PWM Period
    TA1CCTL1 = OUTMOD_7;                         // TA1CCR1 reset/set
    TA1CCR1 = 1;                               // TA1CCR1 PWM duty cycle
    TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, up mode

    P2DIR |= 0xFB;
    P1REN |= 0x1E;              //initialize pulling resistors
    P1OUT |= 0x1E;              //initialize resistors as pull up
    P1IE |=0x1E;                //set the interrupt to activate for
    P1IES |= 0x1E;              //Pins hi to lo edge triggering
    P1IFG &=~ 0xFF;             //clear the interrupt flagP1DIR |= 0x41;
    __enable_interrupt();
    while(1)
    {
        for(rowOut = 0;rowOut < 4;rowOut++)
        {
             P2OUT &=~ 0xFB;     //clear the P2.X outputs. This method will also momentarily check the first button again
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
    int dutyStor;
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
    int output;
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

