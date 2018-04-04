/*
 * GOTO2.h Written by Igor Ovchinnikov 02/07/2016
 */
 
long Stepper_step(long ipSteps, unsigned uStepPin, unsigned uDirPin, unsigned uStepsPS)
{
 long iSteps=ipSteps, lRetVal=0;
 if((uStepPin>53)||(uDirPin>53)) return lRetVal;
 
 if(iSteps > 0) digitalWrite(uDirPin,  LOW);
 if(iSteps < 0) digitalWrite(uDirPin,  HIGH);
 iSteps=abs(iSteps);

 while (iSteps>0)
 {
  digitalWrite(uStepPin,  HIGH);
  delay(1000/uStepsPS);
  delayMicroseconds(1000*(1000%uStepsPS));
  digitalWrite(uStepPin,  LOW);
  iSteps--;
  if (ipSteps>0) lRetVal++; else lRetVal--;
 }
 return lRetVal;
}

void Stepper_X_step(int ipSteps)
{
  Stepper_step(ipSteps, DX_STEP_PIN, DX_DIR_PIN, imStepsXPS);
}

void Stepper_Y_step(int ipSteps)
{
  Stepper_step(ipSteps, DY_STEP_PIN, DY_DIR_PIN, imStepsYPS);
}

int iJoyControl=0;

// Функция int AskJoy() возвращает при ее вызове следующие значения:

//   0 - когда ничего не надо делать
//   1 - когда надо сделать микрошаг вперед по оси Х
//  10 - когда надо сделать полныйшаг вперед по оси Х
//  -1 - когда надо сделать микрошаг назад по оси Х
// -10 - когда надо сделать полныйшаг назад по оси Х
//   2 - когда надо сделать микрошаг вверх по оси У
//  20 - когда надо сделать полныйшаг вверх по оси У
//  -2 - когда надо сделать микрошаг вниз по оси У
// -20 - когда надо сделать полныйшаг вниз по оси У
//   3 - включить трекинг
//  -3 - отключить трекинг

int AskJOY()
{
  int iA1=0, iA2=0, iA3=0;
  int iRetValue=0;

  iA1 = analogRead(A6);
  iA2 = analogRead(A7);
  iA3 = analogRead(A3);
 
  if (0<=iA1&&iA1<=1024) ; else iA1=0;
  if (0<=iA2&&iA2<=1024) ; else iA2=0;
    
  if(iA1>=0   && iA1 < 25)  { iRetValue= 10;} 
  if(iA1>=25 && iA1 < 450)  { iRetValue= 1; } 
  if(iA1>550  && iA1<=999)  { iRetValue=-1; } 
  if(iA1>999  && iA1<=1024) { iRetValue=-10;} 
  
  if(iA2>=0   && iA2 <  25) { iRetValue= 20;} 
  if(iA2>=25  && iA2 < 450) { iRetValue= 2; } 
  if(iA2>550  && iA2 <=999) { iRetValue=-2; } 
  if(iA2>999  && iA2<=1024) { iRetValue=-20;}
  
  if(iA3<500) if (iJoyControl<=0) {iRetValue= 3; iJoyControl= 3; delay(200);} else {iRetValue=-3; iJoyControl=-3; delay(200);};
  
  return iRetValue;
}
