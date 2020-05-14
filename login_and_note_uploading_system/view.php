<?php
    $servername = "localhost:3306";
    $username = "root";
    $password = "localhost";

    $conn = new mysqli($servername, $username, $password);

    if ($conn->connect_error){
        die("Connection failed: " . $conn->connect_error);
    }

    $user= $_COOKIE["user_name"];
    if(!$user){
        echo "You are not the owner of this note.";
        header("Location: login.php");
        exit;
    }
    
    echo $_GET['note_ID'];
     
?>