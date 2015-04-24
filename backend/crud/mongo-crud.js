var assert = require('assert');

var updateHomes = function(db, data, callback){
	var collection = db.collection('home'),
		indexOptions = {min: -500, max: 500, w: 1};
	
	collection.ensureIndex(
		{localidad: "2dsphere"},
		indexOptions, 
		function(err,success){
			assert.equal(err, null);
		}
	);
	
	collection.update(
		{NIS: data.NIS}, 
		data, 
		{upsert: true, w:1}, 
		function(err, success){
			assert.equal(err, null, ['error al insertar los datos']);
			assert.equal(1, success.result.n);
			console.log('datos actualizados correctamente');
		}
	);

	callback(data.NIS);
};

var insertMeassure = function(db, data, callback){
	var collection = db.collection('records');
	
	collection.insert(data, function(err, success){
		assert.equal(err, null, ['error al insertar los datos']);
		assert.equal(success.result.n, 1);
	});

	collection.ensureIndex({date : -1, home_id: 1}, function(err, success){
		assert.equal(err, null, ['error al indexar los atributos']);
	});
}

var findHomes = function(db, lat, lng, radio, callback){
	var collection = db.collection('home');

	collection.find({
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
			callback(docs);
			db.close();
	});	
};

var insertZone = function(db, data, callback){
	var collection = db.collection('zone');

	collection.insert(data, function(err, success){
		assert.equal(err, null, ['error al insertar los datos']);
		assert.equal(success.result.n, 1);
	});

	collection.ensureIndex({name: 1}, function(err, success){
		assert.equal(err, null, ['error al indexar los atributos']);
	});
};

exports.insertDocuments = insertMeassure;
exports.updateHomes = updateHomes;
exports.findHomes = findHomes;
exports.insertZone = insertZone;
