<html>

<head>
<title>MRI - Hose/Valve/Pump Control</title>
<script src="jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#reelstatus").load("reelstatus.php");
        }, 1111);
        setInterval(function() {
                $("#pumpstatus").load("pumpstatus.php");
        }, 1222);
        setInterval(function() {
                $("#anglestatus").load("anglestatus.php");
        }, 1333);
        setInterval(function() {
                $("#connection").load("connection.php");
        }, 1555);
});

</script>

<style>
body{
    font-size:150%;
}

div{
    padding-bottom:20px;
}
.radioLeft {
    text-align:left;
    display:inline-block;
}
#pumpcontrol{
    float:left;
    width:33%;
}
#anglecontrol{
    float:left;
    width:33%;
}
#reelcontrol{
    float:left;
    width:33%;
}
#control{
    clear:both;
    width:100%;
}
#connection{
    clear:both;
    width:60%;
}
#footer{
    clear:both;
}
</style>

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$board = $_POST["board"];
$current_depth = shell_exec("sudo python hose-reel.py cd 0 &");
$current_status = shell_exec("sudo python hose-reel.py q 0 &");
$current_valve = shell_exec("sudo python valve.py q 0 &");
if($board == "hose"){
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
}
if($board == "prime"){
        $prime_output = shell_exec("sudo python prime.py $cmd $value &");
}
if($board == "valve"){
        $valve_output = shell_exec("sudo python valve.py $cmd $value &");
}
if($board == "all"){
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
        $valve_output = shell_exec("sudo python valve.py $cmd $value &");
        $prime_output = shell_exec("sudo python prime.py $cmd $value &");
}
?>


<body>
<center>
<div id="header">
        <h1><underline>Hose and Pump Control</underline></h1>
       <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="s" />
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="all" />
            <input type="submit" value="EMERGENCY STOP" class="button"/>
        </form>
</div>
<div id="control">
    <div id="reelcontrol"> 
        <div id="reelstatus" >
        <?php include('reelstatus.php');?>
        </div>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="pu" />
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="hose" />
            <input type="submit" value="Pull Up Reel" />
        </form>
        <div id="setdepth">
            <form action="index.php" method="post">
                Set Depth: <input type="text" name="value" value="<?php if($board == "hose") echo $value;?>"/>
                <input type="hidden" name="cmd" value="rd" />
                <input type="hidden" name="board" value="hose" />
                <input type="submit" value="Set" />
            </form>
        </div>
    </div>
    <div id="anglecontrol">
        <div id="anglestatus">
        <?php include('anglestatus.php');?>
        </div>
        <form action="index.php" method="post">
            <div class="radioLeft">
            <input type="radio" name="cmd" value="LU" />Up<br>
            <input type="radio" name="cmd" value="LD" />Down<br>
            <input type="radio" name="cmd" value="LL" />Level<br>
            <input type="radio" name="cmd" value="LA" />Auto-level<br>
            <input type="radio" name="cmd" value="LT" />Travel Mode<br>
            <input type="radio" name="cmd" value="LS" />Stop<br>
            </div><br>
            <input type="hidden" name="value" value="0" />
            <input type="hidden" name="board" value="hose" />
            <input type="submit" value="Set Reel Angle" />
        </form>
    </div>
    <div id="pumpcontrol">
        <div id="pumpstatus" >
        <?php include('pumpstatus.php');?>
        </div>
        <div id="valve-control">
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
                Purge Time: <input type="text" name="value"\
                 <?php if($valveFlag) {echo 'disabled value="Check Valve Status"';}?>/>
                <input type="hidden" name="cmd" value="p" />
                <input type="hidden" name="board" value="valve" />
                <input type="submit" value="Purge"<?php if($valveFlag) {echo 'disabled';}?> />
            </form>
            <form action="index.php" method="post">
                Prime Time: <input type="text" name="value" />
                <input type="hidden" name="cmd" value="p" />
                <input type="hidden" name="board" value="prime" />
                <input type="submit" value="Prime" />
            </form>
        </div>
    </div>
</div>
    <br>
<div id="connection"><?php include('connection.php');?></div>
<div id="footer">
    <a href="/data/" target="_blank">View Data Sonde Page</a>
</div>
</center>        
</body>

</html>
