<?php
    $username = "localhost:3306";
    $username = "root";
    $password = "localhost";

    $conn = new mysqli($servername, $username, $password);

    if ($conn->connect_error){
        die("Connection failed: " . $conn->connect_error);
    }
    $user= $_COOKIE["user_name"];
    if(! $user){
        header('Location: login.php');
        exit;
    }
    $password = $_COOKIE["password"];
    echo 'Welcome, '. $user .'.';

?>