var express = require('express');
var path = require('path');
var app = express();
var io = require('socket.io');

var server = require('http').Server(app);

app.use('/', express.static(path.join(__dirname, '/')));

server.listen(3000);
var l = io.listen(server);
var data = {'linkState': 'link-down'};

app.get('/', function (req, res) {
	res.sendFile('/index.html');
    });

// tcp connection state
app.put('/enp2s0Up', function (req, res) {
    data = {'linkState': 'link-up'};
    enp2s0Update(l.sockets, data);
    res.end();
});

app.put('/enp2s0Wait', function (req, res) {
    data = {'linkState': 'link-wait'};
    enp2s0Update(l.sockets, data);
    res.end();
});

app.put('/enp2s0Down', function (req, res) {
    data = {'linkState': 'link-down'};
    enp2s0Update(l.sockets, data);
    res.end();
});

// earth connection state
app.put('/earthUp', function (req, res) {
    data = {'linkState': 'link-up'};
    earthUpdate(l.sockets, data);
    res.end();
});

app.put('/earthWait', function (req, res) {
    data = {'linkState': 'link-wait'};
    earthUpdate(l.sockets, data);
    res.end();
});

app.put('/earthDown', function (req, res) {
    data = {'linkState': 'link-down'};
    earthUpdate(l.sockets, data);
    res.end();
});


l.sockets.on("connection", function(socket) {
    earthUpdate(socket, data);
    enp2s0Update(socket, data);
    
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

