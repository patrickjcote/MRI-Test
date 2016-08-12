<?php

usleep(50000);
$current_status = shell_exec("sudo python sampler.py q 0 &");
usleep(50000);
$current_bottle = shell_exec("sudo python sampler.py bn 0 &");

$status_code = $current_status;
switch($current_status[2]){
case 0:
        $current_status = "Ready";
        break;
case 1:
        $current_status = "Pumping Forward";
        break;
case 2:
        $current_status = "Pumping Backward";
        break;
case 3:
        $current_status = "Arm Moving";
        break;
case 4:
        $current_status = "Arm Reseting";
        break;
case 5:
        $current_status = "All Stop";
        break;
default:
        break;
}
?>
        Status: <?php echo $current_status;?> (<?php echo $status_code;?>)<br>
        Current Bottle: <?php echo $current_bottle;?><br>
