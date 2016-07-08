<html>

<head>
<title>MRI - Data Sonde Control</title>
<script src="jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#reelstatus").load("reelstatus.php");
        }, 1000);
        setInterval(function() {
                $("#graph").load("graph.php");
        }, 10500);
});

</script>

<style>
body{
    font-size:150%;
}
div{
    padding-bottom:20px;
}
img{ 
    display:block;
    margin: 0px;
}
#footer``:w
#{
    clear:both;
}
#connection{
    clear:both;
}
</style>

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$current_depth = shell_exec("sudo python data-reel.py cd 0 &");
$current_status = shell_exec("sudo python data-reel.py q 0 &");
if($board == "data"){
        $hose_output = shell_exec("sudo python data-reel.py $cmd $value &");
}
?>


<body>
<center>
        <h1>Data Sonde Control</h1>
<div id="container" style="width:100%; clear:both;">
    <div id="data" style="width:50%; float:left; display:inline-block;">
       <div id="graph" >
            <?php include("graph.php");?> 
       </div>

    <iframe src="getdata.php" frameborder="0" scrolling="no"></iframe><br>
    </div>
    <div id="control" style="float:left; display:inline-block; width:50%;">
        <br><br><br>
        <div id="header">
            <form action="index.php" method="post">
                <input type="hidden" name="cmd" value="s" />
                <input type="hidden" name="value" value="0" />
                <input type="hidden" name="board" value="all" />
                <input type="submit" value="EMERGENCY STOP" class="button"/>
            </form>
        </div>
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
        <div id="connection"><?php include('connection.php');?></div>
    </div>
</div>

<br>

</center>        
</body>

</html>
