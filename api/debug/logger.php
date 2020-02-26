<?php
    /**
     * This class is made to log any events, which occured in another scripts, to the log file.
     */
    class Logger {
        private static $filename = "debug.log";

        /**
         * Static method to log errors.
         * 
         * @param string $filename
         * Filepath and name of file where events occured.
         * 
         * @param int $id
         * Error code.`
         * 
         * @param string $description
         * Description of the occured error.
         *  
         *  E.g.: Logger::logError("debug/logger.php:20", 500, "Internal server error");
         */
        public static function logError($filename, $id, $description) {
            self::addLog("Error", $filename, $id, $description);
        }

        /**
         * Static method to log warnings.
         * 
         * @param string $filename
         * Filepath and name of file where events occured.
         * 
         * @param int $id
         * Warning code.`
         * 
         * @param string $description
         * Description of the occured warning.
         *  
         *  E.g.: Logger::logWarning("logger.php:38", 80, "Unsafe connection.");
         */
        public static function logWarning($filename, $id, $description) {
            self::addLog("Warning", $filename, $id, $description);
        }

        /**
         * Static method to log messages.
         * 
         * @param string $filename
         * Filepath and name of file where events occured.
         * 
         * @param int $id
         * Message code.
         * 
         * @param string $description
         * Description of the message.
         * 
         * E.g.:  Logger::logMessage("logger.php:56", 200, "Connected.");
         */
        public static function logMessage($filename, $id, $description) {
            self::addLog("Message", $filename ,$id, $description);
        }

        private static function addLog($type, $filename, $id, $description) {
            $single_log->type = $type;
            $single_log->file = $filename;
            $single_log->timestamp = date("Y-m-d H:i:s");
            $single_log->id = $id;
            $single_log->description = $description;

            $fp = fopen(self::$filename, 'a');
            if($fp === FALSE) {
                throw new Exception("Unable to open log file.");
            }

            $strToFile = json_encode($single_log) . "\r\n";
            
            fwrite($fp, $strToFile);
            fclose($fp);
        }
    }
?>