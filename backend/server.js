var express = require('express'),
	app = express(),
	server = require('http').createServer(app),
	io = require('socket.io')(server),
	mongoClient = require('mongodb').MongoClient,
	crud = require('./crud/mongo-crud'),
	assert = require('assert');

//mongodb url server
var url = 'mongodb://localhost:27017/data';
var records = {};

mongoClient.connect(url, {server: {poolSize: 3}}, function(err, db){
	assert.equal(err, null, ['No pudo conectarse a la base de datos']);

	db.collection('home').remove();
	db.collection('records').remove();

	//conexion con el socket cliente
	io.on('connection', function(client){
		console.log("conexion establecido con el cliente");

		//se recibe el perfil de la casa con la medicion
		client.on('home', function(data){
			
			crud.enrollHomes(db, data);

			//verificcar el estado de la base de datos
			db.collection('home').find({}).toArray(function(err, docs){console.log(docs)});
			db.collection('records').find({}).toArray(function(err, docs){console.log(docs)});
		});

		client.on('data_update', function(data){
			crud.updateHomes(db, data, function(reference){

				//creamos una referecia normalizada para la
				//coleccion records desde la coleccion home
				
				records.home_id = reference;
				records.meassure = data.meassure;
				
				crud.insertMeassure(db, records);
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
