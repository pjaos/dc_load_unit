const WATS_PLOT_AREA_ID     = "wattsPlotArea";
const AMPS_PLOT_AREA_ID     = "ampsPlotArea";
const VOLTS_PLOT_AREA_ID    = "voltsPlotArea";
const TEMP_PLOT_AREA_ID     = "tempPlotArea";

const GUAGE_SIZE            = 250;
const MAX_FACTOR            = 1.8;

var setAmpsButton        = document.getElementById("set_current");
var setWattsButton       = document.getElementById("set_watts");

var setConfigButton = document.getElementById("setConfigButton");
var factoryDefaultsButton = document.getElementById("setDefaultsButton");
var rebootButton = document.getElementById("rebootButton");
var targetAmpsField = document.getElementById("targetAmps");

var titleLabel = document.getElementById("titleLabel");
var devNameField = document.getElementById("devNameText");
var groupNameField = document.getElementById("groupNameText");
var enableSyslogCB = document.getElementById("syslogEnableCheckbox");
var maxPwrPlotPointsField = document.getElementById("maxPwrPlotPoints");

var maxPlotPoints=10;
var firstGetStatsCallback = true;
var maxWatts=0;
var maxAmps=0;
var maxAmps=0;
var maxAmps=0;

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
   traceColour = 'rgb(0, 102, 0)';
    trace = {
            opacity : 0,
            name : traceName,
            x : xValueList,
            y : yValueList,
            fill : 'tozeroy',
            type : 'scatter',
            line : {
                color : traceColour,
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
        console.dir(err);
    }
}

/**
 * @param Add a value to the to a pre existing plot
 * @param plotIDTag The HTML ID tag of the plot canvas.
 * @param xAxisTime The time to record the temp value.
 * @param value The value on the Y axis to be plotted.
 * @returns
 */
function addPlotValue(plotIDTag, xAxisTime, value) {
    // As Plotly is to big to fit in the devices flash it is downloaded direct
    // and therefore may not be found so wrap in a try/catch block so the
    // javascript runs if Plotly is not available.
    try {
        Plotly.extendTraces(plotIDTag, {y: [[value]], x: [[xAxisTime]]}, [0], maxPlotPoints)
    } catch (err) {
        console.dir(err);
    }
}

/**
 * @brief Create the watts plot
 */
function createWattsPlot() {

    var layout = getTimePlotLayout("Watts");

    var trace = getTrace("Power", [], []);

    plot(WATS_PLOT_AREA_ID, [ trace ], layout);

    return trace;
}

/**
 * @brief Create the amps plot
 */
function createAmpsPlot() {

    var layout = getTimePlotLayout("Amps");

    var trace = getTrace("Current", [], []);

    plot(AMPS_PLOT_AREA_ID, [ trace ], layout);

    return trace;
}

/**
 * @brief Create the volts plot
 */
function createVoltsPlot() {

    var layout = getTimePlotLayout("Volts");

    var trace = getTrace("Voltage", [], []);

    plot(VOLTS_PLOT_AREA_ID, [ trace ], layout);

    return trace;
}

/**
 * @brief Create the temp C plot
 */
function createTempCPlot() {

    var layout = getTimePlotLayout("Centigrade");

    var trace = getTrace("Temperature", [], []);

    plot(TEMP_PLOT_AREA_ID, [ trace ], layout);

    return trace;
}

/**
 * @brief Get a radial guage instantiation.
 * @param renderCanvas
 * @param guageSize
 * @param title
 * @param units
 * @param majorTickCount
 * @param decimalPlaces
 * @param initialValue
 * @param maxValue
 * @return A RadialGuage object.
 */
