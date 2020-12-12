<?php
    include_once 'debug/logger.php';
    $filename = 'config/db.php';

    /**
     * This class allows coresponding with MySQL server in json format.
     */
    class DB extends mysqli {
    
        private $servername = "localhost";
        private $username = "user";
        private $password = "pass";
        private $db = "db";

        private $connected = false;

        /**
         * Constructor establishing a connection to DB server.
         */
        public function __construct() { 
            parent::__construct($this->servername, $this->username, $this->password, $this->db);
            
            if (mysqli_connect_errno()) {
                Logger::logError("config/db.php:24", $this->connect_errno, $this->connect_error);
                $this->connected = false;
            } 
            else {
                $this->connected = true;
                if(!parent::set_charset("utf8")) {
                    Logger::logError("config/db.php:30", $this->connect_errno, $this->connect_error);
                }
            }
        }

        /**
         * Method allows checking connection status.
         * @retval bool
         * Connections status.
         */
        public function isConnected() { return $this->connected; }


        /**
         * This method allows selecting data form the DB.
         * 
         * @param json $dml
         * The entrance parametrs are "select", "from", "where", all of them should follow the SQL syntax.
         *  
         *      E.g.: {"select":"some_columns","from":"some_table","where":"some_column=some_value"}.
         * 
         * @retval json 
         * Returned response constains "status", "no" and "comment" parameters. 
         * If there occure an error, the "no" parameter will contain code of that error.
         * Otherwise "no" will be equal to returned rows count.
         * When execution is successful, there will be also "output" parameter constins selected data.
         * 
         *      Successful response e.g.    {"status":"OK","no":"count(*)","comment":"","output":[]}
         * 
         *      Warning response e.g.       {"status":"WARNING","no":"count(*)","comment":""} 
         * 
         *      Error response e.g.         {"status":"ERROR","no":"error code","comment":""} 
         */
        public function select($dml) {

            if(isset($dml['select']) && isset($dml['from'])) {

                parent::options(MYSQLI_OPT_INT_AND_FLOAT_NATIVE, TRUE);

                $sql = "SELECT " . $dml['select'] . " FROM " . $dml['from'];
                if(isset($dml['where'])) $sql = $sql . " WHERE " . $dml['where'];
                
                $result = parent::query($sql);

                if ($result->num_rows > 0) {

                    $resp->status = "OK";
                    $resp->no = $result->num_rows;
                    $resp->comment = "Successfully executed.";
                    $resp->output = array();

                    while($row = $result->fetch_assoc()) {
                        $resp->output[] = $row;
                    }
                } 
                else if($result === false) {
                    $resp->status = "ERROR";
                    $resp->no = $this->errno;
                    $resp->comment = 'Table ' . $dml['from'] . ': ' . $this->error;
                    Logger::logError($filename . ":89", $this->errno, 'Table ' . $dml['from'] . ': ' . $this->error);
                } 
                else {  
                    $resp->status = "WARNING";
                    $resp->no = $result->num_rows;
                    $resp->comment = "There is no records to select.";
                }
            }
            else {
                $resp->status = "ERROR";
                $resp->no = 400;
                $resp->comment = "Parametrs 'select' and 'from' are required.";
                Logger::logError($filename . ':101', 400, 'Table ' . $dml['from'] . ": Parametrs 'select' and 'from' are required.");
            }

            return $resp;
        }


        /**
         * This method allows inserting new records into the DB.
         * 
         * @param json $ddl
         * Parameter must contain "INTO" where is stored destination table of the new records.
         * The "values_list" need to be made of atleast one row object made of atleast one pair like the following:
         * 
         *      "atribute_name":"val"
         * 
         *  so row object should look like:
         * 
         *      {"atribute_name":"val", ...}
         * 
         * Finally it must be like in the following example:
         * 
         *      {"into":"table_name","values_list":[{"atribute_name":"val", ...}, ...]}
         * 
         * @retval json
         * This method returns report of the operation.
         * "status" flag can be set to "OK" or "WARNING" or "ERROR".
         * "no" contains the last insert id or -1 when there was no insert.
         * 
         *      E.g.: {"status":"","no":last_insert_id,"comment":{"total":int,"inserts":int,"errors":int}}
         * 
         */
        public function insert($ddl) {

            if(isset($ddl['into']) && isset($ddl['values_list'])) {
                $raport->total = 0;
                $raport->inserts = 0;
                $raport->errors = 0;
        
                foreach($ddl['values_list'] as $vals) {
        
                    $sql = "INSERT INTO `" . $ddl['into'] . "` SET";
        
                    $isFirst = true;
                    foreach($vals as $key=>$val) {
        
                        $sql = $sql . ($isFirst ? " `" : ", `") .  $key . "`=" . (is_numeric($val) ? $val : ("'" . $val . "'"));
                        if($isFirst) $isFirst = false;
                    }
        
                    if(parent::query($sql) === TRUE) {
                        $raport->inserts++;
                    }
                    else {
                        $raport->errors++;
                        Logger::logError($filename . ":156", $this->errno, 'Table ' . $ddl['into'] . ': ' . $this->error);
                    }
                    $raport->total++;
                }
        
                if($raport->errors == 0) {
                    $resp->status = "OK";
                    $resp->no = $this->insert_id;
                }
                else if($raport->errors != $raport->total) {
                    $resp->status = "WARNING";
                    $resp->no = $this->insert_id;
                }
                else {
                    $resp->status = "ERROR";
                    $resp->no = -1;
                }
                $resp->comment = $raport;// $raport->total . ", inserts: " . $raport->inserts . ", errors: " . $raport->errors . ".";
            }
            else {
                $resp->status = "ERROR";
                $resp->no = 400;
                $resp->comment = "Parametrs 'into' and 'values_list' are required.";
                Logger::logError($filename . ':179', 400, 'Table ' . $ddl['into'] . ": Parametrs 'into' and 'values_list' are required.");
            }

            return $resp;
        }


        /**
         * Mathod to update records in the DB.
         * 
         * @param json $dml
         * This method must be called with the json object as parameter.
         * The first parametr set into json object is "UPDATE" in which must be name of table to be updated.
         * The second one is "SET" which must contains atleast one pair looking like: 
         *      {"some_column":"some_value", ...}
         * The last one called "WHERE" is optional and must follow the SQL syntax.
         * 
         *       e.g. {"update":"some_table","set":{"some_column":"some_value", ...},"where":"some_column=some_value"}
         * 
         * @retval json
         * The response is the status of execution.
         * When syntax is correct and there is no warning it looks like followig:
         *      {"status":"OK","no":affected_rows,"comment":""}
         *  Parameter "no" is equal to number of updated rows.
         * 
         * If there is some warnings or an error occured, 
         * "no" will be equal to code of that warning or an error 
         * and comment will contain description of it.
         *
         *      Warning response e.g. {"status":"WARNING","no":"warno","comment":""}
         *      Error response e.g. {"status":"ERROR","no":"errno","comment":""}
         */
        public function update($dml) {

            if(isset($dml['update']) && isset($dml['set'])) {
                $sql = "UPDATE `" . $dml['update'] . "` SET";
                
                $isFirst = true;
                foreach($dml['set'] as $key=>$val) {
            
                    $sql = $sql . ($isFirst ? " `" : ", `") .  $key . "`=" . (is_numeric($val) ? $val : ("'" . $val . "'"));
                    if($isFirst) $isFirst = false;
                }
    
                if(isset($dml['where'])) $sql = $sql . " WHERE " .  $dml['where'];
    
    
                if(parent::query($sql) === true) {
                    $resp->status = "OK";
                    $resp->no = $this->affected_rows;
                    $resp->comment = "Updated " . $this->affected_rows . " rows.";
                }
                if($this->errno) {
                    $resp->status = "ERROR";
                    $resp->no = $this->errno;
                    $resp->comment = $this->error;
                    Logger::logError($filename . ':235', $this->errno, 'Table ' . $dml['update']. ': ' . $this->error);
                }
                else if($this->warning_count) {
                    if ($result = parent::query("SHOW WARNINGS")) {
                        $row = $result->fetch_row();
    
                        $resp->status = "WARNING";
                        $resp->no = $row[1];
                        $resp->comment = $row[2];
                        Logger::logWarning($filename . ':244', $row[1], 'Table ' . $dml['update']. ': ' .  $row[2]);
    
                        $result->close();
                    }
                }
            }
            else {
                $resp->status = "ERROR";
                $resp->no = 400;
                $resp->comment = "Parametrs 'update' and 'set' are required.";
                Logger::logError($filename . ':254', 400, 'Table ' . $dml['update']. ": Parametrs 'update' and 'set' are required.");
            }
    
            return $resp;
        }
    
        /**
         * Methods to delete records from the DB.
         * 
         * @param json $dml
         * According to SQL syntax: DELETE FROM some_table [WHERE some_column=some_value]
         * $dml is a json object, it must contain FROM clause. However, optionaly it can contain WHERE clause.
         * 
         * e.g. {"from":"some_table","where":"some_colum=some_value"}
         * 
         * @retval json 
         * This method returns two kind of response. First one is returned when operation was successful executed.
         * Successfuly response looks like the following:
         *      {"status":"OK","no":"n","comment":"Deleted n rows."}
         *  The flag "status" is set to "OK" so that means success, parameter "no" is equal to "n" (n is the number of deleted rows.).
         * 
         * An error occured response:
         *      {"status":"ERROR","no":"errno","comment":""}
         *  The flag "status" is set to "ERROR", parameter "no" is equal to the "errno" which is a code of a occured error. 
         *  Comment field should contain descripiton of the error. 
         * 
         */
        public function delete($dml) {
            if(isset($dml['from'])) {
    
                $sql = "DELETE FROM " . $dml['from'];
                if(isset($dml['where'])) $sql = $sql . " WHERE " . $dml['where'];
                
                if(parent::query($sql) === true) {
                    $resp->status = "OK";
                    $resp->no = $this->affected_rows;
                    $resp->comment = "Deleted " . $this->affected_rows . " rows.";
                }
                if($this->errno) {
                    $resp->status = "ERROR";
                    $resp->no = $this->errno;
                    $resp->comment = $this->error;
                    Logger::logError($filename . ':296', $this->errno, $this->error);
                }
            }
            else {
                $resp->status = "ERROR";
                $resp->no = 400;
                $resp->comment = "Parametr 'from' is required.";
            }
    
            return $resp;
        }
    }

?>