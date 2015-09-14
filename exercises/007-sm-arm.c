#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_sysctl.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/adc.h>
#include <driverlib/debug.h>
#include <rit128x96x4.h>

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

short int btnLeft  = 0,
		  btnRight = 0,
		  btnUp    = 0,
		  btnDown  = 0;

// STATES
#define BOOT_HANDLER 0
#define MONEY_WAITING_HANDLER 1
#define PRODUCT_SELECTION_HANDLER 2
#define PURCHASE_CONFIRMATION_HANDLER 3
#define OPERATION_CANCELING_HANDLER 8
int STATE = BOOT_HANDLER;

// PRODUCTS
typedef struct {
  char *name;
  int amount;
  int price;
} Product;

#define PRODUCTS_AMOUNT 2
Product PRODUCTS[PRODUCTS_AMOUNT];
int SELECTED_PRODUCT = 0;

int CURRENT_VALUE = 0;

void delay(int time) {
  int i = 0;
  for (i = 0; i < time * 500000; i++);
}

void _setupCore() {
   SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  GPIOPinTypeGPIOInput(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_E, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3);
  GPIOPadConfigSet(PORT_F, PIN_0 | PIN_1 | PIN_2 | PIN_3, DEFAULT_STRENGTH, DEFAULT_PULL_TYPE);
}

void _setupDisplay() {
  RIT128x96x4Init(1000000);
  RIT128x96x4StringDraw("       SM BETA       ", 0, 0,  15);
  RIT128x96x4StringDraw("---------------------", 0, 7,  15);
  RIT128x96x4StringDraw("*  | Product | $	  ", 0, 13, 15);
  RIT128x96x4StringDraw("---------------------", 0, 20, 15);
  RIT128x96x4StringDraw("10 | $$$     | $ 0.0 ", 0, 28, 15);
  RIT128x96x4StringDraw("05 | $$$     | $ 0.0 ", 0, 38, 15);
  RIT128x96x4StringDraw("---------------------", 0, 45, 15);
  RIT128x96x4StringDraw("               	  ", 0, 60, 15);
  RIT128x96x4StringDraw("---------------------", 0, 75, 15);
  RIT128x96x4StringDraw("$ 0     			  ", 0, 85, 15);

}

void _setupProducts(){
  PRODUCTS[0].name = "Water";
  PRODUCTS[0].amount = 10;
  PRODUCTS[0].price = 1;
  PRODUCTS[1].name = "Coke ";
  PRODUCTS[1].amount = 1;
  PRODUCTS[1].price = 2;
}

void setCurrentStateTo(int newState) {
  STATE = newState;
}

int hasProductOnInventory(int i) {
  return PRODUCTS[i].amount > 0;
}

int isAValidSelectedProduct() {
  return SELECTED_PRODUCT >= 0
    && SELECTED_PRODUCT <= (PRODUCTS_AMOUNT - 1)
    && hasProductOnInventory(SELECTED_PRODUCT)
    && CURRENT_VALUE >= PRODUCTS[SELECTED_PRODUCT].price;
}

void displayUpdateProductList() {
  int i = 0, yBase = 28, yJump = 10, listCounter = 0;
  for(i = 0; i < PRODUCTS_AMOUNT; i++) {
    unsigned char buffer[30];
    sprintf(buffer, "%02i | %s   | $ %i", PRODUCTS[i].amount, PRODUCTS[i].name, PRODUCTS[i].price);
    RIT128x96x4StringDraw(buffer, 0, (yBase + (yJump * listCounter++)), 15);
  }
}

void displayUpdateCurrentValue() {
  unsigned char buffer[10];
  sprintf(buffer, "$ %i     ", CURRENT_VALUE);
  RIT128x96x4StringDraw(buffer, 0, 85, 15);
}

void displayClearCurrentMessage() {
  RIT128x96x4StringDraw("                        ", 0, 60, 15);
}
void displayUpdateCurrentMessage(char* buffer) {
  displayClearCurrentMessage();
  RIT128x96x4StringDraw(buffer, 0, 60, 15);
}

