#include <stdio.h>
#include <stdlib.h>

#define BOOT 0
#define INSERT_MONEY 1
#define SELECT_PRODUCT 2
#define PURCHASE 3
#define CANCEL_OPERATION 8
#define SHUTDOWN 9
int STATE = BOOT;

typedef struct {
  char *name;
  int amount;
  float price;
} product;

#define PRODUCTS_AMOUNT 2
product PRODUCTS[PRODUCTS_AMOUNT];

float VALUE = 0.0;
int SELECTED_PRODUCT = -1;

void returnChange() {
  printf("\n Returning $%f", VALUE);
  VALUE = 0.0;
}

void generateSeed(){
  PRODUCTS[0].name = "Stuff";
  PRODUCTS[0].amount = 1;
  PRODUCTS[0].price = 10.5;

  PRODUCTS[1].name = "Tooo";
  PRODUCTS[1].amount = 2;
  PRODUCTS[1].price = 20.3;
}

void printProduct(int i) {
  printf("\n%i\t%s\t%i\t$%f", i, PRODUCTS[i].name, PRODUCTS[i].amount, PRODUCTS[i].price);
}

void listProducts(int inventoryOnly) {
  int i = 0;
  printf("\nID\tProduct\tAmount\tPrice");
  for(i = 0; i < PRODUCTS_AMOUNT; i++) {
    int showProduct = (inventoryOnly ? PRODUCTS[i].amount > 0 : 1 );
    if (showProduct) {
      printProduct(i);
    }
  }
}

int hasProductOnInventory(int i) {
  return PRODUCTS[i].amount > 0;
}

int isAValidSelectedProduct() {
  return SELECTED_PRODUCT >= 0
    && SELECTED_PRODUCT <= (PRODUCTS_AMOUNT - 1)
    && hasProductOnInventory(SELECTED_PRODUCT)
    && VALUE >= PRODUCTS[SELECTED_PRODUCT].price;
}

void setCurrentStateTo(int newState) {
  STATE = newState;
}

void _bootHandler() {
  generateSeed();
  setCurrentStateTo(INSERT_MONEY);
}

void _insertMoneyHandler() {
  while (VALUE <= 0.0) {
    printf("\n Please Insert money: ");
    scanf("%f", &VALUE);
    fflush(stdin);
    printf("\n Value: %f", VALUE);
  }
  setCurrentStateTo(SELECT_PRODUCT);
}

void _selectProductHandler() {
  int forceCancel = 0;
  while (!isAValidSelectedProduct()) {
    listProducts(1);
    printf("\n Please select a valid product: ");
    fflush(stdin);
    scanf("%i", &SELECTED_PRODUCT);
  }
  setCurrentStateTo(PURCHASE);
}

void _purchaseHandler() {
  VALUE = VALUE - PRODUCTS[SELECTED_PRODUCT].price;
  PRODUCTS[SELECTED_PRODUCT].amount = PRODUCTS[SELECTED_PRODUCT].amount - 1;
  printf("\n %s purchased successfully!\n", PRODUCTS[SELECTED_PRODUCT].name);

  SELECTED_PRODUCT = -1;
  returnChange();
  
  setCurrentStateTo(INSERT_MONEY);
}

void _cancelOperaionHandler() {
  printf("\n Operation canceled!");
  returnChange();
  setCurrentStateTo(SHUTDOWN);
}

void _shutdownHandler() {
  printf("\n Shutting down!");
}

void stateMachineHandler() {
  while (STATE != SHUTDOWN) {
    switch (STATE) {
      case BOOT: _bootHandler(); break;
      case INSERT_MONEY: _insertMoneyHandler(); break;
      case SELECT_PRODUCT: _selectProductHandler(); break;
      case PURCHASE: _purchaseHandler(); break;
      case CANCEL_OPERATION: _cancelOperaionHandler(); break;
      default: setCurrentStateTo(BOOT); break;
    }
  }
  _shutdownHandler();
}

int main() {
  stateMachineHandler();
  return 0;
}