<?php

/**
 * Input data:
 * {
 *  "getMaxRange": bool,
 *  "lastHours": "6:15",
 *  "range": {
 *      "since": "2020-12-12T06:30",
 *      "until": "2020-12-12T06:38"
 *  }
 *  "series": [
 *      string
 *  ]
 * }
 * 
 * 
 * Output data:
 * {
 *  code: int,
 *  maxRange: {
 *      since: string,
 *      until: string
 *  }
 *  data: [
 *      {
 *          serieName: value...
 *      }
 *  ]
 * }
 */

include_once 'config/db.php';

$filename = 'get_plot_data.php';

$api_key = $_GET["api_key"];
$raw_json = file_get_contents('php://input');
$json_obj = json_decode($raw_json, true);

if(isset($api_key) && $api_key == "web-app" && isset($raw_json) && isset($json_obj)) {

    $db = new DB();
    $ans['code'] = 400;

    $query['select'] = implode(", ", $json_obj['series']);
    $query['from'] = "dev_co_plot_data";
    $query['where'] = "";

    if(isset($json_obj['lastHours'])) {
        $hoursMins = explode(":", $json_obj['lastHours']);
        $mins = $hoursMins[0] * 60 + $hoursMins[1];
        $query['where'] = "`timestamp` > (NOW() - INTERVAL ".$mins." MINUTE)";
    }
    else if(isset($json_obj['range'])) {
        $query['where'] = "`timestamp` >= '".$json_obj['range']['since']."' and `timestamp` <= '".$json_obj['range']['until']."'";
    }
    else {
        $query['where'] = "`timestamp` > (NOW() - INTERVAL 6 HOUR)";
    }

    $resp = $db->select($query);

    if($resp->status == "OK") {
        $ans['code'] = 200;
        $ans['data'] = $resp->output;
    }
    else {
        $ans['code'] = $resp->no;
    }

    unset($resp);

    if($ans['code'] == 200 && isset($json_obj['getMaxRange']) && $json_obj['getMaxRange'])
    {
        $query['select'] = "timestamp";
        $query['where'] = "id in (SELECT min(id) FROM `dev_co_plot_data` union SELECT max(id) FROM `dev_co_plot_data`)";

        $resp = $db->select($query);

        if($resp->status == "OK") {
            $ans['maxRange']['since'] = str_replace(" ", "T", $resp->output[0]['timestamp']);
            $ans['maxRange']['until'] = str_replace(" ", "T", $resp->output[1]['timestamp']);
        }
        else {
            $ans['code'] = $resp->no;
        }
    }

    echo json_encode($ans);
    
    $db->close();
}
else {
    header("HTTP/1.0 401 Unauthorized");
    Logger::logWarning($filename . ":33", 401, 'Unauthorized.');
}

?>