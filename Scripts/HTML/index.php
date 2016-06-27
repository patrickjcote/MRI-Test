<html>

<head>
<title>MRI - Pit Boat Control</title>
<script src="jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#status").load("status.php");
        }, 1000);
});

</script>

<style>

padding:10px;

</style>

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$board = $_POST["board"];
$current_depth = shell_exec("sudo python hose-reel.py cd 0 &");
$current_status = shell_exec("sudo python hose-reel.py q 0 &");
if($board == "hose"){
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
}
if($board == "valve"){
        $valve_output = shell_exec("sudo python valve.py $cmd $value &");
}
if($board == "all"){
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
        $valve_output = shell_exec("sudo python valve.py $cmd $value &");
}
?>


<body>
<center>
        <h1><underline>Hose and Pump Control</underline></h1>
       <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="s" />
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="all" />
            <input type="submit" value="EMERGENCY STOP" class="button"/>
        </form>
<br><br>
        <div id="status" style="float:left">
        <?php include('status.php');?>
        </div>
        <div id="reel-control" style="float:left">
        <form action="index.php" method="post">
            <input type="radio" name="cmd" value="LU" />Up<br>
            <input type="radio" name="cmd" value="LD" />Down<br>
            <input type="radio" name="cmd" value="LL" />Level<br>
            <input type="radio" name="cmd" value="LA" />Auto-level<br>
            <input type="radio" name="cmd" value="LS" />Stop<br>
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="hose" />
            <input type="submit" value="Set Reel Angle" />
        </form>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="pu" />
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="hose" />
            <input type="submit" value="Pull Up Reel" />
        </form>
        </div>
        <br><br>
        <div id="valve-control" style="float:left">
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="v" />
            <input type="hidden" name="value" value="S" />
            <input type="hidden" name="board" value="valve" />
            <input type="submit" value="Valve to Sample Position" />
        </form>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="v" />
            <input type="hidden" name="value" value="P" />
            <input type="hidden" name="board" value="valve" />
            <input type="submit" value="Valve to Purge Position"/>
        </form>
        <form action="index.php" method="post">
                Purge Time: <input type="text" name="value" value="<?php if($board == "valve")echo $value;?>"/>
                <input type="hidden" name="cmd" value="p" />
                <input type="hidden" name="board" value="valve" />
                <input type="submit" value="Purge Time" />
        </form>
</div>
<br><br>        
<form action="index.php" method="post">
        Set Depth: <input type="text" name="value" value="<?php if($board == "hose") echo $value;?>"/>
                <input type="hidden" name="cmd" value="rd" />
                <input type="hidden" name="board" value="hose" />
                <input type="submit" value="Set Depth" />
        </form>
        <br><br>
        <div id="connection"><?php include('connection.php');?></div>
</center>        
</body>

</html>
