/*---------------------------------------------------------------------------------------------
 *  Copyright © 2016-present Earth Computing Corporation. All rights reserved.
 *  Licensed under the MIT License. See LICENSE.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
var express = require('express');
var path = require('path');
var app = express();
var io = require('socket.io');

var server = require('http').Server(app);
var bodyParser = require('body-parser');

app.use('/', express.static(path.join(__dirname, '/')));
app.use(bodyParser.json());

server.listen(3000);
var l = io.listen(server);
var data = {"deviceName": "enp6s0",
	    "linkState": "DOWN",
	    "entlState":"IDLE",
	    "entlCount":"0",
	    "AITMessage":"-none-"
	   };

app.get('/', function (req, res) {
	res.sendFile('/index.html');
    });


app.put('/earthUpdate', function (req, res) {
    earthUpdate(l.sockets,req.body);
    res.end();
});


l.sockets.on("connection", function(socket) {
    earthUpdate(socket, data);
    //    enp2s0Update(socket, data);
    
    socket.on("aitMessage", function(data){
	console.log("in AIT handler");
	earthAITMessage(socket, data);
});
    
});

// update function for tcp-connect update
function enp2s0Update (socket, data) {
    console.log(data);
    //console.log(socket);
    socket.emit("enp2s0-update", data);
}

//update function for earth-connect update
function earthUpdate (socket, data) {
    console.log(data);
    socket.emit("earth-update", data);
} 


function earthAITMessage(socket, data) {
    console.log(data);
    // send Message to NAL using AIT
    //socket.emit("earth-ait-done", data);
    
}