function getRadialGauge(renderCanvas, guageSize, title, units, majorTickCount, decimalPlaces, initialValue, maxValue) {
    var maxValue;
    var increment;
    var tickValue=0;
    var maxList = [];
    
    if( maxValue < .1 ) {
        maxValue = 0.1;
    }
    increment = maxValue/(majorTickCount-1);
    
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
        value: initialValue,
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
function createWattsGauge(initialValue) {
    maxWatts = initialValue*MAX_FACTOR;
    var wattsGauge = getRadialGauge('watts-gauge-canvas',GUAGE_SIZE, 'Power',"Watts", 7, 1, initialValue, maxWatts);
    wattsGauge.draw();
    return wattsGauge;

}

/**
 * @brief Init the amps guage.
 * @returns A RadialGuage object for measuring amps.
 */
function createAmpsGauge(initialValue) {
    maxAmps = initialValue*MAX_FACTOR;
    var ampsGauge = getRadialGauge('amps-gauge-canvas',GUAGE_SIZE, 'Current',"Amps", 7, 1, initialValue, maxAmps);
    ampsGauge.draw();
    return ampsGauge;
}

/**
 * @brief Init the volts guage.
 * @returns A RadialGuage object for measuring volts.
 */
function createVoltsGauge(initialValue) {
    maxVolts = initialValue*MAX_FACTOR;
    var voltsGauge = getRadialGauge('volts-gauge-canvas',GUAGE_SIZE, 'Voltage',"Volts", 7, 1, initialValue, maxVolts);
    voltsGauge.draw();
    return voltsGauge;
}

/**
 * @brief Init the temp gauge.
 * @param , initialValue The initial value of the guage.
 * @returns A RadialGuage object for measuring temp in C.
 */
function createTempGauge(initialValue) {
    maxTemp = initialValue*MAX_FACTOR;
    var tempGauge = getRadialGauge('temp-gauge-canvas',GUAGE_SIZE, 'Temperature',"Centigrade", 7, 1, initialValue, maxTemp);
    tempGauge.draw();
    return tempGauge;
}

/**
 * @brief Init the cooling guage.
 * @param , initialValue The initial value of the guage.
 * @returns A RadialGuage object for measuring cooling (fan on count).
 */
function createCoolingGauge(initialValue) {
    var fanCount = 5;
    var coolingGauge = getRadialGauge('cooling-gauge-canvas',GUAGE_SIZE, 'Cooling',"Fans On", 6, 0, initialValue, fanCount);
    coolingGauge.draw();
    return coolingGauge;
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
            timeNow = getTimeNow();
            uo.debug("got stats");
            console.dir(data);
            var watts = data["watts"];
            var amps  = data["amps"];
            var volts = data["volts"];
            var tempC = data["temp_c"];
            var fanOnCount = data["fan_on_count"];
            
            if( firstGetStatsCallback ) {
                createWattsPlot();
                createAmpsPlot();
                createVoltsPlot();
                createTempCPlot();
                
                wattsGauge = createWattsGauge(watts);
                ampsGauge = createAmpsGauge(amps);
                voltsGauge = createVoltsGauge(volts);
                tempGauge = createTempGauge(tempC);
                coolingGuage = createCoolingGauge(0)
                
                firstGetStatsCallback = false;
            }
            else {
                addPlotValue(WATS_PLOT_AREA_ID, timeNow, watts);
                addPlotValue(AMPS_PLOT_AREA_ID, timeNow, amps);
                addPlotValue(VOLTS_PLOT_AREA_ID, timeNow, volts);
                addPlotValue(TEMP_PLOT_AREA_ID, timeNow, tempC);
                
                if( watts > maxWatts ) {
                    wattsGauge = createWattsGauge(watts);
                }
                wattsGauge.value=watts;
                

                if( amps > maxAmps ) {
                    ampsGauge = createAmpsGauge(amps);
                }
                else {
                    ampsGauge.value=amps;
                }

                if( volts > maxVolts ) {
                    voltsGauge = createVoltsGauge(volts);
                }
                else {
                    voltsGauge.value=volts;
                }
                
                if( tempC > maxTemp ) {
                    tempGauge = createTempGauge(tempC);
                }
                tempGauge.value=tempC;
                
                coolingGuage.value=fanOnCount;
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

	var targetAmps = targetAmpsField.value;
	if( targetAmps > 20 ) {
        alert("The maximum target current is 20 amps.");
        return;
	}
    
    var jsonStr = JSON.stringify({
        amps: targetAmps,
   }, null, '\t');

   $.ajax({
        url: '/rpc/target_amps',
        data: jsonStr,
        type: 'POST',
        success: function(data) {
        },
    })

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
   //Set target amps if user presses return in target amps field
   targetAmpsField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setTargetAmps();
       }
   });

   setWattsButton.addEventListener("click", setTargetWatts);

   setConfigButton.addEventListener("click", setConfig);
   factoryDefaultsButton.addEventListener("click", setFactoryDefaults);
   rebootButton.addEventListener("click", reboot); 

}

//Select the required tab when the product.html page is loaded.
document.getElementById("defaultOpen").click();


