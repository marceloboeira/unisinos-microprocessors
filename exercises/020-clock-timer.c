#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "rit128x96x4.h"

// HARDWARE DEFINITIONS
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

#define BTN_NONE 0
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_UP 3
#define BTN_DOWN 5

#define CURSOR_POSITION_NONE 0
#define CURSOR_POSITION_SECONDS 1
#define CURSOR_POSITION_MINUTES 2
#define CURSOR_POSITION_HOURS 3

char (*g_cursorTemplate[])[] = { &"        ",
        	                  &"       ^",
        	                  &"    ^   ",
        	                  &" ^      " };

int g_cursorPosition = CURSOR_POSITION_NONE;

short int g_button;
unsigned long g_ulFlags;
int g_seconds = 3600;
char buffer[20];

#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine) { }
#endif

void renderDisplay() {
  int seconds = 0, minutes = 0, hours = 0;
  seconds = g_seconds;

  if (seconds >= 60) {
    minutes = seconds / 60;
    seconds = seconds - minutes * 60;
  }

  if (minutes >= 60) {
    hours = minutes / 60;
    minutes = minutes - hours * 60;
  }

  // Button events
  updateCursorPosition(g_button);
  updateGlobalSeconds(g_button);
  g_button = BTN_NONE;

  // Render display components
  sprintf(buffer, "%02i:%02i:%02i", hours, minutes, seconds);
  RIT128x96x4StringDraw(buffer, 42, 32, 15);
  RIT128x96x4StringDraw(g_cursorTemplate[g_cursorPosition], 42, 41 , 15);

}

void updateCursorPosition(int btn) {
  if (btn == BTN_LEFT) g_cursorPosition++;
  else if (btn == BTN_RIGHT) g_cursorPosition--;

  if (g_cursorPosition < CURSOR_POSITION_NONE) g_cursorPosition = CURSOR_POSITION_NONE;
  else if (g_cursorPosition > CURSOR_POSITION_HOURS) g_cursorPosition = CURSOR_POSITION_NONE;
}

void updateGlobalSeconds(int btn) {
  int increaseFactor = 0;

  switch (g_cursorPosition) {
    case CURSOR_POSITION_SECONDS: increaseFactor =  1; break;
    case CURSOR_POSITION_MINUTES: increaseFactor = 60; break;
    case CURSOR_POSITION_HOURS: increaseFactor = 3600; break;
    case CURSOR_POSITION_NONE:
    default: increaseFactor = 0; break;
  }

  if (btn == BTN_UP) g_seconds += increaseFactor;
  else if (btn == BTN_DOWN) g_seconds -= increaseFactor;

  if ((g_seconds < 0) || (g_seconds >= 86400)) g_seconds = 0;
}

void clockTimerHandler(void) {
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  g_seconds++;
}

void keyboardHandler(void) {
  if ((GPIOPinRead(PORT_E, PIN_0) & 0x01) != 0x01)      g_button = BTN_UP;
  else if ((GPIOPinRead(PORT_E, PIN_1) & 0x02) != 0x02) g_button = BTN_DOWN;
  else if ((GPIOPinRead(PORT_E, PIN_2) & 0x04) != 0x04) g_button = BTN_LEFT;
  else if ((GPIOPinRead(PORT_E, PIN_3) & 0x08) != 0x08) g_button = BTN_RIGHT;
  else g_button = BTN_NONE;

  GPIOPinIntClear(GPIO_PORTE_BASE, PIN_0 | PIN_1 | PIN_2 | PIN_3);
}

void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

  // Timer
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER0_BASE, TIMER_A);

  // Keyboard
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOIntTypeSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, GPIO_FALLING_EDGE);
  GPIOPinIntEnable(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPinTypeGPIOInput(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  // LCD
  RIT128x96x4Init(1000000);

  // Interruptions
  IntMasterEnable();
  IntEnable(INT_TIMER0A);
  IntEnable(INT_GPIOE);
  IntPrioritySet(INT_GPIOE, 0x00);
}

void loop() {
  renderDisplay();
}

int main(void) {
  setup();

  while(true) {
    loop();
  }
  return 0;
}
