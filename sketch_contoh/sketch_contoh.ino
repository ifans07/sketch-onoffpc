/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP8266 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLTo1Fp77w"
#define BLYNK_TEMPLATE_NAME "Smarthome"
#define BLYNK_AUTH_TOKEN "soXZJg279hW3tn231h_fE42iLqSkCGDO"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PZEM004Tv30.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char token[] = BLYNK_AUTH_TOKEN;
char ssid[] = "fh_8f5538 - 2";
char pass[] = "wlan70aac7";

// SET UP sensor ultrasonic dan relay
int pinRelay = 4;
int pinLed1 = D3;
int pinTrig = 5;
int pinEcho = 16;
int pinLed2 = D5;


long timer;
int jarak;
int jarakNyala = 5;

int setPinRelay;
int setActive;

PZEM004Tv30 pzem(12, 13); //12=d6 (rx) , 13=d7 (tx)
float PowerKW, Energi, Voltase, currentAmpere;

BLYNK_WRITE(V0){
  int pinValue = param.asInt();
  if(pinValue == HIGH){
    digitalWrite(pinLed1, LOW);
  }else{
    digitalWrite(pinLed1, HIGH);
  }
}

// MENYALAKAN KOMPUTER DARI BLYNK
BLYNK_WRITE(V1){
  int pinValue = param.asInt();
  digitalWrite(pinRelay, pinValue);
  delay(500);
  if(pinValue == HIGH){
    setPinRelay = 1;
  }else{
    setPinRelay = 1;
  }
}

BLYNK_WRITE(V3){
  int pinValue = param.asInt();
  if(pinValue == HIGH){
    setActive = 0;
    setPinRelay = 1;
    digitalWrite(pinLed2, HIGH);
    digitalWrite(pinLed1, LOW);
    Blynk.virtualWrite(V4, LOW);
    Blynk.virtualWrite(V5, HIGH);
  }else{
    setActive = 1;
    setPinRelay = 0;
    digitalWrite(pinLed2, LOW);
    digitalWrite(pinLed1, HIGH);
    Blynk.virtualWrite(V4, HIGH);
    Blynk.virtualWrite(V5, LOW);
  }
}

BLYNK_CONNECTED(){
  // Blynk.syncVirtual(V0);
  // Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V3);
  // Blynk.syncVirtual(pinLed2);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V6);
}


void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(token, ssid, pass, "blynk.cloud", 80);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  pinMode(pinRelay, OUTPUT);
  pinMode(pinLed1, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  
  digitalWrite(pinRelay, HIGH);
  // digitalWrite(pinLed2, LOW);
}

void loop()
{
  Blynk.run();
  if(setPinRelay == 0){
    digitalWrite(pinRelay, HIGH);
  }
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!


  // pin power
  PowerKW = pzem.power();
  if(isnan(PowerKW)){
    Serial.println("gagal");
    Blynk.virtualWrite(V7, 0);
    
  }else{
    Serial.print("Power: "); Serial.print(PowerKW); Serial.println("kW");
    Blynk.virtualWrite(V7, PowerKW);
    if(PowerKW > 3){
      Blynk.virtualWrite(V6, HIGH);
    }else{
      Blynk.virtualWrite(V6, LOW);
    }
  }
  Energi = pzem.energy();
  if(isnan(Energi)){
    Serial.println("gagal");
    Blynk.virtualWrite(V8, 0);
  }else{
    Serial.print("Energi: "); Serial.print(Energi); Serial.println("kWh");
    Blynk.virtualWrite(V8, Energi);
  }
  Voltase = pzem.voltage();
  if(isnan(Voltase)){
    Serial.println(Voltase);
    Blynk.virtualWrite(V9, 0);
  }else{
    Serial.print("Voltase: "); Serial.print(Voltase); Serial.println("V");
    Blynk.virtualWrite(V9, Voltase);
  }
  currentAmpere = pzem.current();
  if(isnan(currentAmpere)){
    Serial.println("gagal");
  }else{
    Serial.print("Ampere/ current: "); Serial.print(currentAmpere); Serial.println("A");
  }
  Serial.println();
  delay(250);
  // if(digitalRead(pinPower) == 1){
  //   Blynk.virtualWrite(V6, HIGH);
  // }else{
  //   Blynk.virtualWrite(V6, LOW);
  // }

  // akhir pin power

 
  // SENSOR JARAK
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(5);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  timer = pulseIn(pinEcho, HIGH);
  jarak = timer/58;
  delay(250);

  Serial.print("Jarak: "); Serial.print(jarak); Serial.print(" cm");
  Serial.println();
  // AKHIR SENSOR JARAK


  // KIRIM SENSOR JARAK KE BLYNK
  Blynk.virtualWrite(V2, jarak);
  // AKHIR KIRIM SENSOR JARAK KE BLYNK


  // MENYALAKAN PC dengan sensor jarak
  if(PowerKW >= 0 && PowerKW < 3){
   if(setActive == 0){
    if(jarak <= jarakNyala){
      digitalWrite(pinRelay, LOW);
      digitalWrite(pinLed1, HIGH);
      digitalWrite(pinLed2, HIGH);
      setPinRelay = 1;
    }else{
      digitalWrite(pinRelay, HIGH);
      digitalWrite(pinLed1, LOW);
      digitalWrite(pinLed2, HIGH);
    }
   }else{
     digitalWrite(pinRelay, HIGH);
     delay(250);
   }
  }else{
    digitalWrite(pinRelay, HIGH);
    delay(250);
  }
  // AKHIR MENYALAKAN PC
  
}
