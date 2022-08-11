#include <SPI.h>
#include "RF24.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
const uint64_t pipe = 0xE8E8F0F0E1LL; // d?a ch? phát
RF24 radio(9,10);//thay 10 thành 53 v?i mega
float msg[8];char t[100];
long tg;
void setup(){
  Serial.begin(9600);
  radio.begin();    
  mySerial.begin(9600);                 
  radio.setAutoAck(1);              
  radio.setDataRate(RF24_1MBPS);    // T?c d? d? li?u
  radio.setChannel(10);               // Ð?t kênh
  radio.openReadingPipe(1,pipe);     
  radio.startListening();  
}
 
void loop(){
  
  if (radio.available()){
    while (radio.available()){
      radio.read(&msg, sizeof(msg));
      Serial.print(msg[0]); Serial.print("    "); 
      Serial.print(msg[1]); Serial.print("    "); 
      Serial.print(msg[2]); Serial.print("    "); 
      Serial.print(msg[3]); Serial.print("    "); 
      Serial.print(msg[4]); Serial.print("    ");
      Serial.print(msg[5]); Serial.println("    ");
      Serial.print(msg[6]); Serial.println("    ");
      Serial.print(msg[7]); Serial.println("    ");
      sprintf(t,"a%db%dc%dd%de%df%dg%dh%dk100",(int)(msg[0]*100),(int)(msg[1]*100),(int)(msg[2]*100),(int)(msg[3]*100),(int)(msg[4]*100),(int)(msg[5]*100),(int)(msg[6]*100),(int)(msg[7]*100));
      //sprintf(t,"a%fb%fc%fd%fe%ff%f",msg[0],msg[1],msg[2],msg[3],msg[4],msg[5]);
      
     
    }
  }
  if(millis()-tg>=2000)
  {
    tg=millis();
   mySerial.println(t);
   Serial.println(t);
   }
  
}