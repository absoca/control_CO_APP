WebServer server(80);

////////////////////FUNCIONES/////////////////////////
  //Función para contar palabras de un String dada una palabra
    int contarPalabras (String _palabra, String _cadena){
      int encontradas=0;
      for (uint8_t i = 0; i < _cadena.length() ; i++) {
        if(_cadena.substring(i, _palabra.length()+i).equals(_palabra))
          encontradas++;    
      }
      return encontradas;
    }

  //Función para contar almacenar los datos en el json
    void almacenarDatos(String _datos){
      if(dispEncontrados != 0){
        if((_datos.length() < LON_CADENA)){
          //Creamos los documentos JSON
          DynamicJsonDocument jsonAlmacenado(capacidadJSON);
          DynamicJsonDocument jsonRecibido(LON_STRING);
          
          DeserializationError errorAlmacenado = deserializeJson(jsonAlmacenado, dataSensores);
          
          if (!errorAlmacenado) {
            DeserializationError errorRecibido = deserializeJson(jsonRecibido, _datos);
            if (!errorRecibido) {
              //Serial.println(_datos);
              const char* MAC2=jsonRecibido["MAC"];
              for (uint8_t i = 0; i < dispEncontrados ; i++) {
                const char* MAC1=jsonAlmacenado[i]["MAC"];
                if (strcmp(MAC1, MAC2) == 0){
                  //Actualizamos el valor del sensor recibido
                  jsonAlmacenado[i]["data"] = jsonRecibido["data"];
  
                  //Indicamos que el valor se ha actualizado
                  jsonAlmacenado[i]["update"] = millis()/1000;
                }
              }
            }
          }
          //Borramos el string para crear el nuevo
          dataSensores="";
          serializeJson(jsonAlmacenado, dataSensores);
          Serial.println(dataSensores);
        }
      }
    }

////////////////////MÉTODO GET/////////////////////////
  // Funcion que se ejecutara en la URI '/'
    void handleRoot() 
    {
       server.send(200, "text/plain", "Bienvenido al control de gas");
    }
     
  // Funcion que se ejecutara en URI desconocida
    void handleNotFound() 
    {
       server.send(404, "text/plain", "Not found");
    }
    
  // Funcion que se ejecutara en URI desconocida
    void handleGetValues() 
    {
      
      //Creamos el documento JSON
      DynamicJsonDocument doc(capacidadJSON);
      
      DeserializationError error = deserializeJson(doc, dataSensores);
      //Comprobamos si no tenemos ningún error con el formato del JSON
      if (error) {
        server.send (500, "text/json", "[{\"type_error\": 1,\"cod_error\":" + (String)error.code() + ",\"desc_error\":\""+ (String)error.c_str() +"\"}]");
      }
      else
        server.send (200, "text/json", dataSensores);      
    }

////////////////////MÉTODO POST/////////////////////////
  // Funcion que se ejecutara cuando recibamos las MAC a configurar
    void handleSetMAC() 
    {  
      //Buscamos cuantos sensores ha introducido el usuario
      dispEncontrados = contarPalabras(MASCARA, server.arg(0));

      //Calculamos la capacidad que debe tener nuestro objeto del JSON
      //capacidadJSON = JSON_ARRAY_SIZE(dispEncontrados) + dispEncontrados*JSON_OBJECT_SIZE(4)+ LON_SENSOR + LON_NOMBRE + LON_MAC + LON_DATA + (LON_CAMPO_NOM + LON_CAMPO_MAC)*dispEncontrados;
      capacidadJSON = JSON_ARRAY_SIZE(dispEncontrados) + dispEncontrados*JSON_OBJECT_SIZE(5)+ LON_SENSOR + LON_NOMBRE + LON_MAC + LON_DATA + LON_UPDATE + (LON_CAMPO_NOM + LON_CAMPO_MAC)*dispEncontrados;
      
      //Almacenamos en un string todo el json que viene en la petición
      dataSensores = server.arg(0); 
        
      // devolver respuesta
      server.send(200, "text/plain", "Success");
    }

////////////////////INICIALIZACIÓN/////////////////////////
    void initServer()
    {
      dispEncontrados=0;
    ////////////////////RUTEO GENÉRICO/////////////////////////
      // Ruteo para '/'
      server.on("/", handleRoot);
      
      // Ruteo para URI desconocida
      server.onNotFound(handleNotFound);
       
    ////////////////////RUTEO GET/////////////////////////
      server.on("/values", HTTP_GET, handleGetValues);
    
    ////////////////////RUTEO POST/////////////////////////
      // Ruteo para recibir las MAC por POST
      server.on("/mac", HTTP_POST, handleSetMAC);
    
    ////////////////////INICIO SERVER/////////////////////////
     server.begin();
     Serial.println("HTTP server started");
    }

//String** disp_ES8266; //Array de MACs para enlazar todas las ES8266 con ESP-NOW  
  /////////////USANDO ARRAYS DE STRINGS///////////////////////////
  /*//Creamos el array en función de los dispositivos encontrados
  disp_ES8266 = new String*[dispEncontrados];
  uint8_t j = 0;
  
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i).indexOf("MAC") == 0){
      disp_ES8266[j] = new String[longitudMAC];
      *disp_ES8266[j] = server.arg(i); //Guardamos en el array las MAC introducidas por el usuario
      j++;
    }
  }*/
