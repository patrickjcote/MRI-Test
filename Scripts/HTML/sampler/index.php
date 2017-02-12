 <html>

<head>
<title>MRI - Sampler Control</title>
<script src="jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#pumpstatus").load("pumpstatus.php");
        }, 1002);
        setInterval(function() {
                $("#connection").load("connection.php");
        }, 5005);
});

</script>

<style>
body{
    font-size:150%;
}

div{
    padding-bottom:20px;
}
</style>

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
usleep(50000);
$bottle = shell_exec("sudo python sampler.py cd 0 &");
usleep(50000);
$status = shell_exec("sudo python sampler.py q 0 &");
usleep(50000);
$sampler_output = shell_exec("sudo python sampler.py $cmd $value &");
?>
<body>
<center>
<div id="header">
        <h1><underline>Sampler Control</underline></h1>
       <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="s" />
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="all" />
            <input type="submit" value="EMERGENCY STOP" class="button"/>
        </form>
</div>
<div id="control">
    <div id="pumpcontrol">
        <div id="pumpstatus" >
        <?php include('pumpstatus.php');?>
        </div>
        <div id="valve-control">
            <form action="index.php" method="post">
                <input type="hidden" name="cmd" value="BR" />
                <input type="hidden" name="value" value="0" />
                <input type="submit" value="Reset Arm" />
            </form>
            <form action="index.php" method="post">
                Go to Bottle #: <input type="text" name="value" />
                <input type="hidden" name="cmd" value="bp" />
                <input type="submit" value="Go" />
            </form>
            <form action="index.php" method="post">
                Pump # of mililiters: <input type="text" name="value"><br>
                <input type="radio" name="cmd" value="pf" checked="checked" >Sample
                <input type="radio" name="cmd" value="pr">Purge<br>
                <input type="submit" value="Pump" />
            </form>
        </div>
    </div>
</div>
    <br>
<div id="connection"><?php include('connection.php');?></div>
<div id="footer">
    <a href="/data/" target="_blank">View Data Sonde Page</a>
    <a href="/hose/" target="_blank">View Hose Reel Page</a>
</div>
</center>        
</body>

</html>
