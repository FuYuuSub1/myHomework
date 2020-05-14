<h1>Login</h1>
<form action="login.php" method="GET">
Username:
<input type="text" name="user_name"><br>
Password:
<input type="text" name="password"><br>
<input type="submit">
</form>


<?php
if (isset($_GET["user_name"]) && isset($_GET["password"])) {
   $servername = "localhost:3306";
   $username = "root";
   $password = "localhost";
   // Create connection
   $conn = new mysqli($servername, $username, $password);
   // Check connection
   if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	$sql = "SELECT name, password FROM a2.users WHERE name='"
    .$_GET["user_name"]."' and password=SHA2('CONCAT(".$_GET["password"].",',256);";
			
    //$sql = "SELECT name, password FROM lab1.USER WHERE name='1'";
	printf("The SQL Statement:<br> $sql<br/></br>");

 $resultSet = $conn->query($sql);
      if (!$resultSet) {
      trigger_error('Invalid query: ' . $conn->error);
      }

      if ($resultSet->num_rows > 0) {
     	 $row = $resultSet ->fetch_assoc(); //fetch a result row as an associative array
      	echo "<h2> Welcome: ".$row["name"]."</h2>";
      }
      else {
      	echo "<h2>Invalid username/password!</h2>";
      }

	$conn->close();
}
?>