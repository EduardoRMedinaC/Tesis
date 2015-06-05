(function () {

	var datosZona = [
		{date: Date.now(), consumo: 250, potenciaReactiva: 50, potenciaAparente: 280, 
			factorPotencia: 0.98, monto: 300000, Lat:-27.5, Lng:-60},
		{date: Date.now() + 1, consumo: 400, potenciaReactiva: 300, potenciaAparente: 500, 
			factorPotencia: 0.83, monto: 500000,Lat:-26.8, Lng:-58.9},
		{date: Date.now() + 2, consumo: 130, potenciaReactiva: 50, potenciaAparente: 190,
			factorPotencia: 0.91, monto: 200000, Lat:-26.9, Lng:-55.896},
		{date: Date.now() + 3, consumo: 255, potenciaReactiva: 60, potenciaAparente: 290,
			factorPotencia: 0.95, monto: 350000, Lat:-26.93, Lng:-58.89},
		{date: Date.now() + 4, consumo: 800, potenciaReactiva: 430, potenciaAparente: 960,
			factorPotencia: 0.89, monto: 700000, Lat:-25.4, Lng:-59.998899}
	];

	var app = angular.module(
		'geoMeter', 
		['customDirectives','materialchart','graphcontroller','googleMaps']
	);

	app.controller('restController',['$http', function($http){
		this.form = {};

		this.viewZone = {
			name: 'Esta zona no esta definida',
			description: 'Sin Descripcion...'
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
