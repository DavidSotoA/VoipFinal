#!/usr/bin/php -q
<?php
require('phpagi.php');
$agi = new AGI();
$agi->answer();

///////////////////////////////constantes///////////////////////////////////////
$usuarios = array("1234","2345","3456");
$saludo = "Bienvenido";
$despedida = "Gracias por utilizar el sitema de audiorespuesta, hasta pronto";
$error_prompt = "error en la conexion";
$opciones = "Presione uno para encenderla luz, dos para saber la temperatura, tres para saber la humedad o cuatro para salir";
$resp_luz_0 = "Luces apagadas";
$resp_luz_1 = "Luces encendidas";
$resp_temp = "La temperatura de la casa es ";
$resp_hum = "La humedad de la casa es ";
$opc_incorrecta = "Opcion incorrecta, intente nuevamente";
$autencicacion = "Por favor ingrese su clave de acceso";
$ladron= "alerta de intruso, adios";
$clave_correcta="clave correcta";
$clave_incorrecta="clave incorrecta";
$num_opc = 3;
$intentos_menu=5;
$error = "ERROR:";
$success = "SUCCESS:";
$URL = '192.168.0.36/opc=';

//////////////////////////////funciones/////////////////////////////////////////
class utilities {

  public function opciones(){
    global $stateLed,$opciones;
    if($stateLed==1){
      return  "Presione uno para apagar las luces, dos para saber la temperatura, tres para saber la humedad o cuatro para salir";
    }else{
      return  "Presione uno para encender las luces, dos para saber la temperatura, tres para saber la humedad o cuatro para salir";
    }
  }

  public function speak($text){
  global $agi;
      if ($ttsengine == 0){
                  $agi->text2wav($text);
          }else {
                  $agi->swift($text);
          }
  }

  public function sendGet($opc){
    global $URL;
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_URL,$URL.$opc);
    $content = curl_exec($ch);

    for($i=0;$i<strlen($content);$i++){
      if($content[$i]=='~'){
        $response="";
        $i++;
        while($content[$i]!='~'){
            $response=$response.$content[$i];
            $i++;
        }
      }
    }
    return $response;
  }

  public function menu1($num_opc,$count){
    global $agi,$opciones,$opc_incorrecta,$intentos_menu;
    $this->speak($this->opciones());
    $_result = $agi->get_data('beep', 5000, 1);
    $opc = $_result['result'];
    if(intval($opc)>$num_opc || $opc===""){
      $this->speak($opc_incorrecta);
      sleep(1);
      $this->menu1($num_opc,$count);
    }else{
      return $opc;
    }
  }

  public function respuestas($opc,$response){
    global $resp_luz_0,$resp_luz_1,$resp_temp,$resp_hum,$stateLed;
    switch ($opc){
      case 1:
        $stateLed=$response;
        if(strcmp($response, "1") == 0){
          $this->speak($resp_luz_1);
        }else if(strcmp($response, "0") == 0){
          $this->speak($resp_luz_0);
        }
      break;
      case 2:
        $this->speak($resp_temp.$response." grados centigrados");
      break;
      case 3:
        $this->speak($resp_hum.$response." porciento");
      break;
      default:
      break;
    }
  }

  public function autenticar(){
    global $agi,$autencicacion,$usuarios;
    $this->speak($autencicacion);
    $_result = $agi->get_data('beep', 5000, 4);
    $opc = $_result['result'];
    foreach ($usuarios as &$usuario) {
      if($opc===$usuario){
        $this->speak("clave correcta");
        return true;
      }
    }
    $this->speak("clave incorrecta");
    return false;
  }
}

//////////////////////////////programa//////////////////////////////////////////
$testObject = new utilities();
$testObject->speak($saludo);

//Autenticar al usuario
$intentos=3;
$existUser=false;
do{
  $intentos=$intentos-1;
  $existUser=$testObject->autenticar();
}while(!$existUser && $intentos>0);

if($intentos==0 && !$existUser){
  $testObject->speak($ladron);
  $agi->hangup();
}
//Obtener estado inicial del led
$stateLed=$testObject->sendGet(4);

//funcionalidad
do{
  $opc=$testObject->menu1(4,0);
  $response=$testObject->sendGet($opc);
  $testObject->respuestas($opc,$response);
  sleep(1);
}while(intval($opc)!=4);

//pico y chao
$testObject->speak($despedida);
$agi->hangup();
?>
