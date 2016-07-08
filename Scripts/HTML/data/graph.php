<?php
exec("sudo octave depth.m &");
?>

Current Data Updated: 
<?php
if(file_exists("current.txt")){
        echo date("m/d/Y - H:i:s",filemtime("current.txt"));
}
else{
        echo "No Current Data";
}
?>

<br>
<img src="currentdepth.jpg?src=<?php echo rand();?> " width="50%">
<br><br>
<?php
if(file_exists("current.txt")){?>
<a href="current.txt" target="_blank">View current raw data</a><br>
<?php } ?>
Read Flag: <?php include("r.flag");?><br>
