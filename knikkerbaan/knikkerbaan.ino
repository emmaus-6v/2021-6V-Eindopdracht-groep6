//==========================================================================================================================
//==========================================code 2 sensors bij elkaar============================================================
//==========================================================================================================================

#include <Servo.h> // motor

#define LEDPIN 13 // ledje voor motor
 
#define SENSORPIN 4 // ir sensor 1 aangesloten
  
#include <Adafruit_NeoPixel.h> // library voor neopixel

#define PIN 2	 // input pin Neopixel is attached to

#define NUMPIXELS 20 // number of neopixels in strip

#define SENSORPIN2 12 // tweede ir sensor
 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 50; // timing delay in milliseconds

// voor het random effect van de neopixel
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

// servo variabelen
Servo myservo; 
int pos = 0;  

// ir sensors variabelen
int sensorState = 0, lastState=0;
int sensorState2 = 0, lastState2 =0;

//===============================SETUP=========================================
void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();   
  pixels.setBrightness(64); 

  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT); 
  
  // motor op pin 9
  myservo.attach(9);
  
  // initialize the sensor 1 pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  
  // initialize the sensor 2 pin as an input:
  pinMode(SENSORPIN2, INPUT);     
  digitalWrite(SENSORPIN2, HIGH); // turn on the pullup
  
  Serial.begin(9600);
}


//=========================LOOP======================================
void loop() {
  
  // read the state of the sensors value:
  sensorState = digitalRead(SENSORPIN);
  sensorState2 = digitalRead(SENSORPIN2);
  
  //-------- ledje bij de motor---------------------------------
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    } 
  
  //----------motor anticiperen op sensor1 waarde ---------------------
  if (!sensorState && lastState) {
    Serial.println("Broken");
      for (pos = 0; pos <= 55; pos += 1) {                  
      myservo.write(pos);              
      delay(10); 
      }
      for (pos = 55; pos >= 0; pos -= 1) { 
      myservo.write(pos);             
        delay(10);                       
    }
  }
  lastState = sensorState;
  
  //---------------neopixel anticiperen op sensor 2---------------------
  if (sensorState2 && !lastState2) {
    Serial.println("Unbroken");
    } 
 
  if (!sensorState2 && lastState2) {
    Serial.println("Broken");
      setColor();
  
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
    
    // This sends the updated pixel color to the hardware.
    pixels.show();

    // Delay for a period of time (in milliseconds).
    delay(delayval);
    }  
  }
  lastState2 = sensorState2;
}
//------------------- loop is beeindigd-----------------------------------
// setColor()
// picks random values to set for RGB
void setColor(){
  redColor = random(0, 255);
  greenColor = random(0,255);
  blueColor = random(0, 255);
}


//==================================================================================================================
//====================================alle code + wifi bij elkaar===================================================
//==================================================================================================================

// voor de wifi 
#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>

// voor de sensoren, motor en ledstrip
#include <Servo.h> // motor
#define LEDPIN 13 // ledje voor motor
#define SENSORPIN 4 // ir sensor 1 aangesloten
#include <Adafruit_NeoPixel.h> // library voor neopixel
#define PIN 2	 // input pin Neopixel is attached to
#define NUMPIXELS 12 // number of neopixels in strip
#define SENSORPIN2 12 // tweede ir sensor
 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

char ssid[] = "Nacho Wifi";    // naam van het WiFi-netwerk. Dat heet SSID
char pass[] = "FreeWifi";                  // WiFi-wachtwoord

int status = WL_IDLE_STATUS;

//domeinnaam server
char server[] = "3000-brown-catshark-l4u1rl9i.ws-eu03.gitpod.io";

// We maken een client-object aan dat berichten naar de server kan sturen
// en antwoord terug kan krijgen.
WiFiSSLClient client;

int delayval = 50; // timing delay in milliseconds

// voor het random effect van de neopixel
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

// servo variabelen
Servo myservo; 
int pos = 0;  

// ir sensors variabelen
int sensorState = 0, lastState=0;
int sensorState2 = 0, lastState2 =0;
int totalKnikkers = 0;

// statusuitlezen 
long statusTimerLaatsteActivering = 0;
int  statusTimerWachtTijd = 1000;  // elke seconde 'tellen' we een knikker

// wifi regeling
long wifiTimerLaatsteActivering = 0;
int  wifiTimerWachtTijd = 30000; // elke 30 seconden sturen we een update naar de server

