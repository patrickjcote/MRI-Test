<?php


$current_pump = shell_exec("sudo python valve.py t 0 &");
$current_valve = shell_exec("sudo python valve.py q 0 &");

if($current_valve[2] == "1")
        $valve = "Ready to Sample";
else
        $valve = "Ready to Purge";

$time_m = intval(floor(intval($current_pump)/60));
$time_s = intval($current_pump) % 60;

?>
        <strong>Pump/Valve Control</strong><br>
        Pump Time Remaining: <?php echo sprintf("%02d:%02d", $time_m, $time_s);?><br>
        Valve Status: <?php echo $valve;?><br>
