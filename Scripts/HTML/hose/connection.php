<?php
function connected($instring){
        if($instring[0] == "O"){
                echo "<font color='green'>Connected</font>";
        }
        else{
                echo "<font color='red'>No Connection</font>";
        }
}
usleep(50000);
$hose_reel = shell_exec("sudo python hose-reel.py i 0 &");
usleep(5000);
$valve1 = shell_exec("sudo python valve1.py i 0 &");
usleep(5000);
$valve2 = shell_exec("sudo python valve2.py i 0 &");
?>
    <div>
        Hose Reel: <?php connected($hose_reel); ?><br>
        Valve 1/Purge Pump: <?php connected($valve1); ?><br>
        Valve 2/Prime Pump: <?php connected($valve2); ?>
    </div>
