<?php
    include_once 'functions.php';

    $filename = 'get_curr_measures.php';
    
    $api_key = $_GET["api_key"];
    
    if(isset($api_key) && $api_key == "apk") {
        echo json_encode(getParams());
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
?>