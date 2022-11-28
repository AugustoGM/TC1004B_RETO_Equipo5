//Biblioteca necesaria para conectar el ESP32 a internet
#include "Arduino.h"
#include <esp_wpa2.h>
#include <WiFi.h>
//Biblioteca necesaria para conectare el ESP32 a firebase
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include "addons/RTDBHelper.h"
//Biblioteca para sensor ultrasonico
#include "NewPing.h"       
//Se incluye la biblioteca DHT sensor y se definen las variables necesarias para poder usarla
#include "DHT.h"
//Conexion para el sensor de temperatura/humedad
#define DHTPIN 4
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

//Conexion para el sensor de gas
int Gas_analog = 39; GIOP39

//Conexiones para el senor ultrasonico
int trigPin = 0;      // trigger pin GIOP0
int echoPin = 2;      // echo pin GIOP2
NewPing sonar(trigPin, echoPin);

//Conexion para sensor pir
const int PIN_TO_SENSOR = 15; // GIOP15 
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin

//Valores para conectar a redes institucionales
const char* ssid = "Tec";
#define EAP_IDENTITY "yourEmail@tec.mx"
#define EAP_PASSWORD "yourpassword"

//Se asigna el usuario desde el que se enviaran los registros a la base de datos de Firebase
// Firebase insert auth
#define USER_EMAIL "yourEmail@gmail.com"
#define USER_PASSWORD "yourpassword"

//Se cambio a la API Key correspondiente a la base de datos 
#define API_KEY "yourApiKey"//AIzaSyAjjTHMIV0y394tayvijhU-aVVcKdkIZxU

//Se cambio a la URL de la base de datos del equipo
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://yourURL.firebaseio.com/"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//Investigar
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

//Conexiones display 7 segmentos {A, B, C, D, E, F, G}
int LEDs[] = {12,32,33,25,26,27,14};    // for ESP32 microcontroller

//se declaran los arreglos que forman los dígitos
int zero[] = {1, 1, 1, 1, 1, 1, 0};   // cero
int one[] = {0, 1, 1, 0, 0, 0, 0};   // uno
int two[] = {1, 1, 0, 1, 1, 0, 1};   // dos
int three[] = {1, 1, 1, 1, 0, 0, 1};   // tres
int four[] = {0, 1, 1, 0, 0, 1, 1};   // cuatro 
int five[] = {1, 0, 1, 1, 0, 1, 1};   // cinco
int six[] = {1, 0, 1, 1, 1, 1, 1};   // seis
int seven[] = {1, 1, 1, 0, 0, 0, 0};   // siete
int eight[] = {1, 1, 1, 1, 1, 1, 1}; // ocho
int nine[] = {1, 1, 1, 1, 0, 1, 1};   // nueve
int ten[] = {0, 0, 0, 0, 0, 0, 1};   // diez

//Declarar numero para el valor que se recibira de firebase
int numero2 = 0; 

//función que llama a cada valor que tendra el display dependiendo del valor de entrada
void segment_display(unsigned char valor)
{
    switch(valor)
    {
        case 0:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], zero[i]);
                    break;
        case 1:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], one[i]);
                    break;
        case 2:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], two[i]);
                    break;
        case 3:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], three[i]);
                    break;
        case 4:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], four[i]);
                    break;
        case 5:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], five[i]);
                    break;
        case 6:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], six[i]);
                    break;
        case 7:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], seven[i]);
                    break;
        case 8:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], eight[i]);
                    break;
        case 9:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], nine[i]);
                    break;
        default:
                    for (int i = 0; i<7; i++) digitalWrite(LEDs[i], ten[i]);
                    break;          
    }
}

void setup() {
    Serial.begin(115200);

    //Sensor Pir
    pinMode(PIN_TO_SENSOR, INPUT);

    //Display 7 segmentos
    for (int i = 0; i<7; i++) pinMode(LEDs[i], OUTPUT);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    // WPA2 enterprise magic starts here
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);   //init wifi mode
    //esp_wifi_set_mac(ESP_IF_WIFI_STA, &masterCustomMac[0]);
    Serial.print("MAC >> ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Connecting to WiFi: %s ", ssid);
    //esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    //esp_wpa2_config_t configW = WPA2_CONFIG_INIT_DEFAULT();
    //esp_wifi_sta_wpa2_ent_enable(&configW);
    esp_wifi_sta_wpa2_ent_enable();
    // WPA2 enterprise magic ends here
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

///* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

// Hay un cambio en los valores que se envian a la base de datos mediante la funcion loop
void loop() {
    //Lectura sensor de movimiento
    int mov;
    pinStatePrevious = pinStateCurrent; //guardar estado previo
    pinStateCurrent = digitalRead(PIN_TO_SENSOR);   //leer nuevo estado
    if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // cambia estado del Pin: LOW -> HIGH
      mov = 1;
      Serial.println("Motion detected!");
    }
    else 
      if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
        mov = 0;
        Serial.println("Motion stopped!");
      }
    
    //Lectura sensor ultrasonico
    float distance = sonar.ping_cm(15);
    if(distance>400 || distance<2) distance = -1;

    //Lectura sensor Gas
    int gas = analogRead(Gas_analog);

    // Se crean las variables que conteneran los datos registrados por el sensor DHT11
    //Leer humedad
    float humidity = dht.readHumidity();
    //Leer temperatura en Celsius
    float temperatureC = dht.readTemperature();
    //Leer temperatura en Fahrenheit (isFahrenheit = true)
    float temperatureF = dht.readTemperature(true);

    // Calcular el indice de calor en Fahrenheit(default)
    float hif = dht.computeHeatIndex(temperatureF, humidity);
    // Calcular el indice de calor en Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(temperatureC, humidity, false);

    //Se enviaran los datos a la base de datos periódicamente (si el registro es exitoso y todo está configurado)
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Escribe humedad en la ruta test/humedad de la base de datos
    if (Firebase.RTDB.setInt(&fbdo, "test/humedad", humidity)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Escribe humedad en la ruta test/temperatura(C)
    if (Firebase.RTDB.setFloat(&fbdo, "test/temperatura(C)",hic)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Escribe temperatura(F) en la ruta test/temperatura(F)
    if (Firebase.RTDB.setFloat(&fbdo, "test/temperatura(F)",hif)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Escribe distancia en la ruta test/distancia
    if (Firebase.RTDB.setFloat(&fbdo, "test/distancia",distance)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Escribe gas en la ruta test/gas
    if (Firebase.RTDB.setInt(&fbdo, "test/gas",gas)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Escribe movimiento en la ruta test/movimiento
    if (Firebase.RTDB.setInt(&fbdo, "test/movimiento",mov)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // Recibir el valor del numero de la base de datos con ruta en /test/numero
    if (Firebase.RTDB.getString(&fbdo, "/test/numero")){
      String numero1 = fbdo.stringData();
      numero2 = numero1.toInt(); 
      segment_display(numero2);
    }
  }
}
