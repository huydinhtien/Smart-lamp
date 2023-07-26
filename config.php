<?php
	$server = "localhost";
	$username = "admin";
	$password = "admin";
	$dbname = "smart_lamp";

	$conn = mysqli_connect($server, $username, $password, $dbname);
	
	if($conn === false){
		die("ERROR: Could not connect." . mysqli_connect_error());
		}
?>