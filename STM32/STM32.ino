#include <SPI.h>
#include "RF24.h" 
#include <SoftwareSerial.h> 
#include <SD.h>

File myFile;
float phi;

//C:\Users\Admin\AppData\Local\Temp\arduino_build_421501
float ptong, pmattroi, pluoi;
float A_tong, A_mattroi, A_luoi;
SoftwareSerial mySerial(PB10, PB11); // RX, TX
float u1, u2, i1, i2;
float uhd, ihd1, ihd2;
long tgtt;    /////////////////////////////////???
const uint64_t pipe = 0xE8E8F0F0E1LL; // địa chỉ để phát
RF24 radio(PB0, PA4); 
float msg[6];
int k = 0; 
int mau = 50;
float mu1[50], mu2[50], mi1[50], mi2[50];
const int sensor = A0;
int value = 0;long tg;
long tgtr;    ///////////////////////////////???

void setup() 
{  
  mySerial.begin(9600);   //Terminal init
  mySerial.println("Hello, world?");    //Print random message to test terminal
  pinMode(A0,INPUT);    
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(PA12,INPUT);
  pinMode(A3,INPUT);
  pinMode(PB1,OUTPUT);
  digitalWrite(PB1,0);
  radio.begin();                     
  radio.setAutoAck(1);               
  radio.setRetries(1,1);             
  radio.setDataRate(RF24_1MBPS);    //Data rate RF
  radio.setPALevel(RF24_PA_MAX);      //Set PA max (Dung luong toi da)
  radio.setChannel(10);               //Set channel
  radio.openWritingPipe(pipe);        //Open channel (Mo kenh)
  tg=micros();    //Thoi gian tu luc bat dau chuong trinh
}

//C:\Users\Admin\AppData\Local\Temp\arduino_build_532519
void loop() 
{ 
  tgtr=micros()-tg;   //???
  if(k<mau)
  if(micros()-tg>=300033)   //toc do lay mau: 300ms
  {
    tg=micros();
    u1=analogRead(A0)*11.0/1023.0*3.3/1.0/12.0*220.0;   //Tinh u1 tuc thoi
    u2=analogRead(A1)*11.0/1023.0*3.3/1.0/12.0*220.0;   //Tinh u2 tuc thoi
    i1=((analogRead(A2)*13.3/1023*1000000)-2314174.00)/66000;   //Tinh i1 tuc thoi
    i2=((analogRead(A3)*13.3/1023*1000000)-2314174.00)/66000;   //Tinh i2 tuc thoi
    mu1[k] = u1;
    mu2[k] = u2;
    mi1[k] = abs(i1);
    mi2[k] = abs(i2); 
    k++;
    mySerial.print(mu1[k]);mySerial.print("   ");
    mySerial.print(u2);mySerial.print("   ");
    mySerial.print(i1);mySerial.print("   ");
    mySerial.print(i2);mySerial.print("   ");
    mySerial.print(tgtr);mySerial.print("   ");
    mySerial.println("   ");
  }
  if(k>=mau)
  { 
      k=0;
      float u1_max,u2_max,i1_max,i2_max;
      
      /*Tim u, i max trong cac mang */
      for(int i=0;i<mau;i++)    
      {
         if(u1_max<mu1[i])u1_max=mu1[i];
         if(u2_max<mu2[i])u2_max=mu2[i];
         if(i1_max<mi1[i])i1_max=mi1[i];
         if(i2_max<mi2[i])i2_max=mi2[i];
      }
  
      /*Measure u, i to calculate cos phi */
      u1=analogRead(A0)*11.0/1023.0*3.3/1.0/12.0*220.0;
      u2=analogRead(A1)*11.0/1023.0*3.3/1.0/12.0*220.0;
      i1=((analogRead(A2)*13.3/1023*1000000)-2314174.00)/66000;
      i2=((analogRead(A3)*13.3/1023*1000000)-2314174.00)/66000;
  
      /* Print umax, imax to terminal */
      mySerial.println(".................................");
      mySerial.print(u1_max);mySerial.print("   ");
      mySerial.print(u2_max);mySerial.print("   ");
      mySerial.print(i1_max);mySerial.print("   ");
      mySerial.print(i2_max);mySerial.print("   ");
      mySerial.println("   ");
      float k = cos(acos(u1/u1_max)-acos(i1/i1_max));
      if(k>0.8&&k<=1.0)phi=k;
  
      /*Calculate Urms, Irms */
      uhd=abs(u2_max/sqrt(2)-16);
      if(uhd<=16)uhd=uhd - 16;
      ihd1=abs(i1_max)/sqrt(2)-1.95;
      ihd2=abs(i2_max)/sqrt(2)-1.67;
  
      /*Calculate P */
      ptong=uhd*ihd1*phi;
      pluoi=uhd*ihd2*phi;
      pmattroi=ptong-pluoi;
      A_mattroi=A_mattroi+pmattroi*(millis()-tgtt)/3600000;
      A_tong=A_tong+ptong*(millis()-tgtt)/3600000;
      
      tgtt=millis();    //???
      mySerial.print("Uhd:");mySerial.println(uhd);
      mySerial.print("ihd1:");mySerial.println(ihd1);
      mySerial.print("ihd2:");mySerial.println(ihd2);
      mySerial.print("cos phi");mySerial.println(phi);
      mySerial.println("................................."); 
      msg[0]=uhd;
      msg[1]=ihd1;
      msg[2]=ihd2; 
      msg[3]=phi;
      msg[4]=A_mattroi;
      msg[5]=A_tong; 
  
      /*Reset u, i array */
      for(int i=0;i<50;i++)
      {
        mu1[i]=0;
        mu2[i]=0;
        mi1[i]=0;
        mi2[i]=0;
      }
  
      /*Print to RF */
      for(int i=0;i<100;i++)
      {
        radio.write(&msg, sizeof(msg));
        delay(1);
      }
      //tg=micros();
  }
}
