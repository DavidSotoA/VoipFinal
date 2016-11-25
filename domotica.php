#!/usr/bin/php -q
<?php
require('phpagi.php');
$agi = new AGI();
$agi->answer();

///////////////////////////////constantes///////////////////////////////////////
$saludo = "Bienvenido";
$despedida = "Gracias por utilizar el sitema de audiorespuesta, hasta pronto";
$error_prompt = "error en la conexion";
$opciones = "Presione uno para luz, dos para saber la temperatura, tres para saber la humedad o cuatro para salir";
$resp_luz_0 = "Luces apagadas";
$resp_luz_1 = "Luces encendidas";
$resp_temp = "La temperatura de la casa es ";
$resp_hum = "La humedad de la casa es ";
$opc_incorrecta = "Opcion incorrecta, intente nuevamente";
$num_opc = 3;
$error = "ERROR:";
$success = "SUCCESS:";
$URL = '192.168.0.24/opc=';

//////////////////////////////funciones/////////////////////////////////////////
class utilities {

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

  public function menu1($num_opc){
    global $agi,$opciones,$opc_incorrecta;
    $this->speak($opciones);
    $_result = $agi->get_data('beep', 5000, 1);
    $opc = $_result['result'];
    if(intval($opc)>$num_opc || $opc===""){
      $this->speak($opc_incorrecta);
      sleep(1);
      $this->menu1($num_opc);
    }else{
      return $opc;
    }
  }

  public function respuestas($opc,$response){
    global $resp_luz_0,$resp_luz_1,$resp_temp,$resp_hum;
    switch ($opc){
      case 1:
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
}

//////////////////////////////programa//////////////////////////////////////////
$testObject = new utilities();
$testObject->speak($saludo);

do{
  $opc=$testObject->menu1(4);
  $response=$testObject->sendGet($opc);
  $testObject->respuestas($opc,$response);
  sleep(1);
}while($opc!=4);

$testObject->speak($despedida);
$agi->hangup();
?>
