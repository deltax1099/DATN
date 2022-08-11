#include <SPI.h>
#include <SD.h>
#include "RF24.h" 
#include <SoftwareSerial.h> 
#include <SD.h>
#include <LiquidCrystal.h> 
#include <EEPROM.h>

//C:\Users\ZbookG3\AppData\Local\Temp\arduino_build_658316
//Peripherals
LiquidCrystal lcd(PA8, PA10, PB4, PB5, PB6, PB7);   //LCD
File myFile;    //SD card
const uint64_t pipe = 0xE8E8F0F0E1LL; //RF Address
RF24 radio(PB0,PA4); 
SoftwareSerial mySerial(PB10,PB11); // Serial
const int sensor = A0;

//Gia tri do
int mode=1;  //mode mua/ban
float phi;  
float uhd,ihd1,ihd2;
float A_tong,A_mattroi,A_luoi;
float a_mua;
float a_ban;

//Tuc thoi
float u1,u2,i1,i2;
float ptong,pmattroi,pluoi;
float p_mua;
float p_ban;

//Timer
long tgtt;  //Thoi gian tinh tien - Luu moc tinh tien
long tgtr;  //Thoi gian truoc - Chu ky lay mau
long tg;  //Thoi gian lay mau - Luu moc lay mau

//Data
float msg[8];
int k=0; 
int dem=0;
int mau=250;
double mu1[250],mu2[250],mi1[250],mi2[250];
int value = 0;


void setup() 
{  
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
  
  pinMode(A0,INPUT);  //u1
  pinMode(A1,INPUT);  //u2
  pinMode(A2,INPUT);  //i1
  pinMode(A3,INPUT);  //i2
  
  pinMode(PA12,INPUT); 
  pinMode(PB1,OUTPUT);  //relay
  digitalWrite(PB1,0);

  radio.begin();      //RF init          
  radio.setAutoAck(1);               
  radio.setRetries(1,1);             
  radio.setDataRate(RF24_1MBPS);    //Toc do truyen
  radio.setPALevel(RF24_PA_MAX);      //Dung luong toi da
  radio.setChannel(10);               //Kenh truyen
  radio.openWritingPipe(pipe);        //Mo kenh
  
  tg=micros();
  
  lcd.begin(16, 2);   //LCD init   
  lcd.print("Preparing Data...");
  
  if (!SD.begin(PC15))
  {
    mySerial.println("initialization failed!");     
  }
  a_mua    =EEPROM.read(1);
  a_ban    =EEPROM.read(2);
  A_mattroi=EEPROM.read(3);
  A_tong   =EEPROM.read(3);
}

