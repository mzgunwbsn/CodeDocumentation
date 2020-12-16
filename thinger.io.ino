#include <CTBot.h>
//#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ThingerESP8266.h>
#define USERNAME "mzgunwbsn"
#define DEVICE_ID "NodeMcu_waterlevel"
#define DEV_CREDENTIAL "xsBA9s75j0F-"

CTBot smbot;
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "id.pool.ntp.org", 25200);
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEV_CREDENTIAL);
uint8_t relay = 2;
int trig_pin = 4;
int echo_pin = 5;
long echotime;
float distance;
float waterlevel;
float watervolume;
float poolheight = 20;
float poolwidth = 19;
float poollength = 39;
float debitair;
float Rdistance;
float gap = 6;
String control = "off";
String pumpstatus = "off";
String  debitcheck = "off";
String ssid = "hotspott";
String pass = "12345678";
const char* ssid1 = "hotspott";
const char* pass1 = "12345678";
String token = "1468460060:AAEViAgxZM4HIj866wpZlquOvtF63IcamzY";
String report;


void connecttoWifi() {
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );

  }
  Serial.print("\nWifi Connected!\n");
  thing.add_wifi(ssid1, pass1);
}

void connecttoTelegram()
{
  Serial.println("Starting TelegramBot..");
  smbot.wifiConnect(ssid, pass);
  smbot.setTelegramToken(token);
  if (smbot.testConnection())
    Serial.println("\nConnection OK");
  else
    Serial.println("\nConnection NOK");
}


void sensor()
{
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  echotime = pulseIn(echo_pin, HIGH);
  distance = 0.0001 * ((float)echotime * 340.0) / 2.0;
  Rdistance = distance - gap;
  waterlevel = poolheight - Rdistance;
  watervolume = poollength * poolwidth * waterlevel;
  watervolume /= 1000;
  Serial.print((String) "\nvolume air saat ini : " + watervolume);
  Serial.println(" L");
  Serial.println((String) "Ketinggian air saat ini  : " + waterlevel + (String) " CM");
  if (waterlevel >= 14 && waterlevel <= 19)
  {
    digitalWrite(relay, LOW);
    Serial.print("Pompa Menyala!");
  }
  else
  {
    digitalWrite(relay,HIGH);
    Serial.print("Pompa Mati!");
  }
  delay(500);
}

//void autoControl(int a)
//{
//  if (waterlevel >= 14 && waterlevel <= 19)
//  {
//    String awas = (String) "STATUS AIR SAAT INI BAHAYA!\nMENYALAKAN POMPA SECARA OTOMATIS\n KETINGGIAN AIR SAAT INI : " + waterlevel + (String) " CM";
//    Serial.println(awas);
//
//    smbot.sendMessage(a, awas);
//  }
//  else if (waterlevel > 7 && waterlevel < 14)
//  {
//    String waspada = (String) "STATUS AIR SAAT INI WASPADA!\nKETINGGIAN AIR SAAT INI : " + waterlevel + (String) " CM";
//    Serial.println(waspada);
//
//    digitalWrite(relay, HIGH);
//    smbot.sendMessage(a, waspada);
//  }
//  else
//  {
//    String aman = (String) "STATUS AIR SAAT INI AMAN\nKETINGGIAN AIR SAAT INI : " + waterlevel + (String) " CM";
//    Serial.println(aman);
//
//    digitalWrite(relay, HIGH);
//    smbot.sendMessage(a, aman);
//  }
//}
//void debiton()
//{
//  digitalWrite(relay, LOW);
//}





void setup()
{
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  digitalWrite(relay, HIGH);
  connecttoTelegram();
  connecttoWifi();
  thing["jarak"] >> [] (pson & out) {
    out["waterlevel"] = waterlevel;
    out["watervolume"] = watervolume;
  };
  Serial.print("Data Terkirim");
}

