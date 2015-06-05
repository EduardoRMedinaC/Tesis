from socketIO_client import  SocketIO, LoggingNamespace

coordenadas = [0,0]
profile = {"NIS":"", "Usuario":"", "CI":"", "Direccion":"", "localidad":{"type":"Point", "coordinates":[0,0]}, "mail":"", "RUC":"", "telefono":"", "meter_id":"", "xbee_id":""}
print "Bienvenido!!! este es el asistente de enrolamiento del abonado "
Nombre = raw_input("Ingrese su nombre y su apellido ")
CI = raw_input("Ingrese su numero de cedula ")
Domicilio = raw_input("Ingrese la direccion de su domicilio ")
telefono = raw_input("Ingrese un numero de telefono ")
mail = raw_input("ingrese su correo electronico ")
RUC = raw_input("Ingrese su numero de RUC ")

print "Datos Tecnicos"
print "Ingrese las coordenadas"
coordenadas[0] = input("Longitud: ")
coordenadas[1] = input("Latitud: ")
NIS = raw_input("Ingrese el NIS del abonado ")
meter_id = raw_input("Ingrese el id del medidor ")

band = True
while band:

    print "\r\nLos datos ingresados fueron"
    print "Nombre: %s" %(Nombre)
    print "CI: %s" %(CI)
    print "Domicilio: %s" %(Domicilio)
    print "Telefono: %s" %(telefono)
    print "E-mail: %s" %(mail)
    print "RUC %s" %(RUC)
    print "Longitud: %d" %(coordenadas[0])
    print "Latitud: %d" %(coordenadas[1])
    print "NIS: %s" %(NIS)
    print "meter id: %s" %(meter_id)

    confirmacion = raw_input("\r\nConfirmar los cambios? (Y/N)")

    if confirmacion == "N" or confirmacion == "n":
        print "Nombre: 1        CI: 2       Domicilio: 3"
        print "Telefono: 4      E-mail: 5   RUC: 6"
        print "Coordenadas:7    NIS: 8      meter id: 9"

        seleccion = raw_input("Elija el o los datos a modificar: ")
        seleccionados = seleccion.split(',')
        for indice in seleccionados:
            if indice == "1":
                Nombre = raw_input ("Nombre: ")

            if indice == "2":
                CI = raw_input("CI: ")

            if indice == "3":
                Domicilio = raw_input ("Domicilio: ")

            if indice == "4":
                telefono = raw_input ("Telefono: ")

            if indice == "5":
                mail = raw_input ("E-mail: ")

            if indice == "6":
                RUC = raw_input ("RUC: ")

            if indice == "7":
                coordenadas[0] = input ("Longitud: ")
                coordenadas[1] = input ("Latitud: ")

            if indice == "8":
                NIS = raw_input("NIS: ")

            if indice == "9":
                meter_id = raw_input("meter id: ")
    else:
        band = False

profile["NIS"] = NIS
profile["Usuario"] = Nombre
profile["CI"] = CI
profile["Direccion"] = Domicilio
profile["localidad"]["coordinates"][0] = coordenadas[0]
profile["localidad"]["coordinates"][1] = coordenadas[1]
profile["mail"] = mail
profile["RUC"] = RUC
profile["telefono"] = telefono
profile["meterid"] = meter_id
profile["meassure"] = {}

print profile

def message_callback():
	print "Conexion establecida con el servidor"
	socketIO.emit('home', profile)

with SocketIO('ipdepacheco', 80, LoggingNamespace) as socketIO:
	socketIO.on('connect',message_callback)
	socketIO.wait(seconds=1)
