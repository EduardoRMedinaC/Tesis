(function(){
	var app = angular.module('googleMaps',[]);

	google.maps.event.addDomListener(window, 'load', function(){
		app.directive('googleMapView', function($timeout){
			return {
				restrict: 'E',
				templateUrl: 'templates/maps.html',
				controller: function(){

					this.options = {
				    center:new google.maps
				    	.LatLng(-25.25463261974944, -57.513427734375),
				    zoom:6,
				    mapTypeId:google.maps.MapTypeId.ROADMAP
					};

					this.map = new google.maps
						.Map(document.getElementById("googleMap"),this.options);	

					this.image = 'images/map-marker.png';

					this.marker = new google.maps.Marker({
			            title: 'Location',
			            map: this.map,
			            icon: this.image,
			            draggable: true
					});
				},
				controllerAs: 'gCtrl',
				require: '?ngModel',
				link: function(scope, element, attrs, ngModel) {

					var counter = counter || 0,
						gCtrl = scope.gCtrl;

					google.maps.event
						.addListener(gCtrl.map, 'click', function(event){
							counter++;
							gCtrl.centerCircle = new google
							.maps.LatLng(event.latLng.k, event.latLng.D);

							//permite graficar un circulo como máximo

							if(counter <= 1){

								//renderiza el valor inroducido por el usuario

								ngModel.$render = function(){

									//actualizamos el modelo desde la vista
									
									var newValue = gCtrl.circle.radius;

									$timeout(function(){
										gCtrl.circle.setRadius(+newValue);
									});

								};
						  
					        	gCtrl.circle = new google.maps.Circle({
					        		center: gCtrl.centerCircle,
						            map:gCtrl.map,
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

					        	//actualizamos la vista desde el modelo
					        	scope.$apply();

					        }
					        else {
					        	gCtrl.circle.setCenter(
					        		gCtrl.centerCircle
					        	);
					        };

				        	google.maps.event
					        	.addListener(
					        		gCtrl.circle, 'radius_changed', function(){
					        			scope.$apply();

										//resfull 
										$timeout(function(){
						        			scope.restCtrl.GET(
												gCtrl.circle.radius,
												gCtrl.circle.center.k,
												gCtrl.circle.center.D
											);
						        		});
					        		}
					        	);

					        google.maps.event
					        	.addListener(
					        		gCtrl.circle,'center_changed', function(){
						        		scope.$apply();

						        		//resfull 
						        		$timeout(function(){
						        			scope.restCtrl.GET(
												gCtrl.circle.radius,
												gCtrl.circle.center.k,
												gCtrl.circle.center.D
											);
						        		});
					        		}
					        	);
				        });
				}
			};
		});
	});
})();
