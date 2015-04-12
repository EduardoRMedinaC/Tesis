var express = require('express'),
	app = express(),
	server = require('http').createServer(app),
	io = require('socket.io')(server),
	mongoclient = require('mongodb').MongoClient,
	assert = require('assert'),
	bodyParser = require('body-parser'),
	parseUrlencoded = bodyParser.urlencoded({ extended: false });

//mongodb url server
var url = 'mongodb://localhost:27017/data';

mongoclient.connect(url, {server: {poolSize: 3}}, function(err, db){
	assert.equal(err, null, ['No pudo conectarse a la base de datos']);
	console.log('correctamente conectado con la base de datos');

	//conexion con el socket cliente
	io.on('connection', function(client){
		console.log("conexion establecido con el cliente");

		//se recibe el perfil de la casa
		client.on('perfil', function(data){
			insertDocument(db, 'hogares', data);
		});

		db.collection('hogares').find({}).toArray(function(err, docs){
			console.log(docs);
		}); 
	});
	
});

server.listen(80, function(){
	console.log("tu servidor está listo en " + this.address().port);
});
