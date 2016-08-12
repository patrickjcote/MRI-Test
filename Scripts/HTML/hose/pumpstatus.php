<?php

usleep(5000);
$current_prime = shell_exec("sudo python valve2.py t 0 &");
usleep(5000);
$current_pump = shell_exec("sudo python valve1.py t 0 &");
usleep(5000);
$valve1 = shell_exec("sudo python valve1.py q 0 &");
usleep(5000);
$valve2 = shell_exec("sudo python valve2.py q 0 &");

if($valve2[2] == "1"){
        $valve = "Ready to Prime";
        $valveFlag = 2;
}
else if($valve1[2] == "0"){
        $valve = "Ready to Sample";
        $valveFlag = 1;
}
else if($valve1[2] == "1" && $valve2[2] =="0"){
        $valve = "Ready to Purge";
        $valveFlag = 0;
}
else{
        $valve = "Error";
        $valveFlag = 3;
}

$time_m = intval(floor(intval($current_pump)/60));
$time_s = intval($current_pump) % 60;
$ptime_m = intval(floor(intval($current_prime)/60));
$ptime_s = intval($current_prime) % 60;

?>
        <strong>Pump/Valve Control</strong><br><br>
        Prime Time Remaining: <?php echo sprintf("%02d:%02d", $ptime_m, $ptime_s);?><br>
        Purge Time Remaining: <?php echo sprintf("%02d:%02d", $time_m, $time_s);?><br><br>
        Valve Status: <?php echo $valve;?><br>
