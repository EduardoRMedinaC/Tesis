var express = require('express'),
	router = express.Router(),
	bodyParser = require('body-parser'),
	parseUrlencoded = bodyParser.urlencoded({ extended: false }),
	mongoClient = require('mongodb').MongoClient,
	assert = require('assert'),
	crud = require('../crud/mongo-crud');

var url = 'mongodb://localhost:27017/data';

router.route('/')
	.get(function(request, response){
		
		//query ?radio=<radio>&lat=<lat>&lng=<lng>

		var lng = request.query.centerLng,
			lat = request.query.centerLat,
			radio = request.query.radio;

		mongoClient.connect(url, {server: {poolSize: 1}}, function(err, db){
			assert.equal(err, null, ['No pudo conectarse a la base de datos']);

			crud.findHomes(db, lat, lng, radio, function(docs){
				response.json(docs);
			});
		});
	})
	.post(parseUrlencoded, function(request, response){
		var zona = request.body;

		mongoClient.connect(url, {server: {poolSize: 1}}, function(err, db){
			null;
		});
	});

module.exports = router; 
