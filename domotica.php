#!/usr/bin/php -q
<?php
/////////////////////////////inicializacion/////////////////////////////////////
require('phpagi.php');
$agi = new AGI();
$agi->answer();
$arduino_ip = "192.168.0.19";
$arduino_port = 5000;
///////////////////////////////constantes///////////////////////////////////////
$saludo="Bienvenido";
$despedida="Gracias por utilizar el sitema de audiorespuesta, hasta pronto";
$error_prompt = "error en la conexion";
$opciones="Presione uno para saber la temperatura, dos para saber la humedad, 3 para cerrar abrir la puerta";
$error="ERROR:";
$success="SUCCESS:";
//////////////////////////////funciones/////////////////////////////////////////
function speak($text){
global $agi;
    if ($ttsengine == 0){
                $agi->text2wav($text);
        }else {
                $agi->swift($text);
        }
}

function sendGet(){
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_URL,
    '192.168.0.27/pin=13'
  );
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
//////////////////////////////programa//////////////////////////////////////////
speak($saludo);
$response=sendGet();
echo "response: ".$response."\n";
/*
$fp = fsockopen($arduino_ip, $arduino_port, $errno, $errstr);

if (!$fp) {
   echo "$error $errstr ($errno)\n";
   speak($error_prompt);
   exit();
}

speak($saludo);

$opc =fgets($fp);
echo $opc;
speak($opciones);

$return = $agi->get_data('beep', 5000, 1);
$keys = $return['result'];
$fwrite=fwrite($fp, "hola servidor :3");
//$fwrite=fwrite($fp, $keys);
if ($fwrite === false) {
  echo $error;
  exit();
}

*/
speak($despedida);


fclose($fp);
$agi->hangup();
?>

