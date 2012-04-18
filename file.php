<?php
if (!($fp = fopen('datafile_php', 'w'))) {
  return;
}
$atmopres = (double)$_POST['atmopres'];
$atmotemp = (double)$_POST['atmotemp'];
$rpm = (double)$_POST['rpm'];
$compratio = (double)$_POST['compratio'];
$ncycl = (int)$_POST['ncycl'];
$gamma = (double)$_POST['gamma'];
$tw = (double)$_POST['tw'];
$B = (double)$_POST['B'];
$L = (double)$_POST['L'];
$R = (double)$_POST['R'];
$Ui = (double)$_POST['Ui'];
$Bv = (double)$_POST['Bv'];
$Liv = (double)$_POST['Liv'];
$a1 = (double)$_POST['a1'];
$a2 = (double)$_POST['a2'];
if( !(isset($atmopres) &&
      isset($atmotemp) &&
      isset($rpm) &&
      isset($compratio) &&
      isset($ncycl) &&
      isset($gamma) &&
      isset($tw) &&
      isset($B) &&
      isset($L) && 
      isset($R) &&
      isset($Ui) &&
      isset($Bv) &&
      isset($Liv) &&
      isset($a1) &&
      isset($a2) ) 
  )
{

echo "Some parameters were not set... <br />Reverting to default parameter values values which are : <br / >";

$atmopres = 1;
$atmotemp = 15;
$rpm = 3000;
$compratio = 10;
$ncycl = 4;
$gamma = 1.4;
$tw = 200;
$B = 0.08;
$L = 0.1;
$R = 0.3;
$Ui = 60;
$Bv = 0.032;
$Liv = 0.0065;
$a1 = 330;
$a2 = 370;
echo "$atmopres, $atmotemp, $rpm, $compratio, $ncycl, $gamma, $tw, $B, $L, $R, $Ui, $Bv, $Liv, $a1, $a2 <br />";
}
fprintf($fp,"%lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",$atmotemp,$atmopres,$rpm,$compratio,$ncycl,$gamma,$tw,$B,$L,$R,$Ui,$Bv,$Liv,$a1,$a2);
fclose($fp);
system("./phprun > /dev/null");
?>
<head>
<style type="text/css">
iframe {
float:left;
margin:20px;
}
</style>
<title> Results </title>
</head>
<html>
<body>
<iframe src="results1.dat" width="30%" height="80%" align="left">
</iframe>
<iframe src="results2.dat" width="30%" height="80%" align="right">
</iframe>


</body>
</html>


