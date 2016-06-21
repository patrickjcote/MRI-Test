<?php

$hose_reel = shell_exec("sudo python hose-reel.py i 0 &");
$pump = shell_exec("sudo python valve.py i 0 &");
?>
        Hose Reel Board : <?php echo $hose_reel;?><br>
        Pump/Valve Board : <?php echo $pump;?><br>
        Data Reel Board : <?php echo $data_reel;?><br>
        Sampler Board : <?php echo $sampler;?><br>
        <br><br><br>
        <a href="index.php">Back to Controls Page</a>
