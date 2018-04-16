
const https = require("https");
const DARKSKY_KEY = "197172f19af1baa7e50089fa303099d0";

const client = require('/usr/lib/node_modules/socket.io-client');
const local_service_PORT = 64000;
var latitude, longitude, city, country, temperature, sunset, sunrise;
var DEVICE_ID = 0;
var weather;

function datelog(message) {
    console.log(Date.now() + ": " + message);
}

datelog("  Weather service");
getGeoloc();

var local_service = client("http://127.0.0.1:" + local_service_PORT);
local_service.on('connect', function () {

    datelog(" * WEATHER is connected");
    // The lib will create a client connection to the local_service, namely
    // this url : localhost:local_service_PORT - it does not need any info

});
local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "weather");
    weather = setInterval(function () {
        datelog("Weather update");
        if(getWeather(latitude, longitude,city, country) == true)
        {
            sendWeatherInfo();
        }
    }, 10 * 100 * 60);

});


local_service.on('disconnect', function () {
    datelog('** Disconnected from local service ');
    stop_run();
});

local_service.on('reconnect', function () {
    datelog('** Reconnected to local service ');
});

local_service.on('connect_error', function (error) {
    datelog('** Connection error from local service ');
    stop_run();
});

local_service.on('connect_timeout', function () {
    datelog('** Connection timeout from local service');
});

local_service.on('reconnect_failed', function () {
    datelog('** Reconnection failed to local service ');
    stop_run();
});

local_service.on('reconnect_error', function (error) {
    datelog('** Reconnection error from  local service ');
    stop_run();
});

function getGeoloc() {
    https.get("https://www.freegeoip.net/json/", function (res) {
        var body = '';
        res.on('data', function (chunk) {
            body += chunk;
        });
        res.on('end', function () {
            var data = JSON.parse(body);
            latitude = data.latitude;
            longitude = data.longitude;
            city = data.city;
            country = data.country_name;
            datelog("Geoloc started. Located in " + data.city + ": lat" + data.latitude + "/ long :" + data.longitude);
            getWeather(data.latitude, data.longitude, city, country);
        });
    }).on('error', function (e) {
        datelog("Got an error: ", e);
    });
}

function getWeather(lat, lon, city, country) {
    if (country == "France") {
        https.get('https://www.prevision-meteo.ch/services/json/' + city, function (res) {
            var body = '';
            res.on('data', function (chunk) {
                body += chunk;
            });
            res.on('end', function () {
                var data = JSON.parse(body);
                datelog("Weather info : Temp : " + data.current_condition.tmp + ": summary : " + data.current_condition.condition);
                datelog("Weather info : Sunrise : " + data.city_info.sunrise + ": Sunset : " + data.city_info.sunset);
                sunset = data.city_info.sunset;
                sunrise = data.city_info.sunrise;
                temperature = data.current_condition.tmp;
                return true;
            });
        }).on('error', function (e) {
            datelog("Got an error: ", e);
            return false;
        });
    }
    /*
    if (lat && lon) {
        https.get('https://api.darksky.net/forecast/' + DARKSKY_KEY + '/' + lat + ',' + lon + '?exclude=[minutely,daily,alerts,flags]&units=si', function (res) {
            var body = '';
            res.on('data', function (chunk) {
                body += chunk;
            });
            res.on('end', function () {
                var data = JSON.parse(body);
                datelog("Weather info : cloudCover : " + data.cloudCover + ": summary : " + data.summary);
                return true;
            });
        }).on('error', function (e) {
            datelog("Got an error: ", e);
            return false;
        });
    }*/
    return false;
}

function sendWeatherInfo() {
    var msg2send = "{}";

    if (temperature) {
        msg2send = JSON.stringify({
            device_id: DEVICE_ID.toString(),
            sensor_type: "outdoor_air_temperature",
            sensor_value: temperature.toString(),
        });

        datelog(msg2send);
        local_service.emit("data", msg2send);
    }
    if (sunset) {
        msg2send = JSON.stringify({
            device_id: DEVICE_ID.toString(),
            sensor_type: "sunset",
            sensor_value: sunset,
        });

        datelog(msg2send);
        local_service.emit("data", msg2send);
    }
    if (sunrise) {
        msg2send = JSON.stringify({
            device_id: DEVICE_ID.toString(),
            sensor_type: "sunrise",
            sensor_value: sunrise,
        });

        datelog(msg2send);
        local_service.emit("data", msg2send);
    }
}

function stop_run() {
    if (weather) clearInterval(weather);
}

