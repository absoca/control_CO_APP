//ESP32 PROGRAM
#include <string.h>
#include <stdlib.h>

//const int D12 = 12;     // Receptor de interrupciones
//const int D14 = 14;     // Emisor de interrupciones
//const char separador = ',';

String cadena;
boolean tramaCompleta;
boolean inicioTrama;
boolean finTrama;
char inByte;

void initComunicacion() {
  //pinMode(D12, INPUT);
  //pinMode(D14, OUTPUT);
  //attachInterrupt(D12, recibirInterrupcion, RISING); //LOW/HIGH/FALLING/RISING/CHANGE
  
  tramaCompleta=false;
  inicioTrama=false;
  finTrama=false;  
  cadena="";
}

String leerSerial(){
  //Inicializamos variables al invocar a la función leerSerial()
  tramaCompleta=false;
  inicioTrama=false;
  finTrama=false;
  if (Serial2.available()) {
    while(!tramaCompleta){
      inByte = Serial2.read();
      if (inByte =='{'){
        cadena=""; //Recibimos el caracter de inicio de trama y borramos el string
        tramaCompleta=false;
        inicioTrama=true;
        finTrama=false;
      } else if (inByte =='}'){
        finTrama=true;
        if (inicioTrama){
          tramaCompleta=true;
          }
        else
          finTrama=false;
      }
      if(inicioTrama){      
        //Comprobamos si la cadena obtenida tiene mayor longitud de la esperada
        if (cadena.length() > LON_CADENA){
          inicioTrama=false;
          finTrama=false;
          tramaCompleta=false;
          break;
        } else
          cadena+=String(inByte); //Construyo la cadena caracter a caracter concatenándolos
      }
    }
    return cadena;
  }
}
