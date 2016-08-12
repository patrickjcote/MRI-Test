<?php
function connected($instring){
        if($instring[0] == "O"){
                echo "<font color='green'>Connected</font>";
        }
        else{
                echo "<font color='red'>No Connection</font>";
        }
}


usleep(5103);

$prime = shell_exec("sudo python sampler.py i 0 &");
?>
    <div>
        Sampler: <?php connected($prime); ?>
    </div>
