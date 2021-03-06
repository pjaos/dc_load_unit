const DEBUG                 = false;
const WATS_PLOT_AREA_ID     = "wattsPlotArea";
const AMPS_PLOT_AREA_ID     = "ampsPlotArea";
const VOLTS_PLOT_AREA_ID    = "voltsPlotArea";
const TEMP_PLOT_AREA_ID     = "tempPlotArea";
const DIAL_TICK_COUNT_6     = 6;
const DIAL_DECIMAL_PLACES   = 1;
const MAX_AMPS              = 10.0;
const MAX_WATTS             = 200;

const GUAGE_SIZE            = 250;
const MAX_FACTOR            = 1.8;

var setAmpsButton        = document.getElementById("set_current");
var resetAmpsButton        = document.getElementById("reset_current");
var resetAudioAlarmButton = document.getElementById("reset_audio_alarm");
var audioAlarmDiv = document.getElementById("audio_alarm_div");

var setConfigButton = document.getElementById("setConfigButton");
var factoryDefaultsButton = document.getElementById("setDefaultsButton");
var rebootButton = document.getElementById("rebootButton");
var targetAmpsField = document.getElementById("targetAmps");

var titleLabel = document.getElementById("titleLabel");
var devNameField = document.getElementById("devNameText");
var groupNameField = document.getElementById("groupNameText");
var enableSyslogCB = document.getElementById("syslogEnableCheckbox");
var maxPwrPlotPointsField = document.getElementById("maxPwrPlotPoints");
var loadShutdownVoltageField = document.getElementById("loadShutdownVoltage");
var loadOnTimeField = document.getElementById("loadOnTime");
var lastLoadOnTimeField = document.getElementById("lastLoadOnTime");

var maxPlotPoints=10;
var firstGetStatsCallback = true;
var wattsGauge;
var ampsGauge;
var voltsGauge;
var tempGauge;
var coolingGauge;
var ampHoursGuage;
var wattHoursGuage;
var lastAmpHoursGuage;
var lastWattHoursGuage;

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
var uo = new UO(DEBUG);

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
        if( DEBUG ) {
            console.dir(err);
        }
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
        if( DEBUG ) {
            console.dir(err);
        }
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
 * @brief Create and return an instance of a new dial guage. 
 * @param renderCanvas The canvas ni the html page to draw the guage on.
 * @param guageSize The size of the guage.
 * @param title The guage title text.
 * @param units The guage unit text.
 * @param majorTickCount The number of major ticks around the guage.
 * @param initialValue The initial value to be displayed.
 * @param maxValue The max value for the guage.
 * @returns 
 */
function getRadialGauge(renderCanvas, guageSize, title, units, majorTickCount, initialValue, maxValue) {
    var increment;
    var tickValue=0;
    var maxList = [];
    
    maxValue = Math.ceil(maxValue);
    if( maxValue < 1 ) {
        maxValue = 1;
    }
    increment = maxValue/(majorTickCount-1);
    
    for (i = 0; i < majorTickCount; i++) { 
        maxList.push(tickValue.toFixed(DIAL_DECIMAL_PLACES));
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
        strokeTicks: true,
        value: initialValue,
        highlights: false,
    });
    return theGauge;
}

/**
 * @brief Update the guage value once created.
 * @param guage The guage instance.
 * @param value The value to be displayed.
 * @returns 
 */
function updateGauge(guage, value) {
    if( value > guage.options.maxValue ) {
        var maxValue = Math.ceil( MAX_FACTOR*value );
        var majorTickCount = DIAL_TICK_COUNT_6;
        var increment;
        var tickValue=0;
        var maxList = [];
        
        if( maxValue < .1 ) {
            maxValue = 0.1;
        }
        increment = maxValue/(majorTickCount-1);
        
        for (i = 0; i < majorTickCount; i++) { 
            maxList.push(tickValue.toFixed(DIAL_DECIMAL_PLACES));
            tickValue=tickValue+increment;
        }
        guage.value = value;
        guage.options.maxValue = maxValue;
        guage.options.majorTicks = maxList;
        guage.update();
    }
    else {
        guage.value = value;
    }
}

/**
 * @brief Init the amps gauge.
 * @returns A RadialGuage object for measuring amps.
 */
function createAmpsGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 1;
    }
    var guage = getRadialGauge('amps-gauge-canvas',GUAGE_SIZE, 'Current',"Amps", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the volts gauge.
 * @returns A RadialGuage object for measuring volts.
 */
function createVoltsGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 10;
    }
    var guage = getRadialGauge('volts-gauge-canvas',GUAGE_SIZE, 'Voltage',"Volts", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the watts gauge.
 * @returns A RadialGuage object for measuring watts.
 */
function createWattsGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 5;
    }
    var guage = getRadialGauge('watts-gauge-canvas',GUAGE_SIZE, 'Power',"Watts", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;

}

