<h1>Login</h1>
<form action="create_user.php" method="GET">
Username:
<input type="text" name="user_name"><br>
Password:
<input type="text" name="password"><br>
First name:
<input type="text" name="first_name"><br>
Last name:
<input type="text" name="last_name"><br>
<input type="submit">
</form>

<?php

$errors = array();

if (isset($_GET["user_name"]) && isset($_GET["password"]) && isset($_GET["first_name"]) && isset($_GET["last_name"])){
    $servername = "localhost:3306";
    $username = "root";
    $password = "localhost";

    $conn = new mysqli($servername, $username, $password);
    if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
    }
    $validation = "SELECT * FROM users WHERE username ='$user_name'";
    $validation_result = mysqli_query($conn, $validation);
    $user = mysqli_fetch_assoc($validation_result);

    if($user){
        if ($user['username'] == $username){
            array_push($errors. "Username already exists");
        }
    }

    if(count($errors) == 0){
        $sql = "INSERT INTO a2.users (name, password, firstname, lastname) 
        VALUES ('".$_Get["user_name"]."', SHA2('".$_Get["password"]."',256), 
        '".$_Get["first_name"]."','".$_Get["$last_name"].";";

        $resultSet = $conn->query($sql);
        if (!$resultSet) {
        trigger_error('Invalid query: ' . $conn->error);
        }else {
            echo "<h2>Invalid username/password!</h2>";
        }
    }

    $conn->close();



}

?>