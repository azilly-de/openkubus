<?php

define('BLOCKSIZE', 16);
define('KEYSIZE',   32);
define('DATASIZE',  14);
define('MAXPAD',    33);

function openkubus_auth($pad, $pw, $offset, $num) {
  if(!isset($pad) or !isset($pw) or !isset($offset) or !isset($num)) {
    return -1;
  }

  $key  = substr($pw, 0, KEYSIZE);
  $data = substr($pw, KEYSIZE, DATASIZE);

  # fix base64
  rtrim($pad);
  for($i = 0; $i < strlen($pad); $i++) {
    if    ($pad[$i] == " ") { $pad[$i] = "/"; }
    elseif($pad[$i] == ".") { $pad[$i] = "="; }
    elseif($pad[$i] == "-") { $pad[$i] = "+"; }
  }

  $z = $pad[0];
  $crypted = substr($pad, 1);

  print "$z | $crypted\n";

  if($z == "y" or $z == "Y") {
    for($i = 0; $i < strlen($crypted); $i++) {
      if    ($crypted[$i] == 'y') { $crypted[$i] = "z"; }
      elseif($crypted[$i] == 'Y') { $crypted[$i] = "Z"; }
      elseif($crypted[$i] == 'z') { $crypted[$i] = "y"; }
      elseif($crypted[$i] == 'Z') { $crypted[$i] = "Y"; }
    }
  }

  print "$crypted\n";

  if($z == "Y" or $z == "Z") {
    for($i = 0; $i < strlen($crypted); $i++) {
      if(ctype_upper($crypted[$i])) {
        $crypted[$i] = strtolower($crypted[$i]);
      } else {
        $crypted[$i] = strtoupper($crypted[$i]);
      }
    }
  }

  print "$crypted\n";

  $crypted = base64_decode($crypted);

  /* XXX
  $td = mcrypt_module_open ("rijndael-256", "", "ecb", "");
  mcrypt_generic_init($td, $key, 0);
  $plain = mdecrypt_generic ($td, $crypted);
  mcrypt_generic_end ($td); 
  */

  $i     = substr($plain, 0,1);
  $j     = substr($plain, 1,1);
  $plain = substr($plain, 2, strlen($plain)-2);

  $n = ord($i) + (ord($j) << 8);

  if(($plain == $data) and ($n > $num)) {
    return $n;
  } else {
    return -4;
  }
}

?>