//============================setup===============================================================
void setup() {
  // De Arduino UNO WiFi Rev2 heeft een kleurenled. Die gebruiken we voor de WiFi-status:
  // Zet LED op rood:
  setWiFiLED(255, 0, 0);
  
  // Zet de seriele poort klaar zodat je berichten naar je
  // computer kunt sturen:
  Serial.begin(9600);
  while (!Serial) {
    ; // we wachten hier net zolang tot de seriele poort verbinding heeft
  }

  // verbind met de WiFi
  checkWiFiModule();
  verbindMetWiFiNetwerk();

  // als je hier bent, werkt de WiFi, zet led op groen:
  setWiFiLED(0, 255, 0);

  // initialiseer hier je timers:
  // zet de start, t=0, van de timers op NU,
  // zodat die 1 en 30 seconden vanaf NU voor het eerst 'afgaan'.
  statusTimerLaatsteActivering = millis();
  wifiTimerLaatsteActivering=  millis();

  //-----------setup voor led, servo, ledstrip en sensors -------------------------
  // Initialize the NeoPixel library.
  pixels.begin();
  pixels.set Brightness(64);

  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT); 
  
  // motor op pin 9
  myservo.attach(9);
  
  // initialize the sensor 1 pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  
  // initialize the sensor 2 pin as an input:
  pinMode(SENSORPIN2, INPUT);     
  digitalWrite(SENSORPIN2, HIGH); // turn on the pullup
  
  Serial.begin(9600);
}

//==============================loop========================================================
void loop() {
  
  //---------------wifitimer-------------------------------------
  // controleer of de wifiTimer 'af moet gaan'
  if (millis() > wifiTimerLaatsteActivering + wifiTimerWachtTijd) {
    // er zijn blijkbaar meer van 30000 milliseconden verstreken
    // sinds de laatste activering van de wifiTimer
    
    // update de tijd van de laatste activering naar NU
    wifiTimerLaatsteActivering = millis();

    // tijdens communicatie zetten we de LED even op blauw
    setWiFiLED(0, 0, 255);
    
    //stuur update naar de server
    stuurUpdate1();
    stuurUpdate2();
    stuurUpdate2();
  }

 
  // Je wilt in ieder geval kunnen lezen wat de
  // server als antwoord terugstuurt.
  if (client.connected()) {
    checkHTTPResponse();

    // na antwoord zetten we LED weer op groen:
    setWiFiLED(0, 255, 0);
  }
  
  // hele korte delay om de Arduino niet helemaal gek te maken
  delay(50);
  
    // read the state of the sensors value:
  sensorState = digitalRead(SENSORPIN);
  sensorState2 = digitalRead(SENSORPIN2);
  
  //-------- ledje bij de motor---------------------------------
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
  }
  
  if (sensorState && !lastState) {
    Serial.println("Unbroken");
    } 
  
  //----------motor anticiperen op sensor1 waarde ---------------------
  if (!sensorState && lastState) {
    Serial.println("Broken");
      for (pos = 0; pos <= 55; pos += 1) {                  
      myservo.write(pos);              
      delay(10); 
      }
      for (pos = 55; pos >= 0; pos -= 1) { 
      myservo.write(pos);             
        delay(10);                       
    }
    totalKnikkers = totalKnikkers + 1;
  }
  lastState = sensorState;
  
  //---------------neopixel anticiperen op sensor 2---------------------
  if (sensorState2 && !lastState2) {
    Serial.println("Unbroken");
    } 
 
  if (!sensorState2 && lastState2) {
    Serial.println("Broken");
      setColor();
  
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
    
    // This sends the updated pixel color to the hardware.
    pixels.show();

    // Delay for a period of time (in milliseconds).
    delay(delayval);
    }  
  }
  lastState2 = sensorState2;
}
//================ einde loop====================================================

//========================FUNCTIES=============================================

//----------------functie stuurupdate1--------------------------------------------
void stuurUpdate1() {
  Serial.println("\nStart verbinding met server");

  // hier maken we gebruik van het client-object om
  // de verbinding te maken. Die geeft true of false
  // terug om aan te geven of het is gelukt.
  // Het kan even duren voor de verbinding is gemaakt.
  if (client.connect(server, 443)) {
    Serial.println("Verbonden met de server. HTTP verzoek wordt verstuurd.");
    
    // We sturen nu m.b.v. het client-object de tekst van een HTTP verzoek:
    // 1e regel
    client.print("GET /api/sensorstatus1");
    client.print(sensorstate/);
    client.println(" HTTP/1.1");

    // 2e regel
    client.print("Host: 3000-brown-catshark-l4u1rl9i.ws-eu03.gitpod.io ");
    client.println(server);

    // 3e regel
    client.println("Connection: close");

    // 4e regel -- moet leeg zijn
    client.println();

    /* Dat verzoek ziet er dus zo uit als baanstatus de waarde 8 heeft:
     GET /api/setKnikkerbaanStatus/8 HTTP/1.1
     Host: 3000-blablabla.gitpod.io
     Connection: close
     
    */
  }
  else {
    Serial.println("verbinding maken niet gelukt");
  } 
}

