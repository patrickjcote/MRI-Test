<?php

$depth = $_GET["depth"];

echo "Input variable: ".$depth."<br><br>";

$output = exec("python ../Python/input.py $depth");

?>

<html>

<body>
<?php
echo $output."<br>";
var_dump($output);

?>
</html>
