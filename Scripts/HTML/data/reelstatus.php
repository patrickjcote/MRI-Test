<?php


$dreel_depth = shell_exec("sudo python data-reel.py cd 0 &");
$dreel_status = shell_exec("sudo python data-reel.py q 0 &");

switch($dreel_status){
case 100:
        $dreel_status = "Ready";
        break;
case 001:
        $dreel_status = "Reeling Up";
        break;
case 002:
        $dreel_status = "Reeling Down";
        break;
case 104:
        $dreel_status = "Emergency Stop";
        break;
case 110:
case 120:
case 115:
case 125:
        $dreel_status = "Limit Switch Engaged   (".$dreel_status.")";
        break;

default:
        $dreel_status = "Error ".$dreel_status;
        break;
}

?>
        <strong> Reel Control</strong><br>
        Reel Status: <?php echo $dreel_status;?><br>
        Current Depth: <?php echo $dreel_depth;?><br>
