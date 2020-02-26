<?php
    include_once 'config/db.php';
    include_once 'functions.php';
        
    $filename = 'insert_data.php';
    $raw_json = file_get_contents('php://input');
    $json_obj = json_decode($raw_json, true);
    $api_key = $_GET["api_key"];

    $db = new DB();
    
    function stop() {

        die('{"type":"NACK"}');
    }

    
    if(isset($api_key) && isset($raw_json) && $api_key == 'apk') {
        $query['select'] = "co_param_id";
        $query['from'] = "co_current_param";
        $query['where'] = "co_param_id > 0 limit 1;";

        $resp = $db->select($query);
        if($resp->status != "OK") {
           stop();
        }

        $newestId = $resp->output[0]['co_param_id'];

        if($newestId > $json_obj['oldParamId']) {
            $db->close();
            $ans = getParams();
            $ans['type'] = "NOT_NEWEST";
            die(json_encode($ans));
        }

        $query1['into'] = "co_param";
        $query1['values_list'][0]['id'] = "null";

        $resp = $db->insert($query1);
        if($resp->status != "OK") {
            stop();
        }

        $newestId = $resp->no;

        $query2['into'] = "co_param_elem";

        $i=0;
        foreach($json_obj['newParams'] as $newParam) {
            $query2['values_list'][$i]['value'] = $newParam['value'];
            $query2['values_list'][$i]['co_param_id'] = $newestId;
            $query2['values_list'][$i]['co_def_id'] = $newParam['defId'];
            $i++;
        }
        
        $db->insert($query2);

        $ans = getParams();
        $ans['type'] = "ACK";
        echo json_encode($ans);
    }
    else {
        header("HTTP/1.0 401 Unauthorized");
        //Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
    }
    $db->close();

?>