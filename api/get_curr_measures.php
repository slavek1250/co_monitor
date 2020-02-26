<?php
    include_once 'config/db.php';

    $filename = 'get_curr_measures.php';
    
    $api_key = $_GET["api_key"];
    
    if(isset($api_key) && $api_key == "apk") {

        $db = new DB();

        $query['select'] = "timestamp, co_measurement_id, co_event_id, name, description, value, unit, set_value";
        $query['from'] = "co_current_measures";

        $resp = $db->select($query);

        if($resp->status == "OK") {
            $i = 0;
            foreach($resp->output as $arrElem) {

                $ans['timestamp'] = $arrElem['timestamp'];
                $ans['measurementId'] = $arrElem['co_measurement_id'];
                $eventId = $arrElem['co_event_id'];

                $ans['values'][$i]['name'] = $arrElem['name'];
                $ans['values'][$i]['description'] = $arrElem['description'];
                $ans['values'][$i]['value'] = $arrElem['value'];
                $ans['values'][$i]['unit'] = $arrElem['unit'];
                $ans['values'][$i]['setValue'] = $arrElem['set_value'];
                $i++;
            }
            

            if($eventId > 0) {
                $query['select'] = "ce.description as 'event_descript', ceo.description as 'operation_descript'";
                $query['from'] = "co_event ce join co_event_operation ceo on ce.co_event_operation_id = ceo.id";
                $query['where'] = "ce.id = " . $eventId . ";";

                $resp = $db->select($query);

                if($resp->status == "OK") {
                    foreach($resp->output as $arrElem) {
                        $ans['event']['eventId'] = $eventId;
                        $ans['event']['eventDesctription'] = $arrElem['event_descript'];
                        $ans['event']['operationDescription'] = $arrElem['operation_descript'];
                    }
                }
            }

            echo json_encode($ans);
        }
        
        $db->close();
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
?>