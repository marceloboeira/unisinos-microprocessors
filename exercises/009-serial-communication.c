
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/gpio.h>
#include <rit128x96x4.h>

#define DEFAULT_STRENGTH GPIO_STRENGTH_2MA
#define DEFAULT_PULL_TYPE GPIO_PIN_TYPE_STD_WPU
#define PORT_E GPIO_PORTE_BASE
#define PORT_F GPIO_PORTF_BASE
#define PIN_0 GPIO_PIN_0
#define PIN_1 GPIO_PIN_1
#define PIN_2 GPIO_PIN_2
#define PIN_3 GPIO_PIN_3
#define HIGH 0xFF
#define LOW 0x00
#define B_HIGH 'a'
#define B_LOW 'b'
#define UART_BASE UART1_BASE
#define SYS_UART SYSCTL_PERIPH_UART1

int btnA = 0,
    led = 0,
    lastOutputChar = B_LOW;

void delay(int time) {
  int i = 0;
  for (i = 0; i < time * 250000; i++);
}

void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

  GPIOPinTypeGPIOInput(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  GPIOPinTypeGPIOOutput(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                         SYSCTL_XTAL_8MHZ);

  SysCtlPeripheralEnable(SYS_UART);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);

  UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), 115200,
		  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void keyboardHandler() {
  btnA = 0;
  btnA = (GPIOPinRead(PORT_E, PIN_0) & 0x01) != 0x01;
}

int main(void) {
    setup();
    char buffer;
    char string[30];

    UARTCharPut(UART_BASE, '!');
    RIT128x96x4Init(1000000);

    while (1) {
      buffer = UARTCharGetNonBlocking(UART_BASE);
      if (buffer != 255) {
        sprintf(string, "Input: %c", buffer);
        RIT128x96x4StringDraw(string, 12, 0, 15);

        if (led == 0 && buffer == B_HIGH) {
          led = 1;
          GPIOPinWrite(PORT_F, PIN_0, HIGH);
        }
        else if (led == 1 && buffer == B_LOW) {
          led = 0;
          GPIOPinWrite(PORT_F, PIN_0, LOW);
        }
      }

      keyboardHandler();
      if (btnA) {
    	if (lastOutputChar == B_LOW) {
    	  sprintf(string, "Output: %c", B_HIGH);
    	  UARTCharPut(UART_BASE, B_HIGH);
    	  lastOutputChar = 'a';
    	}
    	else {
    	  sprintf(string, "Output: %c", B_LOW);
    	  UARTCharPut(UART_BASE, B_LOW);
    	  lastOutputChar = B_LOW;
    	}
    	RIT128x96x4StringDraw(string, 12, 10, 15);
    	delay(1);
      }
    };

   return(0);
}
