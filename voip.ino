#include <DHT11.h>
#include <SoftwareSerial.h>

  SoftwareSerial WIFI1(4, 3); // RX | TX
  const String NOMBRE_WIFI="66138025";
  const String CLAVE_WIFI="8988FAF626593";
  const String PUERTO="80";
  const boolean DEBUG=true;
  const int PIN_SENSOR=5;
  DHT11 dht11(PIN_SENSOR);

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
        sendData(ORDENES[i],7000,true);
        Serial.println("....................");
      }

      pinMode(13,OUTPUT);
      digitalWrite(13,LOW);
  }

  void loop() {
    if(WIFI1.available()){
      if(WIFI1.find("+IPD,")){
        delay(100);
        int connectionId = WIFI1.read()-48;

        WIFI1.find("opc=");// se coloca el apuntador luego de pin=
        int opc = (WIFI1.read()-48); // el valor devuelto esta en ascci por tanto se debe de restar 48, pero esto devuelve el valor de 1 asi que se multiplica por 10
        Serial.println("opc: "+String(opc));
        serverRequest(opc,connectionId);
      }
    }
  }

  String sendData(String command, const int timeout, boolean debug){
      String response = "";
      WIFI1.println(command);
      long int time = millis();
      while( (time+timeout) > millis()){ // se  espera el tiempo indicado en el parametro timeput
        while(WIFI1.available()){ // Se verifica si se respondio el comando
          char c = WIFI1.read();  // se lee la respuesta
          response+=c;
        }
      }
      if(debug){
        Serial.print(response); // se imprime la respuesta
      }
      return response;
  }

  void http(String output,String id){
    WIFI1.print("AT+CIPSEND="+id+",");// AT+CIPSEND=0, num
    WIFI1.println(output.length());
    if (WIFI1.find(">")){  // Si recibimos la peticion del mensaje
      WIFI1.println(output);
      WIFI1.println(output); //Aqui va el string hacia el WIFI
      delay(10);
      while ( WIFI1.available() > 0 ) {
        if (  WIFI1.find("SEND OK") ) // Busca el OK en la respuesta del wifi
                break;
      }
    }
  }

  void serverRequest(int opc, int id){
     String strid=String(id);
     http("<!DOCTYPE HTML>",strid);
     http("<html>",strid);
     switch (opc){
       case 1:
         ledRequest(strid);
       break;
       case 2:
         temperaturaRequest(strid);
       break;
       case 3:
         humedadRequest(strid);
       break;
       default:
       break;
     }
     WIFI1.println("AT+CIPCLOSE="+strid);
  }

  void ledRequest(String strid){
    digitalWrite(13, !digitalRead(13));
     if(digitalRead(13)==HIGH){
       Serial.println("1");
       http("~1~",strid);
     }
     else{
       Serial.println("0");
       http("~0~",strid);
     }
  }

  void temperaturaRequest(String strid){
       int err;
       float temp, hum;
       if((err = dht11.read(hum, temp)) == 0)    // Si devuelve 0 es que ha leido bien
          {
             String temp_str=String((int)temp, (unsigned char)DEC);
             http("~"+temp_str+"~",strid);
          }
       else
          {
             http("~-1~",strid);
          }
    }

    void humedadRequest(String strid){
       int err;
       float temp, hum;
       if((err = dht11.read(hum, temp)) == 0)    // Si devuelve 0 es que ha leido bien
          {
             String hum_str=String((int)hum, (unsigned char)DEC);
             http("~"+String(hum_str)+"~",strid);
          }
       else
          {
             http("~-1~",strid);
          }
    }
