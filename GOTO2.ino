/*
 * GOTO2.ino Written by Igor Ovchinnikov 17/07/2016
 */

#define ENABLE_XYZ_PIN 8 //Enable X,Y,Z pin
#define DX_STEP_PIN  5   //Контакт ардуино идущий на STEP драйвера X
#define DX_DIR_PIN   2   //Контакт ардуино идущий на DIR  драйвера X
#define DX_FORCE_PIN 9   //Разгонный пин драйвера X
#define DY_STEP_PIN  6   //Контакт ардуино идущий на STEP драйвера Y
#define DY_DIR_PIN   3   //Контакт ардуино идущий на DIR  драйвера Y
#define DY_FORCE_PIN 10  //Разгонный пин драйвера Y
#define X_JOY_SENCE  A6  //Сенсор оси Х джойстика
#define Y_JOY_SENCE  A7  //Сенсор оси У джойстика
#define SW_JOY_SENCE A3  //Сенсор кнопки джойстика

int iStepsDX  =   48;    //Полных шагов на 1 оборот двигателя X
int iStepsXPS =  250;    //Полных шагов в секунду на двигателе X
int iXStepX   =   16;    //Кратность шага драйвера X
double dRDX   = 1780.69; //Передаточное число редуктора X

int iStepsDY  =   96;    //Полных шагов на 1 оборот двигателя Y
int iStepsYPS = 5000;    //Полных шагов в секунду на двигателе Y
int iYStepX   =    4;    //Кратность шага драйвера Y
double dRDY   = 3168.00; //Передаточное число редуктора Y

int imStepsXPS = iStepsXPS*iXStepX; //Микрошагов в секунду на двигателе X
int imStepsYPS = iStepsYPS*iYStepX; //Микрошагов в секунду на двигателе Y

unsigned long ulSPRA = iStepsDX*dRDX*iXStepX; //Микрошагов двигателя X на полный оборот оси прямого восхождения
unsigned long ulSPDE = iStepsDY*dRDY*iYStepX; //Микрошагов двигателя Y на полный оборот оси склонения

const unsigned long StarMSPS=86164091; //Милисекунд в Звездных сутках

double udRAStepsPMS=double(ulSPRA)/double(StarMSPS); //Микрошагов двигателя X на 1 мс

int iStDX = -1;      //Исходное направление шага двигателя Х
int iStDY =  1;      //Исходное направление шага двигателя Y
int iMovement = 0;   //Может пригодиться 
int iLastMovement=0; //Может пригодиться

unsigned long ulMilisec=0;   //Виртуальное время трекера
unsigned long ulPortTimer=0; //Таймер порта

boolean bDebug  = false;  //Режим отладки
boolean bRun    = false;   //Трекинг выключен изначально
boolean bLCD    = false;  //Скоро пригодится
boolean bForceX = false;  //Ускоренный режим Х
boolean bForceY = false;  //Ускоренный режим Y

#include <D:\YandexDisk\Home\Arduino\_MySketches\GOTO3\GOTO3.h>

int AskControl()
{
  return AskJOY();
}

int Force_X(boolean bForce)
{
 int iXSX=0; 
 if(!bForceX && bForce) //Включаем полношаговый режим
 {
   iXSX = 1; //Кратность шага драйвера X
   digitalWrite(DX_FORCE_PIN, LOW);
   imStepsXPS = iStepsXPS*iXSX; //Шагов в секунду на двигателе X
   ulSPRA = iStepsDX*dRDX*iXSX; //Шагов двигателя X на полный оборот оси прямого восхождения
   bForceX=true;
 }
 if(bForceX && !bForce) //Включаем микрошаговый режим
 {
   iXSX = iXStepX; //Кратность шага драйвера X
   digitalWrite(DX_FORCE_PIN, HIGH);
   imStepsXPS = 500; //Микрошагов в секунду на двигателе X
   ulSPRA = iStepsDX*dRDX*iXSX; //Микрошагов двигателя X на полный оборот оси прямого восхождения
   bForceX=false;
  }
}

int Force_Y(boolean bForce)
{
  int iYSX=0;
  if(!bForceY && bForce) //Включаем полношаговый режим
  {
    iYSX = 1; //Кратность шага драйвера Y
    digitalWrite(DY_FORCE_PIN, LOW);
    imStepsYPS = iStepsYPS*iYSX; //Шагов в секунду на двигателе Y
    ulSPDE = iStepsDY*dRDY*iYSX; //Шагов двигателя Y на полный оборот оси склонений
    bForceY=true;
   }
  if(bForceY && !bForce) //Включаем микрошаговый режим
  {
    iYSX = iYStepX; //Кратность шага драйвера Y
    digitalWrite(DY_FORCE_PIN, HIGH);
    imStepsYPS = 500; //Микрошагов в секунду на двигателе Y
    ulSPDE = iStepsDY*dRDY*iYSX; //Микрошагов двигателя Y на полный оборот оси склонений
    bForceY=false;
  }
}

