const TEMP_PLOT_AREA_ID = "tempPlotArea";

var devNameLabel         = document.getElementById("devNameLabel");
var setAmpsButton        = document.getElementById("set_current");
var setWattsButton       = document.getElementById("set_watts");

var setConfigButton = document.getElementById("setConfigButton");
var factoryDefaultsButton = document.getElementById("setDefaultsButton");
var rebootButton = document.getElementById("rebootButton");
var targetAmpsField = document.getElementById("targetAmps");
var targetWattsField = document.getElementById("targetWatts");

var titleLabel = document.getElementById("titleLabel");
var devNameField = document.getElementById("devNameText");
var groupNameField = document.getElementById("groupNameText");
var enableSyslogCB = document.getElementById("syslogEnableCheckbox");
var maxPwrPlotPointsField = document.getElementById("maxPwrPlotPoints");

var maxPlotPointsIPF     = document.getElementById("maxPwrPlotPoints");

const guageSize    = 250;

var maxWatts=1;
var wattsGauge = null;

var maxAmps = 1;
var ampsGauge = null;

var maxVolts = 1;
var voltsGauge = null;

var maxTemp = 1;
var tempGauge = null;

var firstPlot = true;
var maxPlotPoints=10;

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
 * @brief Get the time now in a string that can be used as the X 
 * axis by plotly.
 * @returns
 */
function getTimeNow() {
    var now = new Date();
    var hours = now.getHours();
    var minutes = now.getMinutes();
    var seconds = now.getSeconds();
    var tenthSeconds = now.getMilliseconds() / 100;
    var formattedDateTime = "" + hours.toString().padStart(2, "0") + ":"
            + minutes.toString().padStart(2, "0") + ":" + seconds;
    return formattedDateTime;
}

/**
 * @brief Returns a random integer between min (inclusive) and max (inclusive). The
 * value is no lower than min (or the next integer greater than min if min isn't
 * an integer) and no greater than max (or the next integer lower than max if
 * max isn't an integer).
 */
function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min + 1)) + min;
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
 * @brief Create the watts plot
 */
function createWattsPlot() {

    var layout = getTimePlotLayout("Watts");

    var trace = getTrace("Power", [], []);

    plot('wattsPlotArea', [ trace ], layout);

    return trace;
}

/**
 * @brief Create the amps plot
 */
function createAmpsPlot() {

    var layout = getTimePlotLayout("Amps");

    var trace = getTrace("Current", [], []);

    plot('ampsPlotArea', [ trace ], layout);

    return trace;
}

/**
 * @brief Create the volts plot
 */
function createVoltsPlot() {

    var layout = getTimePlotLayout("Volts");

    var trace = getTrace("Voltage", [], []);

    plot('voltsPlotArea', [ trace ], layout);

    return trace;
}

/**
 * @brief Create the temp C plot
 */
function createTempCPlot() {

    var layout = getTimePlotLayout("C");

    var trace = getTrace("Temperature", [], []);

    plot(TEMP_PLOT_AREA_ID, [ trace ], layout);

    return trace;
}
/**
 * @param Add a value to the to a pre existing plot
 * @param plotIDTag The HTML ID tag of the plot canvas.
 * @param xAxisTime The time to record the temp value.
 * @param value The value on the Y axis to be plotted.
 * @returns
 */
function addPlotValue(plotIDTag, xAxisTime, value) {
    Plotly.extendTraces(plotIDTag, {y: [[value]], x: [[xAxisTime]]}, [0], maxPlotPoints)
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
 * @brief Init the temp gauge guage.
 * @returns A RadialGuage object for measuring temp in C.
 */
function createTempGauge() {
    tempGauge = getRadialGauge('temp-gauge-canvas',guageSize, 'Temperature',"C", maxTemp, 7, 1 );
    tempGauge.draw();
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
      
      getStats();

}

/**
 * @brief Get the config params from the device.
 * @returns 
 */
function getConfig() {
    uo.debug("getConfig()");
    $.ajax({
        url: '/rpc/get_config',
        success: function(data) {
            uo.debug("got config");
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
            
            maxPlotPoints = data["max_pp_count"];
            if (maxPlotPoints) {
                maxPwrPlotPointsField.value = maxPlotPoints;
            }
            
            var targetAmps = data["target_amps"];
            targetAmpsField.value = targetAmps;
            
            var targetWatts = data["target_watts"];
            targetWattsField.value = targetWatts;

        }
    });
}

/**
 * @brief Get the stats (amps, volts etc) from the device.
 * @returns 
 */
function getStats() {
    uo.debug("getStats()");
    
    $.ajax({
        url: '/rpc/get_stats',
        success: function(data) {
            uo.debug("got stats");
            console.dir(data);
            formattedDateTime = getTimeNow();
            var amps = data["amps"];
/*
            if( firstPlot ) {
                createAmpsPlot();
            }
            addPlotValue('ampsPlotArea', formattedDateTime, amps);
*/
            if( firstPlot || maxAmps < amps ) {
                maxAmps = amps+1;
                createAmpsGauge();
            }
            ampsGauge.value=amps;

            var watts = data["watts"];
/*
            if( firstPlot ) {
                createWattsPlot();
            }
            addPlotValue('wattsPlotArea', formattedDateTime, watts);
*/
            if( firstPlot || maxWatts < watts ) {
                maxWatts = watts+5;
                createWattsGauge();
            }
            wattsGauge.value=watts;

            var volts = data["volts"];
            /*
            if( firstPlot ) {
                createVoltsPlot();
            }
            addPlotValue('voltsPlotArea', formattedDateTime, volts);
*/
            if( firstPlot  || maxVolts < volts ) {
                maxVolts = volts+5;
                createVoltsGauge();
            }
            voltsGauge.value=volts;

            var tempC = data["temp_c"];
/*
            if( firstPlot ) {
                createTempCPlot();
            }
            addPlotValue(TEMP_PLOT_AREA_ID, formattedDateTime, tempC);
*/
            if( firstPlot || maxTemp < tempC ) {
                maxTemp = tempC+5;
                createTempGauge();
            }
            tempGauge.value=tempC;
            
            firstPlot = false;
        }
    });
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
    if( confirm("Are you sure that you wish to set the device configuration to factory defaults and reboot ?") ) {
        $.ajax({
        url: '/rpc/factorydefault',
        type: 'POST',
        success: function(data) {
          uo.debug("Set factory default config success");
          alert("The device is now rebooting.");
          document.body.style.cursor = 'wait';
          setTimeout( location.reload() , 1000);
        },
      })
    }
}

/**
 * @brief Called when the set set config button is selected.
 **/
function reboot() {
    uo.debug("reboot()");
    if( confirm("Are you sure that you wish to reboot the device ?") ) {
        $.ajax({
          url: '/rpc/reboot',
          type: 'POST',
          success: function(data) {
            uo.debug("Reboot success");
            alert("The device is now rebooting.");
            document.body.style.cursor = 'wait';
            setTimeout( location.reload() , 1000);
          },
        })
    }
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


