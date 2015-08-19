(function(){
	var app = angular.module('googleMaps',[]);

	google.maps.event.addDomListener(window, 'load', function(){
		console.log("arbol DOM totalmente construido");
		angular.bootstrap(document, ['geoMeter']);
	});
	app.directive('googleMapView',['$timeout', function($timeout){
		return {
			restrict: 'E',
			templateUrl: 'templates/maps.html',
			require: '?ngModel',
			link: function(scope, element, attrs, ngModel) {

				var _counter = _counter || 0,
					_image = 'images/map-marker.png';

				scope.options = {
			    center:new google.maps
			    	.LatLng(-25.25463261974944, -57.513427734375),
			    zoom:6,
			    mapTypeId:google.maps.MapTypeId.ROADMAP
				};

				scope.map = new google.maps
					.Map(document.getElementById("googleMap"),scope.options);

				scope.marker = new google.maps.Marker({
		            title: 'Location',
		            map: scope.map,
		            icon: _image,
		            draggable: true
				});

				google.maps.event.addListener(scope.map, 'click', function(event){
					_counter++;
					scope.lat = event.latLng.lat();
					scope.lng = event.latLng.lng();

					// creamos un objeto LatLng
					scope.LatLng = new google
					.maps.LatLng(scope.lat, scope.lng);

					// permite graficar un circulo como máximo
					if(_counter <= 1){

						// renderiza el valor inroducido por el usuario
						ngModel.$render = function(){

							//actualizamos el controlador desde la vista
							var _newRadius = scope.circle.radius;

							$timeout(function(){
								scope.circle.setRadius(+_newRadius);
							});

						};
				  
			        	scope.circle = new google.maps.Circle({
			        		center: scope.LatLng,
				            map:scope.map,
				            clickable: false,
				            // metres
				            radius: 50000,
				            fillColor: '#cfd8dc',
				            fillOpacity: .6,
				            strokeColor: '#3f51b5',
				            strokeOpacity: .5,
				            strokeWeight: 2,
				            editable: true,
			        	});

			        	// actualizamos la vista desde el controlador
			        	scope.$apply();

			        }
			        else {
			        	scope.circle.setCenter(
			        		scope.LatLng
			        	);
			        };

		        	google.maps.event.addListener(
		        		scope.circle, 'radius_changed', function(){
		        			scope.$apply();

							//resfull 
							$timeout(function(){
			        			scope.restCtrl.GET(
									scope.circle.radius,
									scope.circle.center.lat,
									scope.circle.center.lng
								);
			        		});
		        		}
		        	);

			        google.maps.event.addListener(scope.circle,'center_changed', function(){
			        	scope.$apply();

			        	//resfull 
			        	$timeout(function(){
			        		scope.restCtrl.GET(
								scope.circle.radius,
								scope.lat,
								scope.lng
							);
			        	});
		        	});
			    });
			}
		};
	}]);

	app.directive('latLng',[ '$timeout',function($timeout){
		return {
			restrict: 'A',
			require: '?ngModel',
			link: function(scope, element, attrs, ngModel){
				ngModel.$render = function(){
					if(ngModel.$viewValue)
						element.val(ngModel.$viewValue);
				};

				ngModel.$commitViewValue = function(){
					if(ngModel.$viewValue){
						if(attrs.coord == "latitud")
							scope.LatLng = new google.maps.LatLng(ngModel.$viewValue, scope.lng);
						else
							scope.LatLng = new google.maps.LatLng(scope.lat, ngModel.$viewValue);
						$timeout(function(){
							scope.circle.setCenter(scope.LatLng);
						});
					};
				};
			}
		};
	}]);
})();
