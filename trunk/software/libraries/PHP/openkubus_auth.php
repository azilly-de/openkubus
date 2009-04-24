<?php

function openkubus_auth($user, $crypted) {
  if(!isset($user) || !isset($crypted))
	{
    return 0;
	}
 
	$host    = "localhost";
	$port    = 11211;
	$timeout = 3;

	$fp = fsockopen($host, $port, $errno, $errstr, $timeout);
	if(!$fp) {
    return 0;
	} else {
		fputs($fp, "$user $crypted\r\n");
		$resp = fgets($fp, 64);

		if(chop($resp) == "ok") {
      fclose($fp);
			return 1;
		} else {
      fclose($fp);
      return 0;
		}
	}
}

?>
