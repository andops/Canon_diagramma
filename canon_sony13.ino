// Управление объективом Canon и затвором Sony NEX 5
// Программа потенциально опасна!!! Некоторые объективы могут сгореть!!! Некоторые комбинации команд поданных по USB могут оставить неподвижные двигатели под напряжением и вывести их из строя!!! 
// Использование программы может стоить очень дорого. Проверяйте каждую строчку кода и используйте на свой страх и риск.
#include <SPI.h>

int buttonPinZ = 9; // Открыть/ Закрыть диафрагму и при длительном нажатии Затвор
int buttonStateZ = 1;
int flagZ = 1;
int buttonPinO = 8;
int buttonStateO = 1;
int flagO = 1;
int buttonPinC = 7;
int buttonStateC = 1;
int flagC = -1;
int sd = 4;
int nd = 1;
int p = 10;
//const int ledPin =  13;
unsigned long m0 = 0;
unsigned long m1;
unsigned long m;

int pulscount=1;  // Счетчик числа импульсов в пакете ИК
int puls = 4;    // Число пакетов ИК
const int chipSelectPin = 4;
int myInts[100];
int i=0;
int j=0;
int inByte;
int f =0;
void setup() { 
  delay(300);   
  CLKPR = B10000000; //Делим тактовую частоту 
  CLKPR = B00000001; // на 2
  
 pinMode(buttonPinZ, INPUT); // Включаем ножку на прием сигналов
 digitalWrite(buttonPinZ, HIGH); //включаем подтягивающие резисторы
 
 pinMode(buttonPinO, INPUT); // Включаем ножку на прием сигналов
 digitalWrite(buttonPinO, HIGH); //включаем подтягивающие резисторы
 
 pinMode(buttonPinC, INPUT); // Включаем ножку на прием сигналов
 digitalWrite(buttonPinC, HIGH); //включаем подтягивающие резисторы
 //pinMode(ledPin, OUTPUT);
 
  Serial.begin(38400); //с учетом пониженной частоты скорость будет 19600

 //Блок инициализации объектива 
   pinMode(chipSelectPin, OUTPUT);
   SPI.begin();
   SPI.setBitOrder(MSBFIRST);
   SPI.setClockDivider(SPI_CLOCK_DIV128); 
   SPI.setDataMode(SPI_MODE3);  
   digitalWrite(12, HIGH); //включаем подтягивающие резисторы. Некоторые объективы без этого не передат ответ. 
      for (j = 0; j <51; j = j + 1 ){ 
      myInts[j] = SPI.transfer(0x00);
      }
      delay(p); 
      myInts[0] = SPI.transfer(0x0A);
     delay(p); 
      myInts[1] = SPI.transfer(0x00);
     delay(p); 
      myInts[2] = SPI.transfer(0x0A);
     delay(p); 
      myInts[3] = SPI.transfer(0x00);
     delay(p); 
      myInts[4] = SPI.transfer(0x80);
     delay(p); 
      myInts[5] = SPI.transfer(0x0A);
     delay(p);  
      myInts[6] = SPI.transfer(0x97);
     delay(p); 
      myInts[7] = SPI.transfer(0x01);
            
     for (j = 8; j <19; j = j + 1 )
        { 
        myInts[j] = SPI.transfer(0x00); 
        delay(p); 
        }

     for (j = 0; j <19; j = j + 1 )
        { 
        Serial.print(myInts[j], DEC);   
        Serial.print("/");
        }
     Serial.println("/");

      delay(30);
      myInts[0] = SPI.transfer(19);// Готовимся к изменению диафрагмы, 
      //для большинства объективов аналогична команде 18, 
      //которая с отдельными объективами работает с противоположным знаком.
      delay(30);
      
      myInts[1] = SPI.transfer(128);// Полностью открываем диафрагму
   //   delay(300);
      for (j = 0; j <9; j = j + 1 ){ 
      myInts[0] = SPI.transfer(0x00);
      delay(p);
      }

////

 TCCR1A = _BV(COM1A1) | _BV(COM1B1) ; // phase and frequency correct mode. NON-inverted mode
 TCCR1B = _BV(WGM13) | _BV(CS10);     // Делитель N = 1 - (_BV(CS10)); 8 - (_BV(CS11))
 OCR1B = 0;              // Скважность 102 - рабочий сигнал, 0 - сигнал отсутствует
 ICR1 = 105;             // Частота 8мГц/2*N*ICR1 = 8000кГц/(2*1*105)=38кГц
 pinMode(10, OUTPUT);    // К 10 ножке подключен ИК светодиод АЛ107 через сопротивление 1 кОм
 }


