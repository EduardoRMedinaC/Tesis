(function () {
	var app = angular.module(
		'geoMeter', 
		['customDirectives','materialchart','graphcontroller','googleMaps', 'menu']
	);

	app.controller('restController',['$http', function($http){

		this.form = {};

		this.viewZone = {
			name: 'Esta zona no esta definida',
			description: 'Sin Descripcion...'
		}

		this.OBISmap = {
			EAT: true,
			ERT: true,
			DM: false,
			CL1: false,
			CL2: false,
			Cl3: false,
			time: true
		}

		var hogares = this;

		this.GET = function(radio, centerLat, centerLng){
			var req = {
				method: 'GET',
				url: '/zona?radio='+radio
					+'&centerLat='+centerLat+'&centerLng='+centerLng,
			};

			$http(req).success(function(data){
				hogares.data = data;
			});

			//Array of arrays [[data,consumo,newfield],[data,consumo, ...], ...[],[]]
			//this.initField = [[hogar[key] for(key in hogar) if(key=='time')]for(hogar of this.totalDatos)];
		};

		this.POST = function(name, description, radio, center){

			var req = {
				method: 'POST',
				url: '/zona',
				data: {
						name: name,
						description: description,
						radio: radio,
						center: center
				},
				headers: {
                	'Content-Type': 'application/json'
            	}
			};

			$http(req)
			.success(function(data){
				console.log("El nombre ya existe etc");
				this.form = {};
			})
			.error(function(){
				
			});
		};

	}]);

})();
