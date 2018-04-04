/*
 * GOTO2.ino Written by Igor Ovchinnikov 02/07/2016
 */

#define ENABLE_XYZ_PIN 8 //Enable X,Y,Z pin
#define DX_STEP_PIN  5   //Контакт ардуино идущий на STEP драйвера X
#define DX_DIR_PIN   2   //Контакт ардуино идущий на DIR  драйвера X
#define DX_FORCE_PIN 9   //Разгонный пин драйвера X
#define DY_STEP_PIN  6   //Контакт ардуино идущий на STEP драйвера Y
#define DY_DIR_PIN   3   //Контакт ардуино идущий на DIR  драйвера Y
#define DY_FORCE_PIN 10  //Разгонный пин драйвера Y

int iStepsDX  =   48;    //Полных шагов на 1 оборот двигателя X
int iStepsXPS =  200;    //Полных шагов в секунду на двигателе X
int iXStepX   =   16;    //Кратность шага драйвера X
double dRDX   = 1780.69; //Передаточное число редуктора X

int iStepsDY  =   96;    //Полных шагов на 1 оборот двигателя Y
int iStepsYPS =  250;    //Полных шагов в секунду на двигателе Y
int iYStepX   =    4;    //Кратность шага драйвера Y
double dRDY   = 3168.00; //Передаточное число редуктора Y

int imStepsXPS = iStepsXPS*iXStepX; //Микрошагов в секунду на двигателе X
int imStepsYPS = iStepsYPS*iYStepX; //Микрошагов в секунду на двигателе Y

unsigned long ulSPRA = iStepsDX*dRDX*iXStepX; //Микрошагов двигателя X на полный оборот оси прямого восхождения

const unsigned long StarMSPS=86164091; //Милисекунд в Звездных сутках

double udRAStepsPMS=double(ulSPRA)/double(StarMSPS); //Микрошагов двигателя X на 1 мс

int iStDX = -1;      //Исходное направление шага двигателя Х
int iStDY =  1;      //Исходное направление шага двигателя Y
int iMovement = 0;   //Может пригодиться 
int iLastMovement=0; //Может пригодиться

unsigned long iMilisec=0; //Время выполнения основного цикла программы

boolean bDebug  = false; //Режим отладки
boolean bRun    = false; //Трекинг отключен изначально
boolean bLCD    = false; //Скоро пригодится
boolean bForceX = false; //Ускоренный режим Х
boolean bForceY = false; //Ускоренный режим Y

//Здесь надо указать не D:\YandexDisk\Home\Arduino\_MySketches\GOTO1\ как у меня,
//а реальный путь, где GOTO1.h хранится:

#include <D:\YandexDisk\Home\Arduino\_MySketches\GOTO2\GOTO2.h>

int AskControl()
{
  int iRetVal=0;
  iRetVal=AskJOY();
  return iRetVal;
}

