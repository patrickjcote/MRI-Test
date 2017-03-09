<?php
exec("sudo octave depth.m &");
?>

Current Depth Graph Updated: 
<?php
if(file_exists("current.txt")){
        echo date("m/d/Y - H:i:s",filemtime("current.txt"));
}
else{
        echo "No Current Data";
}
?>

<br>
<img src="currentdepth.jpg?src=<?php echo rand();?>">
