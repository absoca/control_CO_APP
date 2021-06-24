// ESP8266 PROGRAM
#include <ESP8266WiFi.h>
#include <espnow.h>

//PIN PLACA D4: Es el TX1 (Serial 1)

//Se establece para realizar broadcast a todos los receptores ESP8266
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Estructura de datos
typedef struct struct_message {
    char mac[18];
    int valor;
} struct_message;

// Creamos una estructura de datos
  //Para envío
  struct_message dataSend;
   //Para recepción
  struct_message dataRecv;

void enviarDatosSerializados(){ 
  Serial.println("Envío trama por serial 1");
  //Enviamos los datos del sensor remoto recibido
  Serial1.print("{\"MAC\":\"");
  Serial1.print(dataRecv.mac);
  Serial1.print("\",\"data\":");
  Serial1.print(dataRecv.valor);
  Serial1.print("}");

  
}

// Función de callback cuando se ha mandado la información
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&dataRecv, incomingData, sizeof(dataRecv));
  /*Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("MAC: ");
  Serial.println(dataRecv.mac);
  Serial.print("Data: ");
  Serial.println(dataRecv.valor);
  Serial.println();*/

  enviarDatosSerializados();
}

void setup() {
  Serial.begin(9600);  
  Serial1.begin(9600);
    
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Iniciamos ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Despues de la inicialización de ESPNow establecemos el rol
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Registramos el emparejado
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  //Establecemos los valores a enviar
  WiFi.macAddress().toCharArray(dataSend.mac, WiFi.macAddress().length() + 1);
  
  //Debemos leer el valor del sensor
  dataSend.valor = random(0, 1023);

  //Enviamos los datos del propio sensor
  Serial1.print("{\"MAC\":\"");
  Serial1.print(dataSend.mac);
  Serial1.print("\",\"data\":");
  Serial1.print(dataSend.valor);
  Serial1.print("}");

  esp_now_send(broadcastAddress, (uint8_t *) &dataSend, sizeof(dataSend));
  delay(3777);
}