void reaction () //Обработка команд ПУ
  {
   int iKey=0;
       
// Здесь мы договариваемся, что функция int AskControl(),
// к чему бы она ни была привязана, возвращает при ее вызове следующие значения:

//   0 - когда ничего не надо делать
//   1 - когда надо сделать микрошаг вперед двигателем Х
//  10 - когда надо сделать полныйшаг вперед двигателем Х
//  -1 - когда надо сделать микрошаг назад двигателем Х
// -10 - когда надо сделать полныйшаг назад двигателем Х
//   2 - когда надо сделать микрошаг вверх двигателем У
//  20 - когда надо сделать полныйшаг вверх двигателем У
//  -2 - когда надо сделать микрошаг вниз двигателем У
// -20 - когда надо сделать полныйшаг вниз двигателем У
//   3 - когда надо включить трекинг
//  -3 - отключить  трекинг

  do 
   {
    iKey=AskControl();
       
    if (iKey==-1) // Назад на микрошаг
     {
      if(bForceX) {digitalWrite(DX_FORCE_PIN, HIGH); imStepsXPS = iStepsXPS; bForceX=false;} //*iXStepX;} //Микрошаговый режим
      if(iStDX!=0) Stepper_X_step(-iStDX);
     };

    if (iKey==-10) // Назад на полный шаг
     { 
      if(!bForceX) {digitalWrite(DX_FORCE_PIN, LOW); imStepsXPS = iStepsXPS; bForceX=true;} //Полношаговый режим
      if(iStDX!=0) Stepper_X_step(-iStDX);
      bLCD=false; 
     };
    
    if (iKey==-2) // Вниз на микрошаг
     {
      if(bForceY) {digitalWrite(DY_FORCE_PIN, HIGH); imStepsYPS = iStepsYPS; bForceY=false;} //*iYStepX;} //Микрошаговый режим
      if(iStDY!=0) Stepper_Y_step( iStDY*4);
      bLCD=false;
     };

    if (iKey==-20) // Вниз на полный шаг
     {
      if(!bForceY) {digitalWrite(DY_FORCE_PIN, LOW); imStepsYPS = iStepsYPS; bForceY=true;} //Полношаговый режим
      if(iStDY!=0) Stepper_Y_step( iStDY);
      bLCD=false;
     };
    
    if (iKey==2) // Вверх на микрошаг
     {
      if(bForceY) {digitalWrite(DY_FORCE_PIN, HIGH); imStepsYPS = iStepsYPS; bForceY=false;} //*iYStepX;} //Микрошаговый режим
      if(iStDY!=0) Stepper_Y_step(-iStDY*4);
      bLCD=false;
     };

   if (iKey==20) // Вверх на полный шаг
     {
      if(!bForceY) {digitalWrite(DY_FORCE_PIN, LOW); imStepsYPS = iStepsYPS; bForceY=true;} //Полношаговый режим
      if(iStDY!=0) Stepper_Y_step(-iStDY);
      bLCD=false;
     };     
    
    if (iKey==1) // Вперед на микрошаг
     {
      if(bForceX) {digitalWrite(DX_FORCE_PIN, HIGH); imStepsXPS = iStepsXPS; bForceX=false;} //*iXStepX;} //Микрошаговый режим
      if(iStDX!=0) Stepper_X_step( iStDX);
      bLCD=false;
     };

    if (iKey==10) // Вперед на полный шаг
     {
      if(!bForceX) {digitalWrite(DX_FORCE_PIN, LOW); imStepsXPS = iStepsXPS; bForceX=true;} //Полношаговый режим
      if(iStDX!=0) Stepper_X_step( iStDX);
      bLCD=false;
     };
   
   if (iKey== 3 && !bRun) //Включить ведение  (Tracking ON)
     {
      bRun=true;
      bLCD=false;
      iMilisec=millis();
     }
     
   if (iKey==-3 &&  bRun) {bRun=false; bLCD=false;} //Отключить ведение (Tracking OFF)
    
  } while (iKey!=0 || iMovement!=0);
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
  pinMode(A3, INPUT_PULLUP);         // Сенсор кнопки джойстика
  pinMode(A6, INPUT);          // Сенсор оси Y джойстика
  pinMode(A7, INPUT);          // Сенсор оси X джойстика
  pinMode(13,  OUTPUT);  // LED на плате ардуино
  digitalWrite(13, LOW); // Выключен
  Serial.begin(9600);    // Подключаем COM порт
  Serial.flush();        // Сбрасываем содержимое COM порта
  iMilisec=millis();     // Фиксируем время начала работы
 }

void loop()
{
 long LoopTime=0;
 long StepsNeed=0;
 reaction ();
 if(bRun)
 {
 if(bForceX) {digitalWrite(DX_FORCE_PIN, HIGH); imStepsXPS = iStepsXPS*iXStepX; bForceX=false;} //Микрошаговый режим
 LoopTime=millis()-iMilisec;
 StepsNeed=LoopTime*udRAStepsPMS;
 if(StepsNeed>=1)
  {
   digitalWrite(13, HIGH);    // Зажигаем LCD
   Stepper_X_step(StepsNeed*iStDX);  // Шагаем
   iMilisec+=double(StepsNeed)/udRAStepsPMS; // Временной эквивалент поворота трекера
  }
 if (bDebug) {Serial.print(" StepsNeed "); Serial.print(StepsNeed); Serial.print(" udRAStepsPMS "); Serial.println(udRAStepsPMS);}
 digitalWrite(13, LOW); // Тушим LCD
 }
}
