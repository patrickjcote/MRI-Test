<html>

<head>
<title>MRI - Hose/Valve/Pump Control</title>
<script src="jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#reelstatus").load("reelstatus.php");
        }, 2002);
        setInterval(function() {
                $("#pumpstatus").load("pumpstatus.php");
        }, 1000);
        setInterval(function() {
                $("#anglestatus").load("anglestatus.php");
        }, 2303);
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

if($board == "hose"){
        usleep(5000);
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
}
if($board == "prime"){
        usleep(5000);
        $prime_output = shell_exec("sudo python valve2.py $cmd $value &");
}
if($board == "valve"){
        if($cmd == "v"){
                if($value == "P"){
                        usleep(5000);
                        shell_exec("sudo python valve1.py $cmd 'C' &");
                        usleep(5000);
                        shell_exec("sudo python valve2.py $cmd 'C' &");
                }
                if($value == "C"){
                        usleep(5000);
                        shell_exec("sudo python valve1.py $cmd 'C' &");
                        usleep(5000);
                        shell_exec("sudo python valve2.py $cmd 'O' &");
                }
                if($value == "S"){
                        usleep(5000);
                        shell_exec("sudo python valve1.py $cmd 'O' &");
                        usleep(5000);
                        shell_exec("sudo python valve2.py $cmd 'O' &");
                }
        }
        else{
                $valve_output = shell_exec("sudo python valve1.py $cmd $value &");
        }


}
if($board == "all"){
        usleep(5000);
        $hose_output = shell_exec("sudo python hose-reel.py $cmd $value &");
        usleep(5000);
        $valve_output = shell_exec("sudo python valve1.py $cmd $value &");
        usleep(5000);
        $prime_output = shell_exec("sudo python valve2.py $cmd $value &");
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
                Set Depth: <input type="text" name="value" value=""/>
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
                <input type="submit" value="Valves to Sample Position" />
            </form>
            <form action="index.php" method="post">
                <input type="hidden" name="cmd" value="v" />
                <input type="hidden" name="value" value="C" />
                <input type="hidden" name="board" value="valve" />
                <input type="submit" value="Valves to Purge Position"/>
            </form>
            <form action="index.php" method="post">
                <input type="hidden" name="cmd" value="v" />
                <input type="hidden" name="value" value="P" />
                <input type="hidden" name="board" value="valve" />
                <input type="submit" value="Valves to Prime Position"/>
            </form>
            <form action="index.php" method="post">
                Purge Time: <input type="text" name="value"\
                 <?php if($valveFlag != 0) {echo 'disabled value="Check Valve Status"';}?>/>
                <input type="hidden" name="cmd" value="p" />
                <input type="hidden" name="board" value="valve" />
                <input type="submit" value="Purge"<?php if($valveFlag != 0) {echo 'disabled';}?> />
            </form>
            <form action="index.php" method="post">
                Prime Time: <input type="text" name="value"\
                 <?php if($valveFlag != 2) {echo 'disabled value="Check Valve Status"';}?>/>
                <input type="hidden" name="cmd" value="p" />
                <input type="hidden" name="board" value="prime" />
                <input type="submit" value="Prime" <?php if($valveFlag != 2) {echo 'disabled';}?>/>
            </form>
        </div>
    </div>
</div>
    <br>
<div id="connection"><?php include('connection.php');?></div>
<div id="footer">
    <a href="../data/" target="_blank">Data Sonde Page</a>
    <a href="../sampler/" target="_blank">Sampler Page</a>
    <a href="../" target="_blank">Home</a>
</div>
</center>        
</body>

</html>
