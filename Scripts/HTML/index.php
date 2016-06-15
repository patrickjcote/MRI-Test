<html>


<head>
</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$output = shell_exec("sudo python reel-input.py $cmd $value &");
$current_depth = shell_exec("sudo python reel-input.py cd 0 &");
$current_status = shell_exec("sudo python reel-input.py q 0 &");

?>


<body>


        <h1><underline>Hose and Reel Control</underline></h1>
        <form action="index.php" method="post">
            <input type="hidden" name="cmd" value="s" />
            <input type="hidden" name="value" value="0" />
            <input type="submit" value="EMERGENCY STOP" />
        </form>
        <br><br>
        <form action="index.php" method="post">
            <input type="submit" value="Refresh Status">
        </form>

        Current Depth: <?php echo $current_depth; ?><br>
        Current Status: <?php echo $current_status; ?><br>
        <br>
        <br>

        <form action="index.php" method="post">
                Set Depth: <input type="text" name="value" />
                <input type="hidden" name="cmd" value="rd" />
                <input type="submit" value="Set Depth" />
        </form>

</body>

</html>
