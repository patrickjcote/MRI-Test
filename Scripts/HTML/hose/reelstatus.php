<?php


usleep(5000);
$set_angle = shell_exec("sudo python hose-reel.py LQS 0 &");
usleep(5000);
$current_angle = shell_exec("sudo python hose-reel.py LQA 0 &");
usleep(5000);
$current_depth = shell_exec("sudo python hose-reel.py cd 0 &");
usleep(5000);
$current_status = shell_exec("sudo python hose-reel.py q 0 &");

switch($current_status){
case 100:
        $status = "Ready";
        break;
case 001:
        $status = "Reeling Up";
        break;
case 002:
        $status = "Reeling Down";
        break;
case 104:
        $status = "Emergency Stop";
        break;
case 110:
case 120:
case 115:
case 125:
        $status = "Limit Switch Engaged   (".$current_status.")";
        break;

default:
        $status = "Error ".$current_status;
        break;
}

?>
        <strong> Reel Depth</strong><br><br>
        Reel Status: <?php echo $status;?><br>
        Current Depth: <?php echo $current_depth;?><br>
