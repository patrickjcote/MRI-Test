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

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$output = shell_exec("sudo python reel-input.py $cmd $value &");
$current_depth = shell_exec("sudo python reel-input.py cd 0 &");
$current_status = shell_exec("sudo python reel-input.py q 0 &");

?>


<body>
<center>

        <h1><underline>Hose and Reel Control</underline></h1>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="s" />
            <input type="hidden" name="value" value="0" />
            <input type="submit" value="EMERGENCY STOP" class="button"/>
        </form>
        <br><br>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="pu" />
            <input type="hidden" name="value" value="0" />
            <input type="submit" value="Pull Up Reels" class="button1"/>
        </form>
        <br><br>
        <div id="status">
        <?php include('status.php');?>
        </div>
        <br>
        <br>

        <form action="index.php" method="post">
                Set Depth: <input type="text" name="value" />
                <input type="hidden" name="cmd" value="rd" />
                <input type="submit" value="Set Depth" />
        </form>
</center>
</body>

</html>
