<?php

$depth = $_GET["depth"];
$cmd = $_GET["cmd"];
echo "Input depth: ".$depth."<br><br>";

$output = exec("python set-depth.py $depth");

?>

<html>

<body>
<?php
echo $output."<br>";

?>
</html>
