#include "msp430g2553.h"

#define TXLED BIT0
#define RXLED BIT6
#define TXD BIT2
#define RXD BIT1

void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength);

static volatile char data;


int main(void)
{
   WDTCTL = WDTPW + WDTHOLD; // Stop WDT

   DCOCTL = 0; // Select lowest DCOx and MODx settings
   BCSCTL1 = CALBC1_1MHZ; // Set DCO
   DCOCTL = CALDCO_1MHZ;

   P2DIR |= 0xFF; // All P2.x outputs
   P2OUT &= 0x00; // All P2.x reset
   P1SEL |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
   P1SEL2 |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
   P1DIR |= RXLED + TXLED;
   P1OUT &= RXLED + TXLED;

   UCA0CTL1 |= UCSSEL_2; // SMCLK
   UCA0BR0 = 104; // 1MHz 115200
   UCA0BR1 = 0; // 1MHz 115200
   UCA0MCTL = UCBRS0; // Modulation UCBRSx = 0
   UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
   IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

   __bis_SR_register(LPM0_bits + GIE); // Enter LPM0 w/ int until Byte RXed

}




#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    data = UCA0RXBUF;

    switch(data){
    case 'E':
    {


    }
    break;

    case 'e':
    {
        P1OUT ^= BIT0;
    }
    break;

    default:
    {
        P1OUT ^= BIT6;
        UARTSendArray("Received command: ", 18);
        UARTSendArray(&data, 1);
        UARTSendArray("\n\r" , 2);

    }
    break;
    }
}


void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength)
{
    while(ArrayLength--){       //loop until string length == 0
        while(!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = *TxArray;
        TxArray++;

    }
}
