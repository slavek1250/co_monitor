<?php
    include_once 'config/db.php';
    
    function getConfig() {
        $db_ = new DB();

        $query['select'] = "cd.name, cd.dev_serial";
        $query['from'] = "co_def cd";
        $query['where'] = "cd.dev_serial is not null;";

        $resp = $db_->select($query);

        if($resp->status == 'OK') {

            foreach($resp->output as $elem) {
                $ans[$elem['name']] = $elem['dev_serial'];
            }

            $query['select'] = "cp.co_param_id as 'id', cp.name, cp.value";
            $query['from'] = "co_current_param cp";
            $query['where'] = "cp.def_type = 's';";

            $resp = $db_->select($query);

            if($resp->status == "OK") {
                foreach ($resp->output as $elem) {
                    $ans['id'] = $elem['id'];
                    $ans[$elem['name']] = $elem['value'];
                }

                $ans['type'] = "CONF";
            }            
        }
        
        if(!isset($ans)) $ans['type'] = "NACK";
        $db_->close();
        return $ans;
    }

    function getParams() {
        $db_ = new DB();

        $query['select'] = "*";
        $query['from'] = "co_current_param";

        $resp = $db_->select($query);

        if($resp->status == 'OK') {
            $i = 0;
            $ans['type'] = "PARAM";
            foreach($resp->output as $elem) {
                $ans['timestamp'] = $elem['timestamp'];
                $ans['paramId'] = $elem['co_param_id'];
                $ans['params'][$i]['defId'] = $elem['co_def_id'];
                $ans['params'][$i]['name'] = $elem['name'];
                $ans['params'][$i]['value'] = $elem['value'];
                $i++;
            }           
        }
        
        $db_->close();
        return $ans;
    }

    function getEventDetails($eventId) {
        $db_ = new DB();

        $query['select'] = "ce.description as 'event_descript', ceo.description as 'operation_descript'";
        $query['from'] = "co_event ce join co_event_operation ceo on ce.co_event_operation_id = ceo.id";
        $query['where'] = "ce.id = " . $eventId . ";";

        $resp = $db_->select($query);

        if($resp->status == "OK") {
            $ans['eventId'] = $eventId;
            $ans['eventDesctription'] = $resp->output[0]['event_descript'];
            $ans['operationDescription'] = $resp->output[0]['operation_descript'];
        }

        $db_->close();
        return $ans;
    }
?>