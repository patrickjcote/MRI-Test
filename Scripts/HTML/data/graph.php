<?php
exec("sudo octave parse_ds_file.m &");
?>

Read Flag: <?php include("r.flag");?><br>
<img src="currentgraph.jpg?src=<?php echo rand();?> " width="50%">
<br>Raw Data:<br>
<a href="current.txt" target="_blank">View current raw data</a><br>
Time Temp pH Sal ORP LDO% Battery BP_mmHg