/**
 * @brief Init the temp gauge.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object for measuring temp in C.
 */
function createTempGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 30;
    }
    var guage = getRadialGauge('temp-gauge-canvas',GUAGE_SIZE, 'Temperature',"Centigrade", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the cooling guage.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object for measuring cooling (fan on count).
 */
function createCoolingGauge(initialValue) {
    var fanCount = 5;
    var guage = getRadialGauge('cooling-gauge-canvas',GUAGE_SIZE, 'Cooling',"Fans On", DIAL_TICK_COUNT_6, initialValue, fanCount);
    guage.draw();
    return guage;
}

/**
 * @brief Init the amp hours guage.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object.
 */
function createAmpsHoursGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 1;
    }
    var guage = getRadialGauge('amp-hours-gauge-canvas',GUAGE_SIZE, 'Amp Hours',"Ah", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the watt hours guage.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object.
 */
function createWattHoursGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 1;
    }
    var guage = getRadialGauge('watt-hours-gauge-canvas',GUAGE_SIZE, 'Watt Hours',"Wh", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the last amp hours guage.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object.
 */
function createLastAmpsHoursGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 1;
    }
    var guage = getRadialGauge('last_amp-hours-gauge-canvas',GUAGE_SIZE, 'Last Amp Hours',"Ah", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
}

/**
 * @brief Init the last watt hours guage.
 * @param , initialValue The initial value of the gauge.
 * @returns A RadialGuage object.
 */
function createLastWattHoursGauge(initialValue) {
    var maxValue = initialValue*MAX_FACTOR;
    if( maxValue == 0 ) {
        maxValue = 1;
    }
    var guage = getRadialGauge('last-watt-hours-gauge-canvas',GUAGE_SIZE, 'Last Watt Hours',"Wh", DIAL_TICK_COUNT_6, initialValue, maxValue);
    guage.draw();
    return guage;
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
 * @brief Handle a temp alarm.
 * @returns
 */
function handleTempAlarm() {
    alert("!!! Maximum temperature reached and load turned off !!!");
    //Reset the temp alarm on the dc load unit.
    $.ajax({
        url: '/rpc/reset_temp_alarm',
        type: 'POST',
        success: function(data) {
        },
    })
}

/**
 * @brief Handle min load voltage alarm.
 * @returns
 */
function handleMinLoadVoltageAlarm() {
    alert("Load turned off as the load voltage is below "+loadShutdownVoltageField.value+" volts.");
    //Reset the min voltage alarm on the dc load unit.
    $.ajax({
        url: '/rpc/reset_min_load_voltage_alarm',
        type: 'POST',
        success: function(data) {
        },
    })
}


/**
 * @brief Handle max load voltage alarm.
 * @returns
 */
function handleMaxLoadVoltageAlarm() {
    alert("Turned off as the load current as the voltage is above the maximum value of 200 volts.\n\n!!! Reduce the load voltage or damage to the DC Load Unit may occur.");
    //Reset the max voltage alarm on the dc load unit.
    $.ajax({
        url: '/rpc/reset_max_load_voltage_alarm',
        type: 'POST',
        success: function(data) {
        },
    })
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
            if( DEBUG ) {
                console.dir(data);
            }

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

            //Not used
            var targetWatts = data["target_watts"];
            loadShutdownVoltageField.value =  data["load_off_voltage"];

        }
    });
}

/**
 * @brief Update the time in the time field.
 * @param loadOnTimeField The input field.
 * @param secs The number of seconds that have passed.
 * @returns
 */
