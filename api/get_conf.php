<?php
    include_once 'functions.php';
    include_once 'debug/logger.php';

    $filename = 'get_conf.php';
    
    $api_key = $_GET["api_key"];
    
    if(isset($api_key) && $api_key == "co") {

        echo json_encode(getConfig());
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
?>