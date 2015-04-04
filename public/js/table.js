(function (){
	var app = angular.module('customDirectives', []);

	app.directive('tablaMediciones',function(){
		return {
			restrict: 'E',
			templateUrl: 'table.html',
			controller: function($scope){
				this.row = null;
				this.umbral = 425;

				this.selectRow = function(newRow, coordenandas){
					this.row = newRow;
					console.log("Podemos hacer uso de las coordenadas para solicitar informacion de un hogar");
				};

				this.info = function(rowSelected){
					return this.row === rowSelected;
				};

				this.warning = function(medicion){
					return 0.95*this.umbral >= medicion.consumo && medicion.consumo >= 0.9*this.umbral;
				};

				this.danger = function(medicion){
					return medicion.consumo > 0.95*this.umbral;
				};

				this.markerOnMap = function(coordenandas){
					var LatLng = new google.maps.LatLng(coordenandas.Lat, coordenandas.Lng);
					$scope.gCtrl.marker.setPosition(LatLng);
				};

			},
			controllerAs: 'tableCtrl'
			,
		};
	});
	
})();
