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

#define KEYBOARD_HANDLER 0
#define BLINK_HANDLER 1


typedef struct {
  unsigned long port;
  unsigned long pin;
  int interval;
  short int inverse;
} Led;

Led LEDS[3];

int btnLeft = 0,
    btnRight = 0,
    btnUp = 0,
    btnDown = 0,
    selectedLed = 2;
int STATE = 0;

void delay(int time) {
  int i = 0;
  for (i = 0; i < time * 1000000; i++);
}

void blink(Led led) {
  GPIOPinWrite(led.port, led.pin, led.inverse ? LOW : HIGH);
  delay(led.interval);
  GPIOPinWrite(led.port, led.pin, led.inverse ? HIGH : LOW);
}

void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeGPIOInput(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  LEDS[0].port     = PORT_F;
  LEDS[0].pin      = PIN_0;
  LEDS[0].interval = 1;
  LEDS[0].inverse  = false;

  LEDS[1].port     = PORT_F;
  LEDS[1].pin 	   = PIN_2;
  LEDS[1].interval = 1;
  LEDS[1].inverse  = true;

  LEDS[2].port     = PORT_F;
  LEDS[2].pin      = PIN_3;
  LEDS[2].interval = 1;
  LEDS[2].inverse  = true;
}

void setCurrentStateTo(int newState) {
  STATE = newState;
}

void keyboardHandler() {
  btnLeft = btnRight = btnUp = btnDown = 0;
  btnLeft  = (GPIOPinRead(PORT_E, PIN_0) & 0x01) != 0x01;
  btnRight = (GPIOPinRead(PORT_E, PIN_1) & 0x02) != 0x02;
  btnUp    = (GPIOPinRead(PORT_E, PIN_2) & 0x04) != 0x04;
  btnDown  = (GPIOPinRead(PORT_E, PIN_3) & 0x08) != 0x08;

  if (btnUp) {
    LEDS[selectedLed].interval += 2;
  }
  if (btnDown) {
   LEDS[selectedLed].interval -= 2;
  }
  if (btnRight) {
    selectedLed++;
  }
  if (btnLeft) {
    selectedLed--;
  }
  if (selectedLed > 2 || selectedLed < 0) {
	selectedLed = 0;
  }

  setCurrentStateTo(BLINK_HANDLER);
}
void blinkHandler() {
  int i = 0;
  for (i = 0; i <= 2; i++) {
	if (LEDS[i].interval < 1) {
	  LEDS[i].interval = 1;
	}
    blink(LEDS[i]);
  }
  setCurrentStateTo(KEYBOARD_HANDLER);
}

int main(){
  setup();

  while (1) {
    switch (STATE) {
      case KEYBOARD_HANDLER: keyboardHandler(); break;
      case BLINK_HANDLER: blinkHandler(); break;
      default: setCurrentStateTo(KEYBOARD_HANDLER); break;
    }
  }
}
