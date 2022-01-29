'use strict';


//CALLED DIRECTLY ON INDEX.JS

function listen(websocket) {

    websocket.on('connection', function (clientWS) {

        clientWS.on('message', (message) => {

            console.log(message)
            clientWS.broadcast.emit("coordinates", "Hola mundo"); 
        });

        console.log("Nueva conexion!");

    });
}

module.exports = listen;