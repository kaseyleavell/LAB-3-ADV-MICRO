#include <msp430.h>


/**
 * LAB 3 PWM main.c
 */
int rowOut;
char out[4] = {0x00,0x01,0x02,0x03};
char checkBit[4] = {0x02,0x04,0x08,0x10};
int keypad[4][4] = {{1000,1000,1000,0},
                    {1550,1500,1550,0},
                    {2000,2000,2000,0},
                    {0,0,0,0}};
int checkInput();
int int2Duty(int input);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer


    P1DIR |= 0x40;                            // P2.2 output
    P1SEL |= 0x40;                            // P2.2 TA1.1 option
    P1SEL2 &= ~0x40;                        // P2.2 TA1.1 option
    TA0CCR0 = 20000;                             // PWM Period
    TA0CCTL1 = OUTMOD_7;                         // TA1CCR1 reset/set
    TA0CCR1 = 1500;                               // TA1CCR1 PWM duty cycle
    TA0CTL = TASSEL_2 + MC_1;                  // SMCLK, up mode

    P2DIR |= 0x14;
    P2SEL |= 0x14;
    P2SEL2 &=~ 0x14;
    TA1CCR0 = 18800;
    TA1CCTL1 = OUTMOD_7;
    TA1CCR1 = 1500;
    TA1CCR2 = 1500;
    TA1CTL = TASSEL_2 + MC_1;

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
    if(inputValue == 0)
    {
        TA1CCR2 = dutyStor;     //for the left rotational servo
    }else if(inputValue == 1)
    {
        TA0CCR1 = dutyStor;     //right rotational servo
    }else if(inputValue == 2)
    {
        TA1CCR1 = dutyStor;     //position servo
    }
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