void reaction () //Обработка команд ПУ
  {
   int iKey=0;

// Здесь мы договариваемся, что функция int AskControl(),
// к чему бы она ни была привязана, возвращает при ее вызове следующие значения:

//   0 - когда ничего не надо делать
//   1 - когда надо сделать микрошаг вперед по оси Х
//  16 - когда надо сделать полныйшаг вперед по оси Х
//   4 - когда надо сделать микрошаг назад по оси Х
//  64 - когда надо сделать полныйшаг назад по оси Х
//   2 - когда надо сделать микрошаг вверх по оси У
//  32 - когда надо сделать полныйшаг вверх по оси У
//   8 - когда надо сделать микрошаг вниз по оси У
// 128 - когда надо сделать полныйшаг вниз по оси У
// 256 - включить/отключить трекинг

  do 
   {
    iMovement=0;
    iKey=AskControl();
    if ((iKey &   4)==  4 && iStDX!=0) {Force_X(false); Stepper_X_step(-iStDX); iMovement=iMovement |   4;} // Микрошаг назад
    if ((iKey &  64)== 64 && iStDX!=0) {Force_X(true);  Stepper_X_step(-iStDX); iMovement=iMovement |  64;} // Полный шаг назад
    if ((iKey &   8)==  8 && iStDY!=0) {Force_Y(false); Stepper_Y_step( iStDY); iMovement=iMovement |   8;} // Микрошаг вниз
    if ((iKey & 128)==128 && iStDY!=0) {Force_Y(true);  Stepper_Y_step( iStDY); iMovement=iMovement | 128;} // Полный шаг вниз
    if ((iKey &   2)==  2 && iStDY!=0) {Force_Y(false); Stepper_Y_step(-iStDY); iMovement=iMovement |   2;} // Микрошаг вверх
    if ((iKey &  32)== 32 && iStDY!=0) {Force_Y(true);  Stepper_Y_step(-iStDY); iMovement=iMovement |  32;} // Полный шаг вверх    
    if ((iKey &   1)==  1 && iStDX!=0) {Force_X(false); Stepper_X_step( iStDX); iMovement=iMovement |   1;} // Микрошаг вперед
    if ((iKey &  16)== 16 && iStDX!=0) {Force_X(true);  Stepper_X_step( iStDX); iMovement=iMovement |  16;} // Полный шаг вперед
    if ((iKey & 256)==256) 
     {
     if(!bRun) {bRun=true;  bLCD=false; bForceX = true; Force_X(false); ulMilisec=millis();} //Включить  ведение  (Tracking ON)
     else      {bRun=false; bLCD=false;} //Отключить ведение (Tracking OFF)
     iMovement=iMovement | 256;
     }
   } while (iKey!=0);
  if (iMovement!=0) ulMilisec=millis(); 
 } 

void setup()
{
  pinMode(ENABLE_XYZ_PIN,  OUTPUT);  // ENABLE XYZ PIN
  digitalWrite(ENABLE_XYZ_PIN, LOW); // LOW
  pinMode(DX_STEP_PIN,  OUTPUT);     // DX STEP PIN
  digitalWrite(DX_STEP_PIN, LOW);    // LOW
  pinMode(DX_DIR_PIN,  OUTPUT);      // DX DIR PIN
  digitalWrite(DX_DIR_PIN, LOW);     // LOW
  pinMode(DX_FORCE_PIN,  OUTPUT);    // DX FORCE PIN
  digitalWrite(DX_FORCE_PIN, HIGH);  // HIGH
  pinMode(DY_STEP_PIN,  OUTPUT);     // DY STEP PIN
  digitalWrite(DY_STEP_PIN, LOW);    // LOW
  pinMode(DY_DIR_PIN,  OUTPUT);      // DY DIR PIN
  digitalWrite(DY_DIR_PIN, LOW);     // LOW
  pinMode(DY_FORCE_PIN,  OUTPUT);    // DY FORCE PIN
  digitalWrite(DY_FORCE_PIN, HIGH);  // HIGH
  pinMode(SW_JOY_SENCE, INPUT_PULLUP);         // Сенсор кнопки джойстика
  pinMode(X_JOY_SENCE, INPUT);          // Сенсор оси X джойстика
  pinMode(Y_JOY_SENCE, INPUT);          // Сенсор оси Y джойстика
  pinMode(13,  OUTPUT);  // LED на плате ардуино
  digitalWrite(13, LOW); // Выключен
  Serial.begin(9600);    // Подключаем COM порт
  Serial.flush();        // Сбрасываем содержимое COM порта
  ulMilisec=millis();     // Фиксируем время начала работы
 }

void loop()
{
 long LoopTime=0;
 long StepsNeed=0;
 reaction ();
 if(bRun)
 {
 Force_X(false); //Микрошаговый режим
 LoopTime=millis()-ulMilisec;
 StepsNeed=LoopTime*udRAStepsPMS;
 if(StepsNeed>=1)
  {
   digitalWrite(13, HIGH);    // Зажигаем LCD
   Stepper_X_step(StepsNeed*iStDX);  // Шагаем
   ulMilisec+=double(StepsNeed)/udRAStepsPMS; // Виртуальное время трекера
  }
 if (bDebug) {Serial.print(" StepsNeed "); Serial.print(StepsNeed); Serial.print(" udRAStepsPMS "); Serial.println(udRAStepsPMS);}
 digitalWrite(13, LOW); // Тушим LCD
 }
}