function updateTime(loadOnTimeField, secs) {
    if( secs > 0 ) {
        var h = Math.floor(secs / 3600);
        var m = Math.floor(secs % 3600 / 60);
        var s = Math.floor(secs % 3600 % 60);
        var fH = ("0" + h).slice(-2);
        var fM = ("0" + m).slice(-2);
        var fS = ("0" + s).slice(-2);
        loadOnTimeField.innerHTML = fH+":"+fM+":"+fS;
    }
    else {
        loadOnTimeField.innerHTML = "00:00:00";
    }
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
            if( DEBUG ) {
                console.dir(data);
            }
            var amps  = data["amps"];
            var watts = data["watts"];
            var volts = data["volts"];
            var tempC = data["temp_c"];
            var fanOnCount = data["fan_on_count"];
            var ampHours = data["amp_hours"];
            var wattHours = data["watt_hours"];
            var lastAmpHours = data["previous_amp_hours"];
            var lastWattHours = data["previous_watt_hours"];
            var loadOnSecs = data["load_on_secs"]
            var previousLoadOnSecs = data["previous_load_on_secs"]

            if( firstGetStatsCallback ) {
                createAmpsPlot();
                createVoltsPlot();
                createWattsPlot();
                createTempCPlot();

                ampsGauge = createAmpsGauge(amps);
                voltsGauge = createVoltsGauge(volts);
                wattsGauge = createWattsGauge(watts);
                tempGauge = createTempGauge(tempC);
                coolingGauge = createCoolingGauge(fanOnCount);
                ampHoursGuage = createAmpsHoursGauge(ampHours);
                wattHoursGuage = createWattHoursGauge(wattHours);
                lastAmpHoursGuage = createLastAmpsHoursGauge(lastAmpHours);
                lastWattHoursGuage = createLastWattHoursGauge(lastWattHours);
    
                firstGetStatsCallback = false;
            }
            else {
                addPlotValue(AMPS_PLOT_AREA_ID, timeNow, amps);
                addPlotValue(VOLTS_PLOT_AREA_ID, timeNow, volts);
                addPlotValue(WATS_PLOT_AREA_ID, timeNow, watts);
                addPlotValue(TEMP_PLOT_AREA_ID, timeNow, tempC);

                updateGauge(ampsGauge, amps);
                updateGauge(voltsGauge, volts);
                updateGauge(wattsGauge, watts);
                updateGauge(tempGauge, tempC);
                updateGauge(coolingGauge, fanOnCount);
                updateGauge(ampHoursGuage, ampHours);
                updateGauge(wattHoursGuage, wattHours);
                updateGauge(lastAmpHoursGuage, lastAmpHours);
                updateGauge(lastWattHoursGuage, lastWattHours);
                
                updateTime(loadOnTimeField, loadOnSecs);
                updateTime(lastLoadOnTimeField, previousLoadOnSecs);
            }
            
            
            var tempAlarm = data["temp_alarm"];
            if( tempAlarm ) {
                handleTempAlarm();
            }

            var minLoadVoltageAlarm = data["min_load_voltage_alarm"];
            if( minLoadVoltageAlarm ) {
                handleMinLoadVoltageAlarm();
            }

            var maxLoadVoltageAlarm = data["max_load_voltage_alarm"];
            if( maxLoadVoltageAlarm ) {
                handleMaxLoadVoltageAlarm();
            }
            
            var audioAlarm = data["audio_alarm"];
            if( audioAlarm != 0 ) {
                audioAlarmDiv.style.display = "block";
            }
            else {
                audioAlarmDiv.style.display = "none";
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
	if( voltsGauge.value == 0 ) {
        alert("Unable to set load current as no voltage detected on DC load input.");
        return;	    
	}
	var targetAmps = targetAmpsField.value;
	if( targetAmps > MAX_AMPS ) {
        alert("The maximum target current is "+MAX_AMPS+" amps.");
        return;
	}
	
	var expectedWatts = voltsGauge.value * targetAmps;
	if ( expectedWatts > MAX_WATTS ){
        alert("This would set the load to "+expectedWatts+" watts (max watts = "+MAX_WATTS+").");
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
 * @brief Reset an audio alarm on the dc load unit.
 **/
function resetAudioAlarm() {
    var jsonStr = JSON.stringify({
        on: 0,
    }, null, '\t');

    $.ajax({
        url: '/rpc/set_audio_alarm',
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
    var  groupName = groupNameField.value;
    var _enable_syslog=0;
    var  shutDownVoltage = loadShutdownVoltageField.value;
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
        load_off_voltage: shutDownVoltage,
   }, null, '\t');

   $.ajax({
        url: '/rpc/set_config',
        data: jsonStr,
        type: 'POST',
        success: function(data) {
        },
    })
    if( DEBUG ) {
        console.dir(jsonStr);
    }

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
  
   setAmpsButton.addEventListener("click", function(event) {
   if( targetAmpsField.value <= 0 ) {
       alert("Target current must be greater than 0 to turn on load.");
   }
   else {
       setTargetAmps();
   }
   });
   //Set target amps if user presses return in target amps field
   targetAmpsField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setTargetAmps();
       }
   });
   resetAmpsButton.addEventListener("click", function(event) {
       targetAmpsField.value=0;
       setTargetAmps();
   });

   resetAudioAlarmButton.addEventListener("click", function(event) {
       resetAudioAlarm();
   });

   devNameField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setConfig();
       }
   });
   groupNameField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setConfig();
       }
   });
   maxPwrPlotPointsField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setConfig();
       }
   });
   loadShutdownVoltageField.addEventListener("keypress", function(event) {
       if (event.keyCode == 13) {
           setConfig();
       }
   });

   setConfigButton.addEventListener("click", setConfig);
   factoryDefaultsButton.addEventListener("click", setFactoryDefaults);
   rebootButton.addEventListener("click", reboot); 

}

//Select the required tab when the product.html page is loaded.
document.getElementById("defaultOpen").click();