void loop()
{
  thing.handle();
  TBMessage msg;

  sensor();
  if (control == "on") {
    if (waterlevel >= 14 && waterlevel <= 19) {
      String awas = (String)"STATUS AIR SAAT INI BAHAYA!\nMENYALAKAN POMPA SECARA OTOMATIS\n KETINGGIAN AIR SAAT INI : " + waterlevel + (String)" CM";
      Serial.println(awas);

      smbot.sendMessage(msg.sender.id, awas);
    }
    else if (waterlevel > 7 && waterlevel < 14) {
      String waspada = (String)"STATUS AIR SAAT INI WASPADA!\nKETINGGIAN AIR SAAT INI : " + waterlevel + (String)" CM";
      Serial.println(waspada);

      smbot.sendMessage(msg.sender.id, waspada);
    }
    else {
      String aman = (String)"STATUS AIR SAAT INI AMAN\nKETINGGIAN AIR SAAT INI : " + waterlevel + (String)" CM";
      Serial.println(aman);

 
      smbot.sendMessage(msg.sender.id, aman);
    }
  }



  if (debitcheck == "on"){
    digitalWrite(relay, LOW);
  }
  else if (debitcheck == "off"){
    digitalWrite(relay, HIGH);
  }
  if (smbot.getNewMessage(msg)) {

    Serial.print("Pesan Masuk dari @" + msg.sender.username + "(" + msg.sender.id + ")" + msg.text + "\n");

    if (msg.text.equalsIgnoreCase("/start"))
    {

      String welcome = (String) "Halo, " + msg.sender.username + (String) " Perkenalkan saya Smart IOT Bot yang bisa mengontrol ketinggian air.\n silahkan cek command /help untuk menggunakan saya! semoga harimu menyenangkan.";
      String sent = (String) "Pesan Telah dikirimkan ke @" + msg.sender.username + (String) " dengan id : " + msg.sender.id;
      Serial.print(sent);
      smbot.sendMessage(msg.sender.id, welcome);
    }

    else if (msg.text.equalsIgnoreCase("/monitoring"))
    {
      smbot.sendMessage(msg.sender.id, "Tunggu sebentar Monitoring akan segera dilakukan");
      String sent = (String) "Pesan Telah dikirimkan ke @" + msg.sender.username + (String) " dengan id : " + msg.sender.id;
      Serial.print(sent);
      control = "on";
      delay(500);


    }

    else if (msg.text.equalsIgnoreCase("/stopmonitoring"))
    {
      if (control == "off")
      {
        String salah = "Anda belum memulai sistim monitoring\n silahkan start monitoring telebih dahulu di menu";
        smbot.sendMessage(msg.sender.id, salah);
        String sent = (String) "Pesan Telah dikirimkan ke @" + msg.sender.username + (String) " dengan id : " + msg.sender.id;
        Serial.print(sent);
      }
      else
      {
        String benar = "Tunggu sebentar monitoring akan segera berhenti";
        smbot.sendMessage(msg.sender.id, benar);
        delay(500);
        control = "off";
        smbot.sendMessage(msg.sender.id, "Monitoring selesai dilakukan");
        String sent = (String) "Pesan Telah dikirimkan ke @" + msg.sender.username + (String) " dengan id : " + msg.sender.id;
        Serial.print(sent);
      }
    }

    else if (msg.text.equalsIgnoreCase("/waterlevel"))
    {
      String water = (String) "Ketinggian Air Saat ini : " + waterlevel + (String) " CM";
      smbot.sendMessage(msg.sender.id, water);
      String sent = (String) "Pesan Telah dikirimkan ke @" + msg.sender.username + (String) " dengan id : " + msg.sender.id;
      Serial.print(sent);
    }
    else if (msg.text.equalsIgnoreCase("/TurnOnPump"))
    {
      debitcheck = "on";
      String PumpOnn = "Saat ini Pompa Sudah menyala";
      smbot.sendMessage(msg.sender.id, PumpOnn);
    }
    else if (msg.text.equalsIgnoreCase("/TurnOffPump"))
    {
      debitcheck = "off";
      String PumpOff = "Pompa Sudah mati.";
      smbot.sendMessage(msg.sender.id, PumpOff);
    }
    else if (msg.text.equalsIgnoreCase("/help"))
    {
      const char help[] = "Selamat datang di SMIOT Bot\n"
                          "bot ini dapat berfungsi untuk memantau ketinggian air\n"
                          "berikut adalah beberapa fungsi yang dapat digunakan\n"
                          "/monitoring - digunakan untuk auto monitoring \n"
                          "/stopmonitoring - digunakan untuk memberhentikan auto monitoring\n"
                          "/TurnOnPump - digunakan untuk menyalakan pompa secara manual\n"
                          "/TurnOffPump - digunakan untuk mematikan pompa secara manual\n"
                          "/help - untuk memunculkan command bot\n";

      smbot.sendMessage(msg.sender.id, help);
    }
    else
    {
      String reply;
      reply = (String) "Hai, " + msg.sender.username + (String) "Sepertinya kamu salah memasukkan command, coba cek /help ya";
      smbot.sendMessage(msg.sender.id, reply);
    }
  }
}