//--------------------functie stuurupdate2-----------------------------
void stuurUpdate2() {
  Serial.println("\nStart verbinding met server");

  // hier maken we gebruik van het client-object om
  // de verbinding te maken. Die geeft true of false
  // terug om aan te geven of het is gelukt.
  // Het kan even duren voor de verbinding is gemaakt.
  if (client.connect(server, 443)) {
    Serial.println("Verbonden met de server. HTTP verzoek wordt verstuurd.");
    
    // We sturen nu m.b.v. het client-object de tekst van een HTTP verzoek:
    // 1e regel
    client.print("GET /api/sensorstatus2/");
    client.print(sensorstate2);
    client.println(" HTTP/1.1");

    // 2e regel
    client.print("Host: 3000-brown-catshark-l4u1rl9i.ws-eu03.gitpod.io ");
    client.println(server);

    // 3e regel
    client.println("Connection: close");

    // 4e regel -- moet leeg zijn
    client.println();

    /* Dat verzoek ziet er dus zo uit als baanstatus de waarde 8 heeft:
     GET /api/setKnikkerbaanStatus/8 HTTP/1.1
     Host: 3000-blablabla.gitpod.io
     Connection: close
    */
  }
  else {
    Serial.println("verbinding maken niet gelukt");
  }
}

//--------------------functie stuurupdate3-----------------------------
void stuurUpdate3() {
  Serial.println("\nStart verbinding met server");

  // hier maken we gebruik van het client-object om
  // de verbinding te maken. Die geeft true of false
  // terug om aan te geven of het is gelukt.
  // Het kan even duren voor de verbinding is gemaakt.
  if (client.connect(server, 443)) {
    Serial.println("Verbonden met de server. HTTP verzoek wordt verstuurd.");
    
    // We sturen nu m.b.v. het client-object de tekst van een HTTP verzoek:
    // 1e regel
    client.print("GET /api/getTotalKnikkers/");
    client.print(totalKnikkers);
    client.println(" HTTP/1.1");

    // 2e regel
    client.print("Host: 3000-brown-catshark-l4u1rl9i.ws-eu03.gitpod.io ");
    client.println(server);

    // 3e regel
    client.println("Connection: close");

    // 4e regel -- moet leeg zijn
    client.println();

    /* Dat verzoek ziet er dus zo uit als baanstatus de waarde 8 heeft:
     GET /api/setKnikkerbaanStatus/8 HTTP/1.1
     Host: 3000-blablabla.gitpod.io
     Connection: close
    */
  }
  else {
    Serial.println("verbinding maken niet gelukt");
  }
}


//------------------functie checkHTTPResponse-------------------------------------
void checkHTTPResponse() {
  // controleer of de client data beschikbaar heeft
  // lees deze uit en print ze, net zolang tot er
  // geen nieuwe data meer beschikbaar is
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // mocht de server na het antwoord toch zelf niet
  // de verbinding hebben verbroken, doe het dan maar zelf
  if (!client.connected()) {
    Serial.println("");
    Serial.println("---------EINDE ANTWOORD VAN DE SERVER----------");
    Serial.println("De verbinding met de server is verbroken");
    client.stop();
  }
}

//-----------------functie checkWifiModule-------------------------------
void checkWiFiModule() {
  // controleer of er een WiFi module is
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // controleer de versie van de firmware
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
}

//--------------------functie verbindMetWifiNetwerk---------------------
void verbindMetWiFiNetwerk() {
  // probeer contact te maken met het opgegeven WiFi-netwerk:
  // dit gaat met een while-loop, zodat je niet verder kunt zolang
  // je niet verbonden bent.
  while (status != WL_CONNECTED) {
    Serial.print("Probeert te verbinden met SSID: ");
    Serial.println(ssid);
    
    // verbind met WiFi.begin
    status = WiFi.begin(ssid, pass);

    // wacht 4 seconden voor de verbinding:
    // mocht het verbinden niet lekker gaan, probeer er dan
    // 10000 milliseconden van te maken (10 seconden)
    delay(4000);
  }
  // als je hier komt, betekent dat dat je verbonden bent.  
  Serial.println("Verbonden met WiFi");

  // print informatie over het netwerk waarmee je verbonden bent
  printWiFiStatus();
}

//-------------------functie printWifiStatus--------------------------
void printWiFiStatus() {
  // print de naam van het netwerk waarmee je verbonden bent:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print het IP-adres dat je Arduino heeft gekregen:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP-adres: ");
  Serial.println(ip);

  // print de sterkte van het signaal:
  long rssi = WiFi.RSSI();
  Serial.print("Signaalsterkte (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

//----------------functie setWifiLed---------------------------------
// stel de kleur van de RGB led in
void setWiFiLED(uint8_t rood, uint8_t groen, uint8_t blauw) {
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
  WiFiDrv::analogWrite(25, rood);
  WiFiDrv::analogWrite(26, groen);
  WiFiDrv::analogWrite(27, blauw);
}
