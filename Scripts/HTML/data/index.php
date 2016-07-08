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
        <h1>Data Sonde Control</h1>
        <div id="graph" >
<?php include("graph.php");?> 
        </div>

<iframe src="getdata.php" frameborder="0" scrolling="no"></iframe><br>
</body>

</html>
