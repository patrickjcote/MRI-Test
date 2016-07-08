<html>

<head>
<?php
$date = date('Ymd.Hi').'.txt';
$cmd = $_POST["cmd"];
if($cmd == 'getdata'){
        exec("sudo python readsonde.py $date");
}
else{
        exec("sudo python stopread.py &");
}
if($cmd == "clear"){
        exec("sudo rm -f current.txt");
        exec("sudo rm -f currentdepth.jpg");
}
?>
<style>
body{
    font-size:150%;
}
form{
    display:inline-block;
}
a{
    clear:both;
}
</style>
</head>

<body>
<center>
<div style="clear:both">
<form action="getdata.php" method="post">
        <input type="hidden" name="cmd" value="getdata">
        <input type="submit" value="Start recording data (<?php echo $date;?>)">
</form>
        <form action="getdata.php" method="post">
        <input type="hidden" name="cmd" value="stopdata">
        <input type="submit" value="Stop recording data">
</form>
        <form action="getdata.php" method="post">
        <input type="hidden" name="cmd" value="clear">
        <input type="submit" value="Clear current data">
</form>
</div>
</center>
</html>
