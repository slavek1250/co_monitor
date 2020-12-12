<?php
    include_once 'debug/logger.php';
    include_once 'config/db.php';
    include_once 'functions.php';
    
    // type: CONF, ACK, NACK
    
    $filename = 'insert_data.php';
    $raw_json = file_get_contents('php://input');
    $raw_json_without_nan = str_replace('NAN', '0', $raw_json);
    if($raw_json_without_nan != null)
        $raw_json = $raw_json_without_nan;    
    $json_obj = json_decode($raw_json, true);
    $api_key = $_GET["api_key"];

    $db = new DB();
    
    function stop() {

        $db->close();
        die('{"type":"NACK"}');
    }

    if(isset($api_key) && isset($raw_json) && $api_key == 'co' && $json_obj['type'] == 'DATA') {

        $measures_co_param_id = $json_obj['id'];

        $query['select'] = "co_event_id, co_def_name_to_comp as 'param', operator, value_comp_to, co_param_id";
        $query['from'] = "co_event_checker";
        
        $resp = $db->select($query);

        if($resp->status != "OK") {
            stop();
        }
        
        $newest_co_param_id = 1;
        $co_event_id = -1;
        foreach($resp->output as $event) {
            $newest_co_param_id = $event['co_param_id'];

            $current_event_id = $event['co_event_id'];
            $value_comp_to = $event['value_comp_to'];
            $operator = $event['operator'];
            $param = $json_obj[$event['param']];

            if($operator == '>') {
                if($param > $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
            else if($operator == '>=') {
                if($param >= $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
            else if($operator == '<') {
                if($param < $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
            else if($operator == '<=') {
                if($param <= $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
            else if($operator == '=' || $operator == '==') {
                if($param == $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
            else if($operator == '!=') {
                if($param != $value_comp_to) {
                    $co_event_id = $current_event_id;
                    break;
                }
            }
        }

        $query1['into'] = "co_measurement";
        $query1['values_list'][0]['co_param_id'] = $measures_co_param_id;
        if($co_event_id != -1) {
            $query1['values_list'][0]['co_event_id'] = $co_event_id;
        }
       
        $resp = $db->insert($query1);
        
        if($resp->status != "OK") {
            stop();
        }
        $new_measure_id = $resp->no;
        
        
        $query['select'] = "id as 'co_def_id', name as 'co_def_name'";
        $query['from'] = "co_def";
        $query['where'] = "def_type = 'm'";
        $resp = $db->select($query);
        if($resp->status != "OK") {
            stop();
        }
        
        $query2['into'] = "co_measurement_elem";
        $i = 0;
        foreach($resp->output as $elem) {
            if(isset($json_obj[$elem['co_def_name']])) {
                $query2['values_list'][$i]['co_measurement_id'] = $new_measure_id;
                $query2['values_list'][$i]['co_def_id'] = $elem['co_def_id'];
                $query2['values_list'][$i]['value'] = $json_obj[$elem['co_def_name']];
                $i++;
            }
        }
        
        $resp = $db->insert($query2);
        if($resp->status != "OK") {
            stop();
        }
        
        if($measures_co_param_id < $newest_co_param_id) {
            echo json_encode(getConfig());
        }   
        else {
            echo '{"type":"ACK"}';
        }
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
    $db->close();
    
?>