#include<SoftwareSerial.h>
#include <ESP8266WiFi.h> 
const char* ssid = "Helloworld"; // your wireless network name (SSID)
const char* password = "dung251099"; // your Wi-Fi network password
SoftwareSerial mySUART(4, 2);
WiFiClient client;
String chuoi = "a100b200c300d333e222f111g444h555k10", chuoi1,chuoi2,chuoi3,chuoi4,chuoi5,chuoi6,chuoi7,chuoi8,chuoi9;
float f1 = 11;
float f2 = 22;
float f3 = 33;
float f4 = 44;
float f5 = 55; 
float f6 = 55; 
float f7 = 55; 
float f8 = 55; 
float f9 = 55; 
int giamattroi=1938;
int so1=50;int gia1=1678;
int so2=100;int gia2=1734;
int so3=200;int gia3=2014;
int so4=300;int gia4=2536;
int so5=400;int gia5=2834;
int a,b,c,d,e,f,g,h,k;
long thoigian;
// ThingSpeak Settings
const int channelID = 1826316; //
String writeAPIKey = "PV7R49IJTKHANBJW"; // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com"; 

void setup() {
        Serial.begin(9600); 
        mySUART.begin(9600);
        Serial.print("Connecting");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(100);
        }
        Serial.println("\r\nWiFi connected");
        thoigian=millis();
}

void loop() {
if (mySUART.available())
{
chuoi = mySUART.readString();
Serial.println(chuoi);
for (int i = 0; i < chuoi.length(); i++) 
    {
    if (chuoi.charAt(i) == 'a')a = i;  
    if (chuoi.charAt(i) == 'b')b = i; 
    if (chuoi.charAt(i) == 'c')c = i; 
    if (chuoi.charAt(i) == 'd')d = i; 
    if (chuoi.charAt(i) == 'e')e = i; 
    if (chuoi.charAt(i) == 'f')f = i; 
    if (chuoi.charAt(i) == 'g')g = i; 
    if (chuoi.charAt(i) == 'h')h = i; 
    if (chuoi.charAt(i) == 'k')k = i; 
    }
    
    chuoi1 = chuoi;
    chuoi2 = chuoi;
    chuoi3 = chuoi;
    chuoi4 = chuoi;
    chuoi5 = chuoi;
    chuoi6 = chuoi;
    chuoi7 = chuoi;
    chuoi8 = chuoi;
    chuoi9 = chuoi;
    chuoi1.remove(b);  
    chuoi1.remove(0,1); 
    
    chuoi2.remove(c);
    chuoi2.remove(a, b-a+1);  
     
    chuoi3.remove(d); 
    chuoi3.remove(a,c-a+1);

    chuoi4.remove(e); 
    chuoi4.remove(a,d-a+1);

    chuoi5.remove(f); 
    chuoi5.remove(a,e-a+1);

    chuoi6.remove(g);
    chuoi6.remove(a, f-a+1);  
    //a10b20c30d40e50f60g70h80k90 
    chuoi7.remove(h); 
    chuoi7.remove(a,g-a+1);

    chuoi8.remove(k); 
    chuoi8.remove(a,h-a+1);

    chuoi9.remove(a, k-a); 
    chuoi9.remove(0,1);
    
    f1 = (float)chuoi1.toInt()/100; //Chuy?n chuoi1 th�nh s 
    f2 = (float)chuoi2.toInt()/100; //Chuy?n chuoi2 th�nh s 
    f3 = (float)chuoi3.toInt()/100; //Chuy?n chuoi3 th�nh s 
    f4 = (float)chuoi4.toInt()/100; //Chuy?n chuoi4 th�nh s
    f5 = (float)chuoi5.toInt()/100; //Chuy?n chuoi5 th�nh s  
    f6 = (float)chuoi6.toInt()/100; //Chuy?n chuoi2 th�nh s 
    f7 = (float)chuoi7.toInt()/100; //Chuy?n chuoi3 th�nh s 
    f8 = (float)chuoi8.toInt()/100; //Chuy?n chuoi4 th�nh s
    f9 = (float)chuoi9.toInt()/100; //Chuy?n chuoi5 th�nh s
    
    Serial.print("Uhd:"); Serial.println(f1);
    Serial.print("ihd1:");Serial.println(f2);
    Serial.print("ihd2:");Serial.println(f3);
    Serial.print("cos phi:");Serial.println(f4);
    Serial.print("a_mua:");Serial.println(f5);
    Serial.print("a_ban:");Serial.println(f6);
    Serial.print("A_mattroi:");Serial.println(f7);
    Serial.print("A_tong:");Serial.println(f8);
    //Serial.print("Uhd:");Serial.println(f9); 
    
    f5=f5/1000*giamattroi;
    f6=f6/1000;
    if(f6<so1)f6=f6*so1;
    else
    if(f6<so2)f6=f6*so1+(f6-so1)*so2;
    else
    if(f6<so3)f6=f6*so1+f6*so2+(f6-so1-so2)*so3;
    else
    if(f6<so4)f6=f6*so1+f6*so2+(f6-so1-so2-so3)*so4;
    else 
    f6=f6*so1+f6*so2+f6*so3+f6*so4+(f6-so1-so2-so3-so4)*so5;
}
    if(millis()-thoigian>=10000)
    {    
        thoigian=millis();
        if (client.connect(server, 80)) {
                // Construct API request body
                String body = "field1=" + String(f1,2) + "&field2=" + String(f2,2) + "&field3=" + String(f3,2) + "&field4=" + String(f4,2) + "&field5=" + String(f5,2) + "&field6=" + String(f6,2) + "&field7=" + String(f7,2) + "&field8=" + String(f8,2);

                client.print("POST /update HTTP/1.1\n");
                client.print("Host: api.thingspeak.com\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(body.length());
                client.print("\n\n");
                client.print(body);
                client.print("\n\n");
                 
                Serial.println(".........................");   
                //client.print("http://api.thingspeak.com/update','api_key',writeApiKey,'status','good'"); 
        }
        client.stop();
    }
}
