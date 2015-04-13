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

	//conexion con el socket cliente
	io.on('connection', function(client){
		console.log("conexion establecido con el cliente");

		//se recibe el perfil de la casa
		client.on('perfil', function(data){
			insertDocument(db, 'hogares', data);
		});
	});

	app.use(express.static('../public'));

	app.get('/zona', function(request, response){
		
		//query ?radio=<radio>&lat=<lat>&lng=<lng>

		var lng = request.query.centerLng,
			lat = request.query.centerLat,
			radio = request.query.radio;

		db.collection('hogares').find({
			localidad:{
				$near: {
					$geometry: {
						type: "Point",
						coordinates: [parseFloat(lng), parseFloat(lat)]
					},
					$maxDistance: parseFloat(radio)
				}
			}
		})
		.toArray(function(err, docs){
				console.log(docs);
		});
	});

});

var insertDocument = function(db, collection, data){
	var collection = db.collection(collection),
		indexOptions = {min: -500, max: 500, w: 1};
	
	collection.insert(data, {w:1}, function(err, success){
		assert.equal(err, null, ['error al insertar los datos']);
		assert.equal(1, success.result.n);
	});

	collection.ensureIndex({localidad: "2dsphere"}, indexOptions, function(err,success){
			assert.equal(err, null);
	});

};

server.listen(80, function(){
	console.log("tu servidor está listo en " + this.address().port);
});
