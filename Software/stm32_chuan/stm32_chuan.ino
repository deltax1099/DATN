#include <SPI.h>
#include <SD.h>
#include "RF24.h" 
#include <SoftwareSerial.h> 
#include <SD.h>
#include <LiquidCrystal.h> 
#include <EEPROM.h>
LiquidCrystal lcd(PA8, PA10, PB4, PB5, PB6, PB7);
File myFile;
float phi; 
int mode=1;
float p_mua;float p_ban;
float a_mua;float a_ban;
float ptong,pmattroi,pluoi;
float A_tong,A_mattroi,A_luoi;
SoftwareSerial mySerial(PB10,PB11); // RX, TX
float u1,u2,i1,i2;
float uhd,ihd1,ihd2;
long tgtt;
const uint64_t pipe = 0xE8E8F0F0E1LL; // địa chỉ để phát
RF24 radio(PB0,PA4); 
float msg[8];
int k=0; 
int dem=0;
int mau=250;
double mu1[250],mu2[250],mi1[250],mi2[250];
const int sensor = A0;
int value = 0;long tg;
long tgtr;

void setup() {  
    mySerial.begin(9600);
    mySerial.println("Hello, world?");
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
    radio.setDataRate(RF24_1MBPS);    // Tốc độ truyền
    radio.setPALevel(RF24_PA_MAX);      // Dung lượng tối đa
    radio.setChannel(10);               // Đặt kênh
    radio.openWritingPipe(pipe);        // mở kênh 
    tg=micros();
    lcd.begin(16, 2);   
    lcd.print("waitting...");  
    //lcd.setCursor(0, 0);lcd.print("V :");
    if (!SD.begin(PC15)) {
        mySerial.println("initialization failed!"); 
    }
    a_mua    =EEPROM.read(1);
    a_ban    =EEPROM.read(2);
    A_mattroi=EEPROM.read(3);
    A_tong   =EEPROM.read(3);
}
//C:\Users\Admin\AppData\Local\Temp\arduino_build_532519
void loop() { 
    tgtr=micros()-tg; 
    if(k<mau)
        if(micros()-tg>=60033)
        {
            tg=micros();
            u1=analogRead(A0)*11.0/1023.0*3.3/1.0/12.0*220.0;
            u2=analogRead(A1)*11.0/1023.0*3.3/1.0/12.0*220.0;
            i1=((analogRead(A2)*13.3/1023*1000000)-2314174.00)/66000;
            i2=((analogRead(A3)*13.3/1023*1000000)-2314174.00)/66000;  
            mu1[k] = u1;
            mu2[k] = u2;
            mi1[k] = abs(i1);
            mi2[k] = abs(i2); 
            k++;
            if(i1>=0&&i2>=0){dem++;if(dem>150){mode=1;dem=0;}}
            else
                if(i1<=0&&i2<=0){dem++;if(dem>150){mode=1;dem=0;}}
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
    if(k>=mau)
    { 
        
        k=0;float u1_max,u2_max,i1_max,i2_max;
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
        float k=cos(acos(u1/u1_max)-acos(i1/i1_max));
        if(k>0.8&&k<=1.0)phi=k; 
        uhd=abs(u2_max/sqrt(2))*203*206/220/209 - 2.5;
        //if(uhd<=19.7)uhd=uhd;
        ihd1=abs(i1_max)/sqrt(2)-1.95;
        ihd2=abs(i2_max)/sqrt(2)-1.67; 
        i1_max=0;
        i2_max=0;
        ptong=uhd*ihd1*phi;
        pluoi=uhd*ihd2*phi;  
        if(mode==1){p_mua=pluoi;p_ban=0;pmattroi=ptong-pluoi;}//mua dien
        if(mode==0){p_ban=pluoi;p_mua=0;pmattroi=ptong+pluoi;}//ban dien
        A_mattroi=A_mattroi+pmattroi*(millis()-tgtt)/3600000;
        A_tong=A_tong+ptong*(millis()-tgtt)/3600000; 
        hienthi();
        ghi();
        a_mua+=p_mua*(millis()-tgtt)/3600000;
        a_ban+=p_ban*(millis()-tgtt)/3600000;
        tgtt=millis();
        mySerial.print("Uhd:");mySerial.println(uhd);
        mySerial.print("ihd1:");mySerial.println(ihd1);
        mySerial.print("ihd2:");mySerial.println(ihd2);
        mySerial.print("cos phi");mySerial.println(phi);
        mySerial.print("A_mattroi");mySerial.println(A_mattroi);
        mySerial.print("A_tong");mySerial.println(A_tong);
        mySerial.println("................................."); 
        msg[0]=uhd;
        msg[1]=ihd1;
        msg[2]=ihd2; 
        msg[3]=phi;
        msg[4]=a_mua;
        msg[5]=a_ban; 
        msg[6]=A_mattroi;
        msg[7]=A_tong; 
        for(int i=0;i<250;i++)
        {
            mu1[i]=0;mu2[i]=0;mi1[i]=0;mi2[i]=0;
        }
        for(int i=0;i<1000;i++)
        {
            radio.write(&msg, sizeof(msg));
            delay(1);
        }
        //tg=micros();
        if(uhd<10)digitalWrite(PB1,1);
        else digitalWrite(PB1,0);
        ihd1=0;
        ihd2=0; 
    }
    
    
}
void hienthi()
{
    lcd.setCursor(0, 0);lcd.print("V:");lcd.print(uhd);
    lcd.setCursor(9, 0);lcd.print("I:");lcd.print(ihd1);
    
    lcd.setCursor(0, 1);lcd.print("ph:");lcd.print(phi);
    lcd.setCursor(8, 1);lcd.print("At:");lcd.print(A_tong);
}
void ghi()
{
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) { 
        char sd[200];
        sprintf(sd,"%f  %d  %f  %f  %d  ",ihd1,ihd2,uhd,phi,A_tong); 
        myFile.println(sd); 
        myFile.close(); 
        EEPROM.write(1,a_mua);
        EEPROM.write(2,a_ban);
        EEPROM.write(3,A_mattroi);
        EEPROM.write(4,A_tong);
    } else { 
        mySerial.println("error opening test.txt");
    }
}
