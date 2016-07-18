<?php


$current_prime = shell_exec("sudo python prime.py t 0 &");
$current_pump = shell_exec("sudo python valve.py t 0 &");
$current_valve = shell_exec("sudo python valve.py q 0 &");

if($current_valve[2] == "1"){
        $valve = "Ready to Sample";
        $valveFlag = 1;
}
else if($current_valve[2] == "0"){
        $valve = "Ready to Purge";
        $valveFlag = 0;
}
else{
        $valve = "Error";
        $valveFlag = 1;
}
$time_m = intval(floor(intval($current_pump)/60));
$time_s = intval($current_pump) % 60;
$ptime_m = intval(floor(intval($current_prime)/60));
$ptime_s = intval($current_prime) % 60;

?>
        <strong>Pump/Valve Control</strong><br><br>
        Prime Time Remaining: <?php echo sprintf("%02d:%02d", $ptime_m, $ptime_s);?><br>
        Pump Time Remaining: <?php echo sprintf("%02d:%02d", $time_m, $time_s);?><br>
        Valve Status: <?php echo $valve;?><br>
