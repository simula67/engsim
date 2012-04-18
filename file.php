<?php
if (!($fp = fopen('datafile', 'w'))) {
  return;
}

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

fprintf($fp,"%lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",$atmotemp,$atmopres,$rpm,$compratio,$ncycl,$gamma,$tw,$B,$L,$R,$Ui,$Bv,$Liv,$a1,$a2);
fclose($fp);

?>
<head>
<title> Results </title>
</head>
<html>
<body>
This is not real results
<iframe src="results1.dat">
</iframe>
<iframe src="results2.dat">
</iframe>


</body>
</html>