void loop() 
{ 
  tgtr=micros()-tg; 
  if(k<mau)
  {
    if(micros()-tg>=60033)
    {
      tg=micros();
      u1=analogRead(A0)*11.0/1023.0*3.3/1.0/12.0*220.0;   //(R4+R5)*ADC*Vref/R4/kba
      u2=analogRead(A1)*11.0/1023.0*3.3/1.0/12.0*220.0;         
      i1=((analogRead(A2)*13.3/1023*1000000)-2314174.00)/66000;   //((R12+R13)/ADC*(uV)-Voffset)/do nhay(offset mac dinh 2.5 do nguon =  5)
      i2=((analogRead(A3)*13.3/1023*1000000)-2314174.00)/66000;  
      mu1[k] = u1;
      mu2[k] = u2;
      mi1[k] = abs(i1);
      mi2[k] = abs(i2); 
      k++;
      if(i1>=0&&i2>=0)
        {dem++;if(dem>150){mode=1;dem=0;}}
      else if(i1<=0&&i2<=0)
        {dem++;if(dem>150){mode=1;dem=0;}}
      else 
        {dem++;if(dem>150){mode=0;dem=0;}}
        
      mySerial.print(mu1[k]);mySerial.print("   ");
      mySerial.print(u2);mySerial.print("   ");
      mySerial.print(i1);mySerial.print("   ");
      mySerial.print(i2);mySerial.print("   ");
      mySerial.print(tgtr);mySerial.print("   ");
      mySerial.print(k);mySerial.print("   ");
      mySerial.println("   ");
    }
  }
  if(k>=mau)
  { 
    k=0;
    float u1_max, u2_max, i1_max, i2_max;
    for(int i=0;i<mau;i++)
    {
      if(u1_max<mu1[i])u1_max=mu1[i];
      if(u2_max<mu2[i])u2_max=mu2[i];
      if(i1_max<mi1[i])i1_max=mi1[i];
      if(i2_max<mi2[i])i2_max=mi2[i];
    }
    u1=analogRead(A0)*11.0/1023.0*3.3/1.0/12.0*220.0;
    u2=analogRead(A1)*11.0/1023.0*3.3/1.0/12.0*220.0;
    i1=((analogRead(A2)*13.3/1023*1000000)-2314174.00)/66000;
    i2=((analogRead(A3)*13.3/1023*1000000)-2314174.00)/66000;
    
    //    mySerial.println(".................................");
    //    mySerial.print(u1_max);mySerial.print("   ");
    //    mySerial.print(u2_max);mySerial.print("   ");
    //    mySerial.print(i1_max);mySerial.print("   ");
    //    mySerial.print(i2_max);mySerial.print("   ");
    //    mySerial.println("   ");
    
    float k = cos(acos(u1/u1_max)-acos(i1/i1_max));
    
    //uhd  
    uhd = abs(u2_max/sqrt(2))*203/220*206/209 - 2.5;
    if(2 <= uhd && uhd <= 3) 
      uhd = abs(uhd - 2);
    else if(1 <= uhd && uhd <= 2) 
      uhd = abs(uhd - 1);
    else if(uhd <= 0) 
      uhd = 0;
    else{}
    
    //ihd
    ihd1=abs(i1_max)/sqrt(2)-1.95;
    ihd2=abs(i2_max)/sqrt(2)-1.67; 
    
    //cos phi
    if(uhd < 5)
      phi = 0.00;
    else if(k>0.8&&k<=1.0) 
      phi=k;
      
    //P
    ptong=uhd*ihd1*phi;
    pluoi=uhd*ihd2*phi;  
    
    if(mode==1)   //mua dien
    {
      p_mua=pluoi;
      p_ban=0;
      pmattroi=ptong-pluoi;
    }
    if(mode==0)   //ban dien
    {
      p_ban=pluoi;
      p_mua=0;
      pmattroi=ptong+pluoi;
    }
    
    A_mattroi=A_mattroi + pmattroi*(millis()-tgtt)/3600000;
    A_tong=A_tong + ptong*(millis()-tgtt)/3600000; 
    
    hienthi();
    ghi();
    
    a_mua +=p_mua*(millis()-tgtt)/3600000;
    a_ban +=p_ban*(millis()-tgtt)/3600000;
    tgtt=millis();
 
    //Terminal
    mySerial.print("Uhd:");mySerial.println(uhd);
    mySerial.print("ihd1:");mySerial.println(ihd1);
    mySerial.print("ihd2:");mySerial.println(ihd2);
    mySerial.print("cos phi");mySerial.println(phi);
    mySerial.print("A_mattroi");mySerial.println(A_mattroi);
    mySerial.print("A_tong");mySerial.println(A_tong);
    mySerial.println("................................."); 

    //Save to array
    msg[0]=uhd;
    msg[1]=ihd1;
    msg[2]=ihd2; 
    msg[3]=phi;
    msg[4]=a_mua;
    msg[5]=a_ban; 
    msg[6]=A_mattroi;
    msg[7]=A_tong; 

    //Reset data
    for(int i=0;i<250;i++)
    {
      mu1[i]=0;mu2[i]=0;mi1[i]=0;mi2[i]=0;
    }
    
    //transmit rf 
    for(int i=0;i<1000;i++)
    {
      radio.write(&msg, sizeof(msg));
      delay(1);
    }
    //tg=micros();
    if(uhd<10)digitalWrite(PB1,1);  //Relay
    else digitalWrite(PB1,0);

    //Reset data
    i1_max=0;
    i2_max=0;
    uhd = 0;
    ihd1 = 0;
    ihd2 = 0; 
    phi = 0;  
  }
}

void hienthi()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);  lcd.print("V:");  lcd.print(uhd); lcd.print("  ");
  lcd.setCursor(9, 0);  lcd.print(" I:");  lcd.print(ihd1);  lcd.print(" ");
  
  lcd.setCursor(0, 1);  lcd.print("ph:"); lcd.print(phi); lcd.print(" ");
  lcd.setCursor(8, 1);  lcd.print("Pt:"); lcd.print(ptong); lcd.print(" ");
}
void ghi()
{
  myFile = SD.open("test.txt", FILE_WRITE);
  if(myFile) 
  { 
    String sd="  Udh= "+String(uhd)+ "  ihd1= "+String(ihd1)+ "  ihd2= "+String(ihd2)+ "  phi= "+String(phi)+"  A= "+String(A_tong);
    myFile.println(sd); 
    mySerial.println(sd);
    myFile.close();
    mySerial.println("done.");
    EEPROM.write(1,a_mua);
    EEPROM.write(2,a_ban);
    EEPROM.write(3,A_mattroi);
    EEPROM.write(4,A_tong);
  } 
  else 
  { 
    mySerial.println("error opening test.txt");
  }
}
