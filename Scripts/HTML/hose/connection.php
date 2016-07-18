<?php
function connected($instring){
        if($instring[0] == "O"){
                echo "<font color='green'>Connected</font>";
        }
        else{
                echo "<font color='red'>No Connection</font>";
        }
}
$hose_reel = shell_exec("sudo python hose-reel.py i 0 &");
$pump = shell_exec("sudo python valve.py i 0 &");
$prime = shell_exec("sudo python prime.py i 0 &");
?>
    <div>
        Hose Reel: <?php connected($hose_reel); ?><br>
        Pump/Valve: <?php connected($pump); ?><br>
        Prime Pump: <?php connected($prime); ?>
    </div>
