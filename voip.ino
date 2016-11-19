#include <SoftwareSerial.h>

SoftwareSerial WIFI1(4, 3); // RX | TX
const String NOMBRE_WIFI="66138025";
const String CLAVE_WIFI="8988FAF626593";
const String PUERTO="80";

const String ORDENES[]=
  {  "AT+CIOBAUD=9600", //cambiar velocidad de comunicacion
     "AT+CWMODE=3", // activar modo 3
     "AT+CWQAP", // desconectarse de la red previamente configurada
     "AT+CWJAP=\""+NOMBRE_WIFI+"\",\""+CLAVE_WIFI+"\"", // conectarse a la nueva red
     "AT+CIFSR", // asignar direccion ip al modulo wifi
     "AT+CIPMUX=1", // definir el numero de conexiones permitidas
     "AT+CIPSERVER=1,"+PUERTO // definir el puerto
  };

void setup() {
    Serial.begin(9600);
    WIFI1.begin(9600);
    Serial.println("size: "+sizeof(ORDENES));
    for(int i=0;i<7;i++){
      sendData(ORDENES[i],5000,true);
      Serial.println(i);
    }
    
    pinMode(13,OUTPUT);
    digitalWrite(13,LOW);
}

void loop() {
  if(WIFI1.available()){     
    WIFI1.find("pin=");// se coloca el apuntador luego de pin=
    delay(100); // se da un tiempo bara llenar el buffer
    int pinNumber = (WIFI1.read()-48)*10; // el valor devuelto esta en ascci por tanto se debe de restar 48, pero esto devuelve el valor de 1 asi que se multiplica por 10
    pinNumber = pinNumber+(WIFI1.read()-48); // se suma el siguiente valor
    if(pinNumber>0){
      Serial.print("pin: ");
      Serial.println(pinNumber,DEC);
      digitalWrite(pinNumber, !digitalRead(pinNumber));
    }
  }
}

String sendData(String command, const int timeout, boolean debug){
    String response = "";
    WIFI1.println(command); 
    long int time = millis();
    while( (time+timeout) > millis()) // se  espera el tiempo indicado en el parametro timeput
    {
      while(WIFI1.available()) // Se verifica si se respondio el comando
      {  
        char c = WIFI1.read();  // se lee la respuesta
        response+=c;
      }  
    }
    if(debug)
    {
      Serial.print(response); // se imprime la respuesta
    }
    return response;
}
