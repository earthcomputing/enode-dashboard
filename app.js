var express = require('express');
var io = require('socket.io')(server);
var path = require('path');
var app = express();
var server = require('http').Server(app);

app.use('/', express.static(path.join(__dirname, '/')));

server.listen(3000);

app.get('/', function (req, res) {
	res.sendFile('/index.html');
    });

io.on('connection', function (socket) {
	socket.emit('hello', { hello: 'world' });
	socket.on('another event', function (data) {
		console.log("data:"+data+" socket id: "+socket.id);
	    });
    });

