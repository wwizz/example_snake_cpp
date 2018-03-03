/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

function Streamer(updateLightState, notify){
    var that = this;
    var updateFrequency = 50;
    var lightDataSize = 9;
    that.streamData = [];
    var socket = io();
    var timer;
    var animation = ['/', '-', '\\', '|', '/', '-', '\\', '|'];
    var animantionCount = 0;
    notify("idle")
    that.Start = function(locations) {
        var location_count = 0;

        $.each(locations, function (id, pos) {
            location_count++;
        });

        var bytes_needed = location_count * lightDataSize;
        console.log("start streamer for " + location_count + " locations, expected message size: " + bytes_needed + " bytes");

        timer = setInterval(function () {
            if (!that.streamData) {
                notify("idle")
                return;
            }

            if (that.streamData.length === 0) {
                notify("idle")
                return;
            }

            if (bytes_needed !== that.streamData.length) {
                notify("error (data size mismatch)")
                console.log("Error: not right amount of streaming data received! (needed: " + bytes_needed + ", received: " + that.streamData.length + ")")
                return;
            }



            notify("streaming " + animation[animantionCount++ % animation.length]);
            var i = 0;
            for (var c = 0; c < location_count; c++) {
                var addressType = that.streamData[i];

                if (addressType === 0) {
                    var id = (that.streamData[i + 1] << 8) | (that.streamData[i + 2]);
                    var r = Math.round((((that.streamData[i + 3] << 8) | (that.streamData[i + 4])) / 65535.0) * 255);
                    var g = Math.round((((that.streamData[i + 5] << 8) | (that.streamData[i + 6])) / 65535.0) * 255);
                    var b = Math.round((((that.streamData[i + 7] << 8) | (that.streamData[i + 8])) / 65535.0) * 255);
                    updateLightState(id, r, g, b);
                } else {
                    console.log("warning: unsupported address type used");
                }

                i += lightDataSize;
            }
            that.streamData = undefined;
        }, updateFrequency);
    };

    that.Stop = function(){
        clearInterval(timer);
    };


    socket.on('streamData', function (msg) {
        if (msg.colormode === 0) {
            that.streamData = msg.data;
        }
        else {
            console.log("warning: unsupported colormode used");
        }
    });
}



