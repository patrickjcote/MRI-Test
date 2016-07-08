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
?>
    <div style="float:left">
        Hose Reel Board : <?php connected($hose_reel); ?><br>
        Pump/Valve Board : <?php connected($pump); ?>
    </div>
    <div style="float:right; align:right;">

        Data Reel Board : <?php connected($data_reel); ?><br>
        Sampler Board : <?php connected($sampler); ?>
    </div>
