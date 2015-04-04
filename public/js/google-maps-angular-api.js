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
			    center:new google.maps.LatLng(-25.25463261974944, -57.513427734375),
			    zoom:6,
			    mapTypeId:google.maps.MapTypeId.ROADMAP
				};

				this.map = new google.maps.Map(document.getElementById("googleMap"),this.options);	

				this.image = 'map-icons/map-marker.png';

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

				var counter = counter || 0;

				google.maps.event.addListener(scope.gCtrl.map, 'click', function(event){

					counter++;
					scope.gCtrl.latLngCMarker = new google.maps.LatLng(event.latLng.k, event.latLng.D);

					if(counter <= 1){
						ngModel.$render = function(){
							var newValue = scope.gCtrl.circle.radius;
							$timeout(function(){
								scope.gCtrl.circle.setRadius(+newValue);
							});
						};
				  
			        	scope.gCtrl.circle = new google.maps.Circle({
			        		center: scope.gCtrl.latLngCMarker,
				            map:scope.gCtrl.map,
				            clickable: false,
				            // metres
				            radius: 50000,
				            fillColor: '#ffffff',
				            fillOpacity: .6,
				            strokeColor: '#1976d2',
				            strokeOpacity: .5,
				            strokeWeight: 2,
				            editable: true,
			        	});
			        }
			        else {
			        	scope.gCtrl.circle.setCenter(scope.gCtrl.latLngCMarker);
			        };

		        	google.maps.event.addListener(scope.gCtrl.circle,'radius_changed', function(){

							//Angular no detecta los eventos javascript puros - update hacia el controlador

							scope.$apply(function() {
								ngModel.$setViewValue(scope.gCtrl.circle.getRadius());
							});
		        	});

		        	google.maps.event.addListener(scope.gCtrl.circle,'center_changed', function(){
		        		console.log(scope.appCtrl);
		        		scope.$apply(function(){
		        			scope.appCtrl.cambiarTabla();
		        		});
		        		
		        	});
		        	
		        });

			}
		};
	});

})();
