
var devNameLabel         = document.getElementById("devNameLabel");
var setAmpsButton        = document.getElementById("set_current");
var setWattsButton       = document.getElementById("set_watts");

var setConfigButton = document.getElementById("setConfigButton");
var factoryDefaultsButton = document.getElementById("setDefaultsButton");
var rebootButton = document.getElementById("rebootButton");

var titleLabel = document.getElementById("titleLabel");
var devNameField = document.getElementById("devNameText");
var groupNameField = document.getElementById("groupNameText");
var enableSyslogCB = document.getElementById("syslogEnableCheckbox");
var maxPwrPlotPointsField = document.getElementById("maxPwrPlotPoints");

var maxPlotPointsIPF     = document.getElementById("maxPwrPlotPoints");
var wattsDateTimeList    = [];
var wattsList = [];

var ampsDateTimeList    = [];
var ampsList = [];

var voltsDateTimeList    = [];
var voltsList = [];

const guageSize    = 250;

var maxWatts=1;
var wattsGauge = null;

var maxAmps = 1;
var ampsGauge = null;

var maxVolts = 1;
var voltsGauge = null;


/**
 * @brief A UserOutput class responsible for displaying log messages
 **/
class UO {
   /**
    * @brief constructor
    * @param debugEnabled If True debug messages are displayed.
    **/
   constructor(debugEnabled) {
       this.debugEnabled=debugEnabled;
   }

   /**
    * @brief Display an info level message.
    * @param msg The message text to be displayed on the console.
    **/
   info(msg) {
       console.log("INFO:  "+msg);
   }

   /**
    * @brief Display a warning level message.
    * @param msg The message text to be displayed on the console.
    **/
   warn(msg) {
       console.log("WARN:  "+msg);
   }

   /**
    * @brief Display a error level message.
    * @param msg The message text to be displayed on the console.
    **/
   error(msg) {
       console.log("ERROR: "+msg);
   }

   /**
    * @brief Display a debug level message if the constructor argument was true.
    * @param msg The message text to be displayed on the console.
    **/
   debug(msg) {
       if( this.debugEnabled ) {
           console.log("DEBUG: "+msg);
       }
   }
}
var uo = new UO(true);

/**
 * @brief Add to a time series plot
 * @param yValue The value list.
 * @param yValueList A list Y range values on a plot.
 * @param
 * @returns
 */
function addToTimeSeries(yValue, xValueTimeList, yValueList) {
    var now = new Date();
    var hours = now.getHours();
    var minutes = now.getMinutes();
    var seconds = now.getSeconds();
    var tenthSeconds = now.getMilliseconds() / 100;
    var formattedDateTime = "" + hours.toString().padStart(2, "0") + ":"
            + minutes.toString().padStart(2, "0") + ":" + seconds;

    xValueTimeList.push(formattedDateTime);
    yValueList.push(yValue);
}

/**
 * @brief Limit the maximum number of plot points.
 * @param xValueList A list of x Value on a plot.
 * @param yValueList A list of y Value on a plot.
 * @returns
 */
function limitMaxPlotPoint(xValueList, yValueList) {
    var maxPlotPoints = parseInt(maxPlotPointsIPF.value);
    // Limit the plot size so it does not slow down the browser.
    if (xValueList.length > maxPlotPoints) {
        // Randomly remove an element from the list leaving the first
        // to preserve the length of the plot.
        indexToDel = getRandomInt(1, maxPlotPoints);
        xValueList.splice(indexToDel, 1);
        yValueList.splice(indexToDel, 1);
    }
}

/**
 * @brief Get a trace instance.
 * @param traceName The name of the plot trace to add
 * @param xValueList A list of x Value on a plot.
 * @param yValueList A list of y Value on a plot.
 * @returns
 */
function getTrace(traceName, xValueList, yValueList) {
 //PJA   traceColour = 'rgb(0, 102, 0)';
    trace = {
            opacity : 0,
            name : traceName,
            x : xValueList,
            y : yValueList,
            fill : 'tozeroy',
            type : 'scatter',
            line : {
//PJA                color : traceColour,
                width : 0,
                shape : 'spline'
            },
        }
    return trace
}

