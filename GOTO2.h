/*
 * GOTO2.h Written by Igor Ovchinnikov 17/07/2016
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

// Функция int AskJoy() возвращает при ее вызове следующие значения:

//    0 - когда ничего не надо делать
//    1 - когда надо сделать микрошаг вперед по оси Х
//   16 - когда надо сделать полныйшаг вперед по оси Х
//    4 - когда надо сделать микрошаг назад по оси Х
//   64 - когда надо сделать полныйшаг назад по оси Х
//    2 - когда надо сделать микрошаг вверх по оси У
//   32 - когда надо сделать полныйшаг вверх по оси У
//    8 - когда надо сделать микрошаг вниз по оси У
//  128 - когда надо сделать полныйшаг вниз по оси У
//  256 - включить/отключить трекинг

int AskJOY()
{
  int iA1=0, iA2=0, iA3=0;
  int iRetValue=0;

  iA1 = analogRead(X_JOY_SENCE);
  iA2 = analogRead(Y_JOY_SENCE);
  iA3 = analogRead(SW_JOY_SENCE);
    
  if(iA1<25)                { iRetValue=iRetValue | 16; } // Полный шаг X+
  if(iA1>=25 && iA1 < 490)  { iRetValue=iRetValue |  1; } // Микрошаг X+
  if(iA1>520 && iA1<=1000)  { iRetValue=iRetValue |  4; } // Микрошаг X-
  if(iA1>1000)              { iRetValue=iRetValue | 64; } // Полный шаг X-
  
  if(iA2<25)                { iRetValue=iRetValue | 32; } // Полный шаг Y+
  if(iA2>=25  && iA2 < 490) { iRetValue=iRetValue |  2; } // Микрошаг Y+
  if(iA2>510  && iA2<=1000) { iRetValue=iRetValue |  8; } // Микрошаг Y-
  if(iA2>1000)              { iRetValue=iRetValue | 128;} // Полный шаг Y-

  if(iA3<500) {iRetValue=iRetValue | 256; delay(250);}    // Включить/отключить трекинг

  return iRetValue;
}

