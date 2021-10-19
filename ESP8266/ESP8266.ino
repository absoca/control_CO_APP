// ESP8266 PROGRAM
#include <ESP8266WiFi.h>
#include <espnow.h>

//PIN PLACA D4: Es el TX1 (Serial 1)

//---------- CONFIGURACION MQ-135 ----------
const int MQ_PIN = A0;      // Pin del sensor
const int RL_VALUE = 20;      // Resistencia RL del modulo en Kilo ohms
const int R0 = 3.7;          // Resistencia R0 del sensor en Kilo ohms

// Datos para lectura multiple
const int READ_SAMPLE_INTERVAL = 250;    // Tiempo entre muestras
const int READ_SAMPLE_TIMES = 10;       // Numero muestras

// Ajustar estos valores para vuestro sensor según el Datasheet
const float X0 = 10;
const float Y0 = 2.3;
const float X1 = 200;
const float Y1 = 0.8;

// Puntos de la curva de concentración {X, Y}
const float punto0[] = { log10(X0), log10(Y0) };
const float punto1[] = { log10(X1), log10(Y1) };

// Calcular pendiente y coordenada abscisas
const float scope = (punto1[1] - punto0[1]) / (punto1[0] - punto0[0]);
const float coord = punto0[1] - punto0[0] * scope;

//---------- CONFIGURACION ESP-NOW ----------
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
    Serial.println("Datos enviados correctamente");
  }
  else{
    Serial.println("El envío de datos ha fallado");
  }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  Serial.println("Datos recibidos");
  memcpy(&dataRecv, incomingData, sizeof(dataRecv));
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("MAC: ");
  Serial.println(dataRecv.mac);
  Serial.print("Datos: ");
  Serial.println(dataRecv.valor);
  Serial.println();

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
  //dataSend.valor = random(0, 1023);

  float rs_med = readMQ(MQ_PIN);      // Obtener la Rs promedio
  float concentration = getConcentration(rs_med/R0);   // Obtener la concentración
  dataSend.valor = (int) concentration;
  // Mostrar el valor de la concentración por serial
  Serial.print("Concentración: ");
  Serial.println(concentration);
  Serial.println(dataSend.valor);

  //Enviamos los datos del propio sensor
  Serial1.print("{\"MAC\":\"");
  Serial1.print(dataSend.mac);
  Serial1.print("\",\"data\":");
  Serial1.print(dataSend.valor);
  Serial1.print("}");

  esp_now_send(broadcastAddress, (uint8_t *) &dataSend, sizeof(dataSend));
  delay(3777);
}

// Obtener la resistencia promedio en N muestras
float readMQ(int mq_pin)
{
   float rs = 0;
   for (int i = 0;i<READ_SAMPLE_TIMES;i++) {
      rs += getMQResistance(analogRead(mq_pin));
      delay(READ_SAMPLE_INTERVAL);
   }
   return rs / READ_SAMPLE_TIMES;
}
// Obtener resistencia a partir de la lectura analogica
float getMQResistance(int raw_adc)
{
   return (((float)RL_VALUE / 1000.0*(1023 - raw_adc) / raw_adc));
}
// Obtener concentracion 10^(coord + scope * log (rs/r0)
float getConcentration(float rs_ro_ratio)
{
   return pow(10, coord + scope * log(rs_ro_ratio));
}
