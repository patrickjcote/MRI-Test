<html>

<head>
<title>MRI - Pit Boat Control</title>
<script src="jquery.js"></script> 
<script>
$(document).ready(function(){
        setInterval(function() {
                $("#graph").load("graph.php");
        }, 10500);
});

</script>
</head>

<body>
<center>
<iframe src="getdata.php" frameborder="0" scrolling="no"></iframe><br>
        <div id="graph" >
        <?php exec("sudo octave parse_ds_file.m");?>
        <img src="currentgraph.jpg" width="50%">
        
        </div>
</body>

</html>
