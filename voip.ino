  #include <SoftwareSerial.h>
  
  SoftwareSerial WIFI1(4, 3); // RX | TX
  const String NOMBRE_WIFI="66138025";
  const String CLAVE_WIFI="8988FAF626593";
  const String PUERTO="80";
  const boolean DEBUG=true;
  
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
        sendData(ORDENES[i],1000,true);
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
        
        WIFI1.find("pin=");// se coloca el apuntador luego de pin=
        int pinNumber = (WIFI1.read()-48)*10; // el valor devuelto esta en ascci por tanto se debe de restar 48, pero esto devuelve el valor de 1 asi que se multiplica por 10
        pinNumber = pinNumber+(WIFI1.read()-48); // se suma el siguiente valor
        if(pinNumber>0){
          digitalWrite(pinNumber, !digitalRead(pinNumber));
          serverRequest(0,connectionId);
        }
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
     if(digitalRead(13)==HIGH){
       Serial.println("1");
       http("~1~",strid);
     }
     else{
       Serial.println("1");
       http("~0~",strid);
     }
     WIFI1.println("AT+CIPCLOSE="+strid);
    }
