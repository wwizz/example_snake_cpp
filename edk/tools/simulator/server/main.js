var simulatorServer = require('./simulator-server');
var streamsrv = require('./streamer-server');
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var server = new simulatorServer(app, express, streamsrv, io);

process.env.PORT = 80;
process.env.IP = '127.0.0.1';

http.listen(process.env.PORT, process.env.IP, function(){
  console.log('webserver listening on *:' + process.env.PORT);
});

