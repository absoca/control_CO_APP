#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <string.h>
#include <stdlib.h>

//Definiciones para la estructura JSON
#define MASCARA "\"MAC\""
#define LON_SENSOR 7
#define LON_NOMBRE 7
#define LON_MAC 4
#define LON_DATA 5
#define LON_UPDATE 7
#define LON_CAMPO_NOM 16
#define LON_CAMPO_MAC 18

//Definiciones para la estructura JSON del String recibido
#define LON_STRING 96

//Definiciones para la cadena a procesar
#define LON_CADENA 40

//Variable para almacenar la capacidad del json en función de las MAC introducidas por el usuario
size_t capacidadJSON;
int dispEncontrados;

//String que almacena todos los datos de los sensores
String dataSensores; 

// Set these to your desired credentials.
const char *ssid = "ESP32-Sensor";
const char *password = "123456789";

#include "Server.hpp"
#include "Comunicacion.hpp"

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.println("\nConfigurando punto de acceso...");

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Dirección IP del AP: ");
  Serial.println(myIP);
  
  initServer();
  initComunicacion();
}

void loop() {
  server.handleClient();
  almacenarDatos(leerSerial());
  delay(888);
}
