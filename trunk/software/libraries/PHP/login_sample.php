<?php
/*
Quick and dirty: This example file shows how to use OpenKubus in
web-applications.
*/
include("openkubus_auth.php");

$auth_failed = 0;

session_start();

if(isset($_GET["logout"]))
{
  session_unset();
}

if(!isset($_SESSION["time"]))
{
  if(isset($_POST["submitit"]) && isset($_POST["user"]) && isset($_POST["openkubus_pad"])) {
    if(openkubus_auth($_POST["user"], $_POST["openkubus_pad"])) {
      $_SESSION["time"] = time();
      header("Location: /mediawiki/index.php/Hauptseite");
      exit;
    }
    else {
      $auth_failed = 1;
      session_unset();
    }
  }
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=charset*" />
    <title>Wiki - login</title>
  </head>

  <body bgcolor="#FFFFFF">

    <table bgcolor="#000000" border="0" cellpadding="0" cellspacing="0" width="40%" align="center">
     <tr>
      <td>
       <table border="0" width="100%" bgcolor="#486591" cellpadding="2" cellspacing="1">
  <tr bgcolor="#486591">
   <td align="left" valign="middle">
    <font color="#FEFEFE">&nbsp;</font>
   </td>

  </tr>
  <tr bgcolor="#e6e6e6">
   <td valign="baseline">

        <form name="login" method="post" action="login.php" autocomplete="off">
        <input type="hidden" name="passwd_type" value="text" />
          <table border="0" align="center" bgcolor="#486591" width="100%" cellpadding="0" cellspacing="0">
            <tr bgcolor="#e6e6e6">
              <td colspan="2" align="center"><?php if($auth_failed) { echo "Benutzername/Passwort falsch"; } else { echo "&nbsp;"; }?></td>

            </tr>
            <tr bgcolor="#e6e6e6">
              <td align="right"><font color="#000000">Benutzername:&nbsp;</font></td>
              <td><input name="user" value="" /></td>
            </tr>
<!--            <tr bgcolor="#e6e6e6">
              <td align="right"><font color="#000000">Passwort:&nbsp;</font></td>
              <td><input name="passwd" type="password" /></td>
            </tr>-->
<!-- NOT -->
            <tr bgcolor="#e6e6e6">
              <td align="right"><font color="#000000">Stick:&nbsp;</font></td>
              <td><input name="openkubus_pad" type="password" /></td>
            </tr>
<!-- NOT -->
            <tr bgcolor="#e6e6e6">
              <td colspan="2" align="center"><input type="submit" value="einloggen" name="submitit" /></td>
            </tr>
          </table>
        </form>
   
   </td>
  </tr>
       </table>
      </td>

     </tr>
    </table>
  </body>
</html>

<?php
exit;
}
?>
