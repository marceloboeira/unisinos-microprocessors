#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <stdio.h>
#include <string.h>
#include <inc/hw_memmap.h>
#include <inc/hw_sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/debug.h>

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


void hackDelay() {
  int i = 0;
  for (i = 0; i < 1000000; i++);
}

void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeGPIOInput(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);
}

int main(){
  setup();
  while (1) {
  	GPIOPinWrite(PORT_F, PIN_0, HIGH);
  	hackDelay();
  	GPIOPinWrite(PORT_F, PIN_0, LOW);
  	GPIOPinWrite(PORT_F, PIN_2, LOW);
  	hackDelay();
  	GPIOPinWrite(PORT_F, PIN_2, HIGH);
  	GPIOPinWrite(PORT_F, PIN_3, LOW);
  	hackDelay();
  	GPIOPinWrite(PORT_F, PIN_3, HIGH);
  }
}
