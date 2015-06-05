var express = require('express'),
	app = express(),
	server = require('http').createServer(app),
	io = require('socket.io')(server),
	mongoClient = require('mongodb').MongoClient,
	crud = require('./crud/mongo-crud'),
	assert = require('assert');

//mongodb url server
var url = 'mongodb://localhost:27017/data';

mongoClient.connect(url, {server: {poolSize: 3}}, function(err, db){
	assert.equal(err, null, ['No pudo conectarse a la base de datos']);

	//conexion con el socket cliente
	io.on('connection', function(client){
		console.log("conexion establecido con el cliente");

		//se recibe el perfil de la casa con la medicion
		client.on('home', function(data){ 
			crud.updateHomes(db, data, function(reference){

				//creamos una referecia normalizada para la
				//coleccion records
				data.meassure.home_id = reference;

				crud.insertMeassure(db, data.meassure);
			});
		});
	});

	app.use(express.static('../public'));

	var zonas = require('./routes/zonas');
	app.use('/zona', zonas);
});

server.listen(80, function(){
	console.log("tu servidor está listo en " + this.address().port);
});
