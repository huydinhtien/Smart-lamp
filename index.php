<?php
    include("config.php");

	$red = $green = $blue = 0;
	
    $sql = "select * from rgbValues";
    $result = mysqli_query($conn,$sql);
    $rgb = mysqli_fetch_row($result);
    //printf("%d %d %d\n", $rgb[0],$rgb[1],$rgb[2]);
    
    $color = ($rgb[0]<<16)|($rgb[1]<<8)|($rgb[2]);
    if($_SERVER["REQUEST_METHOD"] == "POST"){
        //user input
        $red = $_POST["r"];
        $green = $_POST["g"];
        $blue = $_POST["b"];
        //update table
        $sql = "update rgbValues set red=$red,green=$green,blue=$blue";
        mysqli_query($conn,$sql);
        $sql = "select * from rgbValues";
        $result = mysqli_query($conn,$sql);
        $rgb = mysqli_fetch_row($result);
        $color = ($rgb[0]<<16)|($rgb[1]<<8)|($rgb[2]);
	}

    mysqli_close($conn);
?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Smart Lamp</title>
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css">
	<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css">
	<style>
		body{font: 14px san-serif;}
		.wrapper{float: left; width: 30%; padding: 20px; margin: 20px; height: 420px;}
		.box{margin: auto; width: 60%; height: 50%;}
		.table_size{margin: auto; width: 70%;}
	</style>
</head>

<body>
	<div class="wrapper border border-2 rounded border-primary">
		<h3>DHT11</h3>
		<br>
		<h5>Current Temperature and Humidity values:</h5>
		<table class="table table_size">
			<thead>
				<tr>
					<th scope="col">Temperature</th>
					<th scope="col">Humidity</th>
				</tr>
			</thead>
			<tbody>
				<tr>
					<td> 27.1 C</td>
					<td>40.0 %</td>
				</tr>
			</tbody>
		</table>
		<br>
		<br>
	</div>
	<div class="wrapper border border-2 rounded border-primary">
		<h2>RGB LED CONTROL</h2>
		<br>
		<form action="<?php echo htmlspecialchars($_SEVER['PHP_SELF']); ?>" method="POST">
			<div class="mb-3">
				<label class="form-label">Color Adjustment</label> <br>
				R: <input type="range" class="form-range" name="r" min="0" max="255" id="red">
				G: <input type="range" class="form-range" name="g" min="0" max="255" id="green">
				B: <input type="range" class="form-range" name="b" min="0" max="255" id="blue">
			</div>
			<div class="mb-3">
				<input type="submit" class="btn btn-primary" value="Apply">
			</div>
		</form>
	</div>
	
	<div class="wrapper border border-2 rounded border-primary">
		<h3>CURRENT LED COLOR</h3>
		<br>
		<div class="box border border-info" style="background-color: #<?php echo dechex($color)?>;"> </div> <br>
		<h5>Current RGB values:</h5>
		<table class="table table_size">
			<thead>
				<tr>
					<th scope="col">Red</th>
					<th scope="col">Green</th>
					<th scope="col">Blue</th>
				</tr>
			</thead>
			<tbody>
				<tr>
					<td> <?php echo $rgb[0]; ?> </td>
					<td> <?php echo $rgb[1]; ?> </td>
					<td> <?php echo $rgb[2]; ?> </td>
				</tr>
			</tbody>
		</table>
	</div>
</body>

</html>