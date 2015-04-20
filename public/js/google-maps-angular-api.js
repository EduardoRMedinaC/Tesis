(function(){
	var app = angular.module('googleMaps',[]);

	google.maps.event.addDomListener(window, 'load', function(){
		console.log("arbolDom totalmente construido");
	});

	app.directive('googleMapView', function($timeout){
		return {
			restrict: 'E',
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
						gCtrl.latLngCCircle = new google
						.maps.LatLng(event.latLng.k, event.latLng.D);

						if(counter <= 1){
							ngModel.$render = function(){
								var newValue = gCtrl.circle.radius;

								$timeout(function(){
									gCtrl.circle.setRadius(+newValue);
								});
							};
					  
				        	gCtrl.circle = new google.maps.Circle({
				        		center: gCtrl.latLngCCircle,
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
				        }
				        else {
				        	gCtrl.circle.setCenter(
				        		gCtrl.latLngCCircle
				        	);
				        };

			        	google.maps.event
				        	.addListener(
				        		gCtrl.circle, 'radius_changed', function(){

									//Angular no detecta los eventos js puros
									//update hacia el controlador

									scope.$apply(function() {
										ngModel.$setViewValue(
											gCtrl.circle.getRadius()
										);
									});

									console.log(gCtrl.circle.getCenter());
									//resfull 
									scope.appCtrl.resfull(
										gCtrl.circle.getRadius(),
										gCtrl.circle.getCenter().k,
										gCtrl.circle.getCenter().D
									);
				        		}
				        	);

				        google.maps.event
				        	.addListener(
				        		gCtrl.circle,'center_changed', function(){
				        			console.log("Ha cambiado el centro");
					        		scope.$apply(function(){
					        			null;
					        		});
				        		}
				        	);
			        });
			}
		};
	});
})();