void loop()  
{
  
  m1 = millis(); //Принудительное выключение питания двигателей командой 8 через пол секунды после нажатия кнопки или передачи команды по USB
  m = m1 -m0;
  if (m > 250 && flagO == 1)
    {
   myInts[0] = SPI.transfer(8);
   flagO= -1;
    }       
  
  if (Serial.available()) // Блок трансляции команд с комппьютера
    {
     
    m0 = millis();
    flagC=flagC+1;
    flagO = 1;//Флаг сообщает о необходимости снять напряжение с двигателя
      // read the oldest byte in the serial buffer:
    int incomingByte = Serial.read();
    int asc = incomingByte - 48; //после этого преобразования посылаемое число совпадает 
    //с цифрами на клавиатуре. B = 18, C = 19, : = 10, + = -5, t = 68

    if (asc == 45) {asc = 255;} //]
    if (asc == 43) {asc = 128;} //[
    if (asc == 42) {            //Z
      IR_Sony();
      flagC=1;
                    }
    if (asc == 51) {            //c - запрос информации от объектива

  myInts[0] = SPI.transfer(0x08);
  delay(p);
  for (j = 0; j <9; j = j + 1 ){ 
      myInts[j] = SPI.transfer(0x00);
      delay(p); 
      }

            myInts[0] = SPI.transfer(0x97);
            delay(p); 
             myInts[1] = SPI.transfer(0x01);
             delay(p); 
      for (j = 2; j <19; j = j + 1 ){ 
      myInts[j] = SPI.transfer(0x00);
     delay(p);  
      }
 
      //delay(200);
       for (j = 0; j <19; j = j + 1 ){ 
      Serial.print(myInts[j], DEC);   
    Serial.print("/");
      }
       Serial.println("/");
      myInts[0] = SPI.transfer(0x08);
      delay(p);
       for (j = 0; j <9; j = j + 1 ){ 
      myInts[j] = SPI.transfer(0x00);
      delay(p); 
      } 
    myInts[0] = SPI.transfer(128);
   delay(p); 
    myInts[1] = SPI.transfer(0);
   delay(p); 

    myInts[2] = SPI.transfer(0);
   delay(p); 
    myInts[3] = SPI.transfer(0);
    delay(p); 
    myInts[4] = SPI.transfer(0);
    delay(p); 
    myInts[5] = SPI.transfer(0);
    delay(p); 
    myInts[6] = SPI.transfer(0);
    delay(p); 
    myInts[7] = SPI.transfer(0);
    delay(p); 
    myInts[8] = SPI.transfer(0);
    delay(p); 
    for (j = 0; j <9; j = j + 1 ){ 
      Serial.print(myInts[j], DEC);   
    Serial.print("/");
      }
 Serial.println("/");
  myInts[0] = SPI.transfer(0x08);
    } 
if (asc == 50) {            //B - запрос информации от объектива

  myInts[0] = SPI.transfer(0x08);
  delay(p);
  for (j = 0; j <9; j = j + 1 ){ 
      myInts[j] = SPI.transfer(0x00);
      delay(p); 
      }

    myInts[0] = SPI.transfer(128);
    delay(p); 

    myInts[2] = SPI.transfer(0);
    Serial.print("A");
    Serial.print(myInts[2],DEC);
    myInts[2] = SPI.transfer(0);
    Serial.print("B");
    Serial.print(myInts[2],DEC);
    myInts[2] = SPI.transfer(0);
    Serial.print("C");
    Serial.print(myInts[2],DEC); 
    myInts[2] = SPI.transfer(0);
    Serial.print("D");
    Serial.print(myInts[2],DEC); 
    myInts[2] = SPI.transfer(0);
    Serial.print("E");
    Serial.print(myInts[2],DEC);
    myInts[2] = SPI.transfer(0);
    Serial.print("F");
    Serial.print(myInts[2],DEC);
    Serial.print("X");


    }   else {      
    
    Serial.println(asc, DEC);
    if (flagC > 1){
    inByte = SPI.transfer(asc); 
    Serial.print(inByte, DEC);   
    Serial.println("/");
   //  Serial.println(flagC, DEC);
    }
    }    
    }
  buttonStateZ = digitalRead(buttonPinZ);
  buttonStateO = digitalRead(buttonPinO);
  buttonStateC = digitalRead(buttonPinC);
  
  if (buttonStateZ == LOW) 
    {     
      m0 = millis();
      flagO = 1;
      while (buttonStateZ == LOW)
        {
         buttonStateZ = digitalRead(buttonPinZ);
        }
      m1 = millis();
      m = m1-m0;
      if (m>200)  // при отпускании кнопки нажатой более 0,5 с передается ИК сигнал на спуск камеры
        {
          IR_Sony();
        }
      else 
        {
         //flagZ = flagZ * -1;
         if (flagZ == 1)
           {

           flagZ = -1;

            myInts[0] = SPI.transfer(7);//нужна тоько для 50/1,4
            delay(p);
            myInts[0] = SPI.transfer(19);
            delay(p);    
            myInts[1] = SPI.transfer(128);

              for (j = 0; j <9; j = j + 1 ){ 
      myInts[0] = SPI.transfer(0x00);
      delay(10);
      }

           }
         else 
           {
if (f>0){
           myInts[0] = SPI.transfer(7);//нужно использовать команду 7 если раньше была использована 8. 
           //8 нужна для снятия напряжения с двигателя при простое.  У 135 более 1 А, у остальных 140 - 250 мА. 
           //На 28-80 не действует, возможно, надо снимать напряжение с объектива.
          delay(p);
           myInts[0] = SPI.transfer(19);
          delay(p);
           myInts[0] = SPI.transfer(sd*f);
          
        }    
           flagZ = 1;
           }
         }
    }
    
    if (buttonStateO == LOW) 
    {     
      m0 = millis();
      flagO = 1;
      while (buttonStateO == LOW)
        {
         buttonStateO = digitalRead(buttonPinO);
        }
      m1 = millis();
      m = m1-m0;
      if (m>200)  // при отпускании кнопки нажатой более 0,5 увеличиваем шаг изменения диафрагмы до 1
        {
        nd=2;
        }
      else 
        {
         f=f+nd;
         if (flagZ == 1)
           {
           myInts[0] = SPI.transfer(7);
          delay(p);
           myInts[0] = SPI.transfer(19);
          delay(p);
           myInts[0] = SPI.transfer(nd*sd);       
           
           }
         else 
           {
           
           myInts[0] = SPI.transfer(7);
          delay(p);
           myInts[0] = SPI.transfer(19);
          delay(p);
           myInts[0] = SPI.transfer(sd*f);
          flagZ=1;
       
           }
         }
    }
    
    if (buttonStateC == LOW) 
    {     
      m0 = millis();
      flagO = 1;
      while (buttonStateC == LOW)
        {
         buttonStateC = digitalRead(buttonPinC);
        }
      m1 = millis();
      m = m1-m0;
      if (m>200)  // при отпускании кнопки нажатой более 0,5 уменьшаем шаг изменения диафрагмы до 0,5
        {
          nd=1;
        }
      else 
        {
          
          if (f>0){f=f-nd;
         if (flagZ == 1)
           {
             myInts[0] = SPI.transfer(7);
           delay(p);
           myInts[0] = SPI.transfer(19);
           delay(p);
           myInts[0] = SPI.transfer(-nd*sd);
        
           }
         else 
           {
           if (f>0){
           myInts[0] = SPI.transfer(7);
          delay(p);
           myInts[0] = SPI.transfer(19);
          delay(p);
           myInts[0] = SPI.transfer(sd*f);
           flagZ=1;
          
                   }
           }
           }
         }
    }
}    

void IR_Sony()  { //Подпрограмма подачи ИК сигналя для спуска зотвора камеры NEX5 
 
  pulscount=1;  
   while (pulscount<puls)          // цикл пакета импульсов  
    
    {OCR1B = 102;   // включаем светодиод
    delayMicroseconds(1216);       // светодиод горит вдвое больше микросекунд из-за пониженной частоты 
    OCR1B = 0;     // выключаем светодиод
    delayMicroseconds(288);       // светодиод выключен  микросекунд
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288); 
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
     OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(320);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(288);
    OCR1B = 102;   // включаем светодиод
    delayMicroseconds(608);       
    OCR1B = 0;     
    delayMicroseconds(5568);
    pulscount++;
    }
  
}
