/*
 * Created on Sun Dec 27 2020
 *
 * Author: Dominik Kala
 * E-mail: dominikkala@gmail.com
 * Copyright (c) 2020 Dominik Kala
 */

var timer = null;

const fetchData = (reqBody, callback) => {
  var xmlhttp = new XMLHttpRequest();
  var url = "http://dkala.hopto.org/dev/co_monitor/api/get_plot_data.php?api_key=web-app";

  xmlhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var myArr = JSON.parse(this.responseText);
      callback(myArr);
    }
  };
  xmlhttp.open("POST", url, true);
  xmlhttp.setRequestHeader('Content-type', 'application/json');
  xmlhttp.send(JSON.stringify(reqBody));
}

function showLoader(visible) {
  let loader = document.getElementById("loader_continer");
  let app = document.getElementById("app_continer");
  let chart = document.getElementById("curve_chart");
  if (!visible) {
    loader.classList.add("hide");
    app.classList.remove("hide");
    chart.classList.add("spreadOnPage");
  }
  else {
    chart.classList.remove("spreadOnPage");
    app.classList.add("hide");
    loader.classList.remove("hide");
  }
}

const parseDataToArray = data => {

  var out = data.data.map(row => {
    return [
      new Date(row.timestamp), row.heater_temp, row.set_temp, row.outside_temp, 
      row.feeder_temp, row.feeder_break, row.feeder_revo_num
    ];
  });

  out.unshift([
    'Czas', 'Temp. pieca [\xB0C]', 'Zadana temp. [\xB0C]', 'Temp. na zewnątrz [\xB0C]', 
    'Temp. podajnika [\xB0C]', 'Czas przerwy [s]', 'Liczba obrotów'
  ]);

  return out;
}

const setMaxRange = range => {
  const showRangeSince = document.getElementById("showRangeSince");
  const showRangeUntil = document.getElementById("showRangeUntil");
  showRangeSince.min = range.since;
  showRangeSince.max = range.until;
  showRangeUntil.min = range.since;
  showRangeUntil.max = range.until;
}

function drawChart(dataArray) {
  var data = google.visualization.arrayToDataTable(dataArray);

  var options = {
    title: 'C.O. Monitor',
    chartArea: {
      // leave room for y-axis labels
      width: '94%',
      height: '90%',
    },
    hAxis: {
      format: 'HH:mm',
      gridlines: { count: 15 }
    },
    width: '100%',
    height: '100%',
    legend: { position: 'bottom' },
    crosshair: { trigger: 'both' }
  };

  var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));

  showLoader(false);
  chart.draw(data, options);
}

const initChart = () => {
  const lastHoursMarker = document.getElementById("lastHours").checked;
  const lastHours = document.getElementById("lastHoursInput").value;
  const rangeSince = document.getElementById("showRangeSince").value;
  const rangeUntil = document.getElementById("showRangeUntil").value;

  var reqBody = {
    series: [
      "timestamp", "heater_temp", "set_temp", "feeder_temp", 
      "outside_temp", "feeder_break", "feeder_revo_num"
    ]
  };

  if(lastHoursMarker) {
    reqBody.lastHours = lastHours
  }
  else {
    if((new Date(rangeSince)) > (new Date(rangeUntil))) {
      alert("Data początku późniejsza niż końca!")
      return;
    }

    reqBody.range = {
      since: rangeSince,
      until: rangeUntil
    }
  }

  saveSettingsIntoUrl(reqBody);

  fetchData(reqBody, respBody => {
    if (respBody.code == 200) {
      drawChart(parseDataToArray(respBody));
      if(respBody.maxRange) {
        setMaxRange(respBody.maxRange);
      }
    }
  });
}

const switchProps = option => {
  const optionVal = option.lastHours || !option.showRange;
  const lastHoursMarker = document.getElementById("lastHours").checked;
  
  document.getElementById("lastHoursInput").disabled = !optionVal
  document.getElementById("showRangeSince").disabled = optionVal
  document.getElementById("showRangeUntil").disabled = optionVal

  if(lastHoursMarker && !timer) {
    timer = setInterval(initChart, 60000);
  }
  else if(!lastHoursMarker && timer) {
    clearInterval(timer);
    timer = null;
  }
}

const loadDataBtnHandler = () => {
  showLoader(true);
  initChart();
}

const saveSettingsIntoUrl = settings => {
  const displayType = (settings.lastHours ? 1 : 2);
  let newUrlParam = `?t=${displayType}&`;
  if(displayType == 1)
  {
    newUrlParam += `l=${settings.lastHours}`;
  }
  else if(displayType == 2)
  {
    const begin = settings.range.since;
    const end = settings.range.until;
    newUrlParam += `b=${begin}&e=${end}`;
  }

  history.replaceState(null, null, newUrlParam)
}

const restoreSettingsFromUrl = () => {
  if(!window.location.search || window.location.search == "")
    return false
  
  const params = window.location.search
  const settType = params.match(/t=(\d+)/)

  if(!settType || settType.length != 2)
    return false
  
  if(settType[1] == "1")
  {
    settVal = params.match(/l=(\d{1,2}:\d{1,2})/)

    if(!settVal || settVal.length != 2)
      return false

    document.getElementById("lastHoursInput").value = settVal[1]
    document.getElementById("lastHours").checked = true

    switchProps({ lastHours: true })
    return true
  }
  else if(settType[1] == "2")
  {
    settVals = params.match(/b=(\d{4}-\d{1,2}-\d{1,2}T\d{1,2}:\d{1,2})&e=(\d{4}-\d{1,2}-\d{1,2}T\d{1,2}:\d{1,2})/)

    if(!settVals || settVals.length != 3)
      return false

    const since = settVals[1]
    const until = settVals[2]
    
    if((new Date(since)) > (new Date(until)))
      return false

    document.getElementById("showRangeSince").value = since
    document.getElementById("showRangeUntil").value = until
    document.getElementById("showRange").checked = true
    
    switchProps({ showRange: true })
    return true
  }
  return false
}

window.addEventListener('load', () => {
  showLoader(true);

  if(!restoreSettingsFromUrl() && !timer)
    timer = setInterval(initChart, 60000);

  google.charts.load('current', { 'packages': ['corechart'] });
  google.charts.setOnLoadCallback(initChart);
 
  document.getElementById("lastHours").addEventListener("change", e => 
    switchProps({ lastHours: e.target.checked })
  )
  document.getElementById("showRange").addEventListener("change", e =>
    switchProps({ showRange: e.target.checked })
  )

  document.getElementById("propsLoadBtn").addEventListener("click", loadDataBtnHandler)
});