void displayUpdateSelectedProduct() {
  RIT128x96x4StringDraw(PRODUCTS[SELECTED_PRODUCT].name, 0, 85, 15);
}

void productSelectionHandler() {
  int forceCancel = 0;
  setCurrentStateTo(PURCHASE_CONFIRMATION_HANDLER);
}

void keyboardHandler() {
  btnLeft = btnRight = btnUp = btnDown = 0;
  btnRight = (GPIOPinRead(PORT_E, PIN_0) & 0x01) != 0x01;
  btnLeft = (GPIOPinRead(PORT_E, PIN_1) & 0x02) != 0x02;
  btnUp = (GPIOPinRead(PORT_E, PIN_2) & 0x04) != 0x04;
  btnDown = (GPIOPinRead(PORT_E, PIN_3) & 0x08) != 0x08;
}

void moneyWaitingHandler() {
  displayUpdateCurrentMessage("Insert money");
  displayUpdateProductList();
  displayUpdateCurrentValue();

  while (!btnRight) {
    keyboardHandler();
	if (btnUp) {
	  CURRENT_VALUE += 1;
	}
	delay(1);
	displayUpdateCurrentValue();
  }
  keyboardHandler();
  setCurrentStateTo(PRODUCT_SELECTION_HANDLER);
}

void productionSelectionHandler() {
  displayUpdateCurrentMessage("Choose a product");
  displayUpdateSelectedProduct();

  while (!btnRight) {
    keyboardHandler();
	if (btnUp) {
	  SELECTED_PRODUCT += 1;
	}
	SELECTED_PRODUCT = SELECTED_PRODUCT > (PRODUCTS_AMOUNT - 1) ? 0 : SELECTED_PRODUCT;

	displayUpdateSelectedProduct();
	delay(1);
  }
  keyboardHandler();
  setCurrentStateTo(PURCHASE_CONFIRMATION_HANDLER);
}

void purchaseConfirmationHandler() {
  displayUpdateCurrentMessage("Wait");
  delay(2);
  displayUpdateCurrentMessage("Wait.");
  delay(2);
  displayUpdateCurrentMessage("Wait..");
  delay(2);
  displayUpdateCurrentMessage("Wait...");
  delay(2);

  int newValue = CURRENT_VALUE - PRODUCTS[SELECTED_PRODUCT].price;

  if (newValue < 0) {
	displayUpdateCurrentMessage("Not enough money");
	setCurrentStateTo(PRODUCT_SELECTION_HANDLER);
  }
  else if (!hasProductOnInventory(SELECTED_PRODUCT)){
	displayUpdateCurrentMessage("Out of stock");
	setCurrentStateTo(PRODUCT_SELECTION_HANDLER);
  }
  else {
	displayUpdateCurrentMessage("Thanks!");
	delay(5);
	PRODUCTS[SELECTED_PRODUCT].amount = PRODUCTS[SELECTED_PRODUCT].amount - 1;

	CURRENT_VALUE = newValue;
	unsigned char buffer[10];
	sprintf(buffer, "Change: $ %i", CURRENT_VALUE);
	displayUpdateCurrentMessage(buffer);
	CURRENT_VALUE = 0;

	setCurrentStateTo(MONEY_WAITING_HANDLER);
  }

  delay(10);
}

void bootHandler() {
  _setupCore();
  _setupDisplay();
  _setupProducts();
  setCurrentStateTo(MONEY_WAITING_HANDLER);
}


int main(){
  while (1) {
    switch (STATE) {
       case BOOT_HANDLER: bootHandler(); break;
       case MONEY_WAITING_HANDLER: moneyWaitingHandler(); break;
       case PRODUCT_SELECTION_HANDLER: productionSelectionHandler(); break;
       case PURCHASE_CONFIRMATION_HANDLER: purchaseConfirmationHandler(); break;
       default: setCurrentStateTo(BOOT_HANDLER); break;
    }
  }
}
