<?php

$current_depth = shell_exec("sudo python hose-reel.py cd 0 &");
$current_status = shell_exec("sudo python hose-reel.py q 0 &");
$current_pump = shell_exec("sudo python valve.py t 0 &");
$current_valve = shell_exec("sudo python valve.py q 0 &");

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

switch($current_valve){

case 0x1:
        $valve = "Sampler Line";
        break;
default:
        $valve = "Purge Line";
        break;
}

?>
        Current Depth: <?php echo $current_depth;?><br>
        Reel Status: <?php echo $status;?><br>
        Pump Time Remaining: <?php echo $current_pump;?><br>
        Valve Status: <?php echo $valve;?><br>

