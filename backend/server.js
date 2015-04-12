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
	});

	app.use(express.static('../public'));

});

var insertDocument = function(db, collection, data){
	var collection = db.collection(collection),
		indexOptions = {min: -500, max: 500, w: 1};
	
	collection.insert(data, {w:1}, function(err, success){
		assert.equal(err, null, ['error al insertar los datos']);
		assert.equal(1, success.result.n);
	});

	collection.ensureIndex({loc: "2d"}, indexOptions, function(err,success){
			assert.equal(err, null);
	});
};

server.listen(80, function(){
	console.log("tu servidor está listo en " + this.address().port);
});
