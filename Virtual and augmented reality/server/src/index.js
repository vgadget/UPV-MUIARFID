'use strict';

// Modules
require("dotenv").config();
const express = require('express');

const socketIO = require('socket.io');
const networkModule = require('./dev/networkModule.js');

// Server
const server = express();

// Settings
server.set('port', process.env.PORT || 8080); // SET PORT


// START SERVER
const server_listener = server.listen(server.get('port'), () => {
  console.log("Server started on port: " + server.get('port') + ": ");
});

//Socket settings online multiplayer
const online_socket = socketIO(server_listener);
require('./multiplayer-socket.js')(online_socket);

networkModule.getIP();