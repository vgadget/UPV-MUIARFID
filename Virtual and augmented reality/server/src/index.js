'use strict';

// Modules
require("dotenv").config();
const express = require('express');

const WebSocket = require('ws')
const networkModule = require('./dev/networkModule.js');
const enc = new TextDecoder("utf-8");

// Server
const server = express();

// Settings
server.set('port', process.env.PORT || 8080); // SET PORT

const wss = new WebSocket.Server({ port: server.get('port') },()=>{
  console.log('Server Started');
});

wss.broadcast = function broadcast(from, msg){
  wss.clients.forEach(function each(client){
    if (from != client){
      client.send(msg);
    }
  });
};

wss.on('connection', function connection(ws) {

  ws.on('message', (rawData) => {
    var data = enc.decode(new Uint8Array(rawData));
     console.log('Data received: ', data);
     wss.broadcast(ws, data);
  });
});

wss.on('listening',()=>{
  console.log('Listening on ' + server.get('port'));
});

networkModule.getIP();



networkModule.getIP();