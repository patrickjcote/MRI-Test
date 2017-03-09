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
$data_reel_connection = shell_exec("sudo python data-reel.py i 0 &");
?>
<?php
if(file_exists("current.txt")){?>
<a href="current.txt" target="_blank">View current raw data</a><br>
<?php } ?>
<div style="clear:both">
<a href="logs/" target="_blank">View Logs</a>
</div>
Read Flag: <?php include("r.flag");?><br>
    <div>
        Data Reel: <?php connected($data_reel_connection); ?><br>
    </div>
