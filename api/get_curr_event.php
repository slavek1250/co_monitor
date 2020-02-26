<?php
    include_once 'config/db.php';
    include_once 'functions.php';

    $filename = 'get_curr_event.php';

    $api_key = $_GET["api_key"];

    if(isset($api_key) && $api_key == "apk") {

        $ans['eventId'] = 0;

        $query['select'] = '(case when co_event_id is null then 0 else co_event_id end) co_event_id';
        $query['from'] = 'co_current_measures';
        
        $db = new DB();
        $resp = $db->select($query);
        $db->close();

        if($resp->status == "OK") {
            $eventId = $resp->output[0]['co_event_id'];
            if($eventId > 0) {
                $ans = getEventDetails($eventId);
            }
        }      

        echo json_encode($ans);
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
?>