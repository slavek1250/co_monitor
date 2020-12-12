<?php
    include_once 'config/db.php';

    $filename = 'get_plot_data.php';
    
    $api_key = $_GET["api_key"];
    
    if(isset($api_key) && $api_key == "web-app") {

        $db = new DB();
        $ans['success'] = false;

        $query['select'] = "timestamp, heater_temp, outside_temp, set_temp, feader_break";
        $query['from'] = "co_plot_data";

        $resp = $db->select($query);


        if($resp->status == "OK") {
            $ans['success'] = true;
            $ans['data'] = $resp->output;
        }

        echo json_encode($ans);
        
        $db->close();
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
?>