/**
 * @brief Get A layout for a time series plot.
 * @param yAxisLabel The Y axis label.
 * @returns
 */
function getTimePlotLayout(yAxisLabel) {
    var layout = {
            xaxis : {
                title : 'Time',
            },
            yaxis : {
                title : yAxisLabel,
            },
        };
    return layout
}

/**
 * @brief Plot data on a plot
 * @param plotCanvasName The name of the plot canvas in the html file.
 * @param traceList A list of traces to plot.
 * @param layout The layout to use to plot.
 * @returns 
 */
function plot(plotCanvasName, traceList, layout) {
    // As Plotly is to big to fit in the devices flash it is downloaded direct
    // and therefore may not be found so wrap in a try/catch block so the
    // javascript runs if Plotly is not available.
    try {
        Plotly.newPlot(plotCanvasName, traceList, layout, {
            responsive : true
        });
    } catch (err) {
    }
}

/**
 * @brief Plot watts
 * @param watts The DC load in watts.
 */
function plotWatts(watts) {

    var layout = getTimePlotLayout("Watts");

    limitMaxPlotPoint(wattsDateTimeList, wattsList);
    
    addToTimeSeries(watts, wattsDateTimeList, wattsList);

    var trace = getTrace("Power", wattsDateTimeList, wattsList);

    plot('wattsPlotArea', [ trace ], layout);

    return trace;
}

/**
 * @brief Plot amps
 * @param amps The DC load in amps.
 */
function plotAmps(amps) {

    var layout = getTimePlotLayout("Amps");

    limitMaxPlotPoint(ampsDateTimeList, ampsList);
    
    addToTimeSeries(amps, ampsDateTimeList, ampsList);

    var trace = getTrace("Current", ampsDateTimeList, ampsList);

    plot('ampsPlotArea', [ trace ], layout);

    return trace;
}

/**
 * @brief Plot vols
 * @param volts The DC voltage in volts.
 */
function plotVolts(volts) {

    var layout = getTimePlotLayout("Volts");

    limitMaxPlotPoint(voltsDateTimeList, voltsList);
    
    addToTimeSeries(volts, voltsDateTimeList, voltsList);

    var trace = getTrace("Voltage", voltsDateTimeList, voltsList);

    plot('voltsPlotArea', [ trace ], layout);

    return trace;
}


/**
 * @brief Get a radial guage instantiation.
 * @param renderCanvas
 * @param guageSize
 * @param title
 * @param units
 * @param maxValue
 * @param majorTickCount
 * @param decimalPlaces
 * @returns A RadialGuage object.
 */
function getRadialGauge(renderCanvas, guageSize, title, units, maxValue, majorTickCount, decimalPlaces) {
    var increment=maxValue/(majorTickCount-1);
    var tickValue=0;
    var maxList = [];
    for (i = 0; i < majorTickCount; i++) { 
        maxList.push(tickValue.toFixed(decimalPlaces));
        tickValue=tickValue+increment;
    }

    theGauge = new RadialGauge({
        renderTo: renderCanvas,
        width: guageSize,
        height: guageSize,
        title: title, 
        units: units,
        minValue: 0,
        maxValue: maxValue,
        majorTicks: maxList,
        minorTicks: 5,
        strokeTicks: true,
        highlights: [
            {
                "from": 0,
                "to": maxValue,
                "color": "rgba(157,172,181, .75)"
            }
        ],
    });
    return theGauge;
}

/**
 * @brief Init the watts guage.
 * @returns A RadialGuage object for measuring watts.
 */
function createWattsGauge() {
    wattsGauge = getRadialGauge('watts-gauge-canvas',guageSize, 'Power',"Watts", maxWatts, 7, 1 );
    wattsGauge.draw();
}

/**
 * @brief Init the amps guage.
 * @returns A RadialGuage object for measuring amps.
 */
function createAmpsGauge() {
    ampsGauge = getRadialGauge('amps-gauge-canvas',guageSize, 'Current',"Amps", maxAmps, 7, 1 );
    ampsGauge.draw();
}

/**
 * @brief Init the volts guage.
 * @returns A RadialGuage object for measuring volts.
 */
function createVoltsGauge() {
    voltsGauge = getRadialGauge('volts-gauge-canvas',guageSize, 'Voltage',"Volts", maxVolts, 7, 1 );
    voltsGauge.draw();
}


/**
 * @brief Update the view of the device.
 * @param updateConfig If True the the config is updated. False no config is updated.
 **/
function updateView(updateConfig) {
      uo.debug("updateView()");

      if( updateConfig ) {
          getConfig();
      }

      //PJA REMOVE
      plotWatts(0);
      plotAmps(0);
      plotVolts(0);
      
      createWattsGauge();
      createAmpsGauge();
      createVoltsGauge();
}

function getConfig() {
    uo.debug("getStatus()");
    $.ajax({
        url: '/rpc/get_config',
        success: function(data) {
            uo.debug("get_config:");
            console.dir(data);
            var unitName = data["unit_name"];
            if (unitName) {
                devNameField.value = unitName;
                titleLabel.innerHTML  = unitName;
            }
            var enableSyslog = data["syslog_enabled"];
            if( enableSyslog ) {
                enableSyslogCB.checked = true;
            }
            else {
                enableSyslogCB.checked = false;
            }
            var maxPowrPlotPoints = data["max_pp_count"];
            if (maxPowrPlotPoints) {
                maxPwrPlotPointsField.value = maxPowrPlotPoints;
            }
        }
    });
}


/**
 * @brief Called when a tab (in product.html) is selected.
 **/
function selectTab(evt, cityName) {
  uo.debug("selectTab()");
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
  	tabcontent[i].style.display = "none";
  }
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
  	tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  document.getElementById(cityName).style.display = "block";
  evt.currentTarget.className += " active";
}

/**
 * @brief Called when the set amps button is is selected.
 **/
function setTargetAmps() {
	uo.debug("setTargetAmps()");
	//PJA TODO
}

/**
 * @brief Called when the set watts button is is selected.
 **/
function setTargetWatts() {
	uo.debug("setTargetWatts()");
	//PJA TODO
}

/**
 * @brief Called when the set set config button is selected.
 **/
function setConfig() {
    uo.debug("setConfig()");

    var  devName = devNameField.value;
    var  groupName = devNameField.value;
    var _enable_syslog=0;
    if( enableSyslogCB.checked ) {
        _enable_syslog=1;
    }
    if( maxPwrPlotPointsField.value < 10) {
        alert("A minimum of 10 plot points must be set.");
        return;
    }
    var maxPPCount = maxPwrPlotPointsField.value;
    
    var jsonStr = JSON.stringify({
        dev_name: devName,
        group_name: groupName,
        enable_syslog:_enable_syslog,
        max_pp_count: maxPPCount,
   }, null, '\t');

   $.ajax({
        url: '/rpc/set_config',
        data: jsonStr,
        type: 'POST',
        success: function(data) {
        },
    })
    console.dir(jsonStr);

    updateView(true);
    updateView(true);

}

/**
 * @brief Called when the set set config button is selected.
 **/
function setFactoryDefaults() {
    uo.debug("setFactoryDefaults()");
    //PJA TODO
}


/**
 * @brief Called when the set set config button is selected.
 **/
function reboot() {
    uo.debug("reboot()");
}

/**
 * Called at intervals to update the table if required.
 * 
 * @returns null
 */
setInterval(function() {
    updateView(false); 
}, 1000);

/**
 * @brief Called when the page is loaded.
 **/
window.onload = function(e){
  	uo.debug("window.onload()");
	updateView(true);
  
   setAmpsButton.addEventListener("click", setTargetAmps);
   setWattsButton.addEventListener("click", setTargetWatts);

   setConfigButton.addEventListener("click", setConfig);
   factoryDefaultsButton.addEventListener("click", setFactoryDefaults);
   rebootButton.addEventListener("click", reboot);

}

//Select the required tab when the product.html page is loaded.
document.getElementById("defaultOpen").click();


