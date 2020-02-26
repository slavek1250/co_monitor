<?php 
    include_once 'debug/logger.php';
    include_once 'config/db.php';
    include_once 'functions.php';
    

    Logger::logMessage("api_test.php", 1, "Test");
?>

<html>
    <head>
        <script>
            function post(obj, url) {
                var xhr = new XMLHttpRequest();
                xhr.open("POST", "../api/" + url, true);

                xhr.setRequestHeader("Content-type", "application/json");

                xhr.send(JSON.stringify(obj));

                xhr.onreadystatechange = () => {
                    if(xhr.readyState == 4) {
                        if(xhr.status == 200) {
                            setData(xhr.responseText)
                        }
                        else {
                            setData({"status":"ERROR", "no":xhr.status, "comment":xhr.statusText})
                        }
                    }
                }
            }

            function setData(data) {
                document.getElementById('data').innerText = data;
                console.log(data);
            }

            window.onload = () => {
                setData("Starting...");
            }
        </script>
    </head>
    <body>
        <div id="data"></data>
    </body>
</html>