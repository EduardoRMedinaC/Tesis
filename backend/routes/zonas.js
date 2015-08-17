
var express = require('express'),
	router = express.Router(),
	bodyParser = require('body-parser'),
	parseJSONencoded = bodyParser.json(), //parseUrlencoded to support url
	mongoClient = require('mongodb').MongoClient,
	assert = require('assert'),
	crud = require('../crud/mongo-crud');

var url = 'mongodb://localhost:27017/data/db';

router.route('/')
	.get(function(request, response){
		
		//query ?radio=<radio>&lat=<lat>&lng=<lng>

		var lng = request.query.centerLng,
			lat = request.query.centerLat,
			radio = request.query.radio;

		mongoClient.connect(url, {server: {poolSize: 1}}, function(err, db){
			assert.equal(err, null, ["can't connect to db"]);

			crud.findHomes(db, lat, lng, radio, function(docs){
				response.json(docs);
			});
		});
	})
	.post(parseJSONencoded, function(request, response){
		var zona = request.body;
		response.status(201).json(zona.name);
		
		mongoClient.connect(url, {server: {poolSize: 1}}, function(err, db){
			//crud.insertZone(db, zona);
		});
	});

module.exports = router; 
