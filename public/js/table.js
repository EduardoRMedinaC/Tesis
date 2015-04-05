(function (){
	var app = angular.module('customDirectives', []);

	app.directive('tablaMediciones',function($timeout){
		return {
			restrict: 'E',
			templateUrl: 'table.html',
			controller: function($scope){
				this.umbral = 425;
				this.Warning = {
					total: 0,
					lot: [],
				};
				this.Danger = {
					total: 0,
					lot: [],
				};

				this.indicators = function(referencia){

					$timeout(function(){
						for(parcial of referencia.lot){
							referencia.total += parcial;
						};
					});
				};

				this.selectRow = function(newRow, coordenandas){
					this.row = newRow;
					console.log(
						"Podemos hacer uso de las coordenadas para" 
						+ "solicitar informacion de un hogar"
					);
				};

				this.info = function(rowSelected){
					return this.row === rowSelected;
				};

				this.warning = function(index, medicion){
					var isWarning = 0.95*this.umbral >= medicion.consumo 
						&& medicion.consumo >= 0.9*this.umbral;

					isWarning ? this.Warning.lot[index]=1 
					:this.Warning.lot[index]=0;
					
					return isWarning; 
				};

				this.danger = function(index, medicion){
					this.index = index + 1;
					var isDanger = medicion.consumo > 0.95*this.umbral;
					
					isDanger ? this.Danger.lot[index]=1
					:this.Danger.lot[index]=0; 

					return isDanger;
				};

				this.markerOnMap = function(coordenandas){
					var LatLng = new google.maps
						.LatLng(coordenandas.Lat, coordenandas.Lng);
					$scope.gCtrl.marker.setPosition(LatLng);
				};

				this.indicators(this.Warning);
				this.indicators(this.Danger);
			},
			controllerAs: 'tableCtrl',

		};
	});
	
})();
