<html>

<head>
<title>MRI - Pit Boat Control</title>
<script src="../jquery.js"></script>
<link rel="stylesheet" type="text/css" href="style.css" />
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#graph").load("graph.php");
        }, 1000);
});

</script>

<style>

padding:10px;

</style>

</head>

<?php

$value = $_POST["value"];
$cmd = $_POST["cmd"];
$date = date('Ymd.Hi').'.txt';
        
if($cmd == "getdata"){
        exec("sudo python readsonde.py $date &");
}
?>


<body>
<center>

<?php echo $date;?>
        <form action="index.php" method="post">
        <input type="hidden" name="cmd" value="getdata">
        <input type="submit" value="Start recording data">
</form>

        <div id="graph" >
        <?php exec("sudo octave parse_ds_file.m");?>
        <img src="currentgraph.jpg" width="50%">
        
        </div>
</body>

</html>
