#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <stdio.h>
#include <string.h>
#include <inc/hw_memmap.h>
#include <inc/hw_sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/debug.h>

int btnAPressed = 0, btnBPressed = 0;

int main(){

	// Clock setting
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	// Enable GPIO Port E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	// GPIO_STRENGTH_2MA - 2mA current
	// GPIO_PIN_TYPE_STD_WPU - WPU is the pull-up declaration
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	while (1) {
	  btnAPressed = (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & 0x01) != 0x01;
	  btnBPressed = (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & 0x02) != 0x02;

	  if (btnAPressed)
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	  else
		  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0xFF);

	  if (btnBPressed)
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	  else
	    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0xFF);
	}
}
