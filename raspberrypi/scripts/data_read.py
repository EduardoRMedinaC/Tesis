from __future__ import print_function
from socketIO_client import SocketIO, LoggingNamespace

import serial
import time

def send(port, bytes, tr):
	""" sends an command to serial and reads and checks the echo
	port  - the open serial port
	bytes - the string to be send
	tr    - the responce time
	"""
	#print ("start send")
	port.write(bytes)
	time.sleep(tr)
	echo = port.read (len(bytes))
	if (echo != bytes):
		print ("echo is not same as send:", bytes, "vs", echo)
	#print("end send")

def read_datablock():
	ACK = '\x06'
	STX = '\x02'
	ETX = '\x03'
	tr = 0.2
	"""does all that's needed to get meter data form the meter device """
	try:
		IskraMT174 = serial.Serial(
			port = '/dev/ttyAMA0',
			baudrate = 300,
			bytesize = 7,
			parity = 'E',
			stopbits = 1,
			timeout = 1.5
		); # open port at specified speed
		#1->
		time.sleep(tr)
		Request_message= '/?!\r\n' #IEC 62056-21:2002(E) 6.3.1
		send(IskraMT174, Request_message, tr)

		#2 <-
		time.sleep(tr)
		Identification_message = IskraMT174.readline() #IEC 62056-21:2002(E) 6.3.2
		if(len(Identification_message) < 1 or Identification_message[0] != '/'):
			print("no Identification message")
			IskraMT174.close()
			return ""
		if (len (Identification_message) < 7):
			print("Identification message to short")
			IskraMT174.close()
			return""
		if (Identification_message[4].islower()):
			tr = 0.02
		manufacturers_ID = Identification_message[1:4]
		if (Identification_message[5] == '\\'):
			identification = Identification_message[7:-2]
		else:
			identification = Identification_message[5:-2]
		speed = Identification_message[4]
		#print ("speed = ", speed)
		if (speed == "1"): new_baud_rate = 600
		elif (speed == "2"): new_baud_rate = 1200
		elif (speed == "3"): new_baud_rate = 2400
		elif (speed == "4"): new_baud_rate = 4800
		elif (speed == "5"): new_baud_rate = 9600
		elif (speed == "6"): new_baud_rate = 19200
		else:
			new_baud_rate = 300
			speed = "0"
		print (manufacturers_ID, " ", identification, " speed=", speed)

		# 3 ->
		Acknowledgement_message = ACK + '0' + speed + '0\r\n' #IEC 62056-21:2002(E) 6.3.3
		send (IskraMT174, Acknowledgement_message, tr)
		IskraMT174.baudrate = new_baud_rate
		time.sleep(tr)

		# 4 <-
		datablock = ""
		if (IskraMT174.read() == STX):
			x = IskraMT174.read()
			BCC = 0
			while (x != '!'):
				BCC = BCC ^ord(x)
				datablock = datablock + x
				x = IskraMT174.read()
			while (x != ETX):
				BCC = BCC ^ ord(x) # ETX itself is part of block check
				x = IskraMT174.read()
			BCC = BCC ^ ord(x)
			x = IskraMT174.read() # x is now the Block Check Character
			# last character is read, could close connection here
			if (BCC != ord(x)): # received correctly?
				datablock= ""
				print("Result not OK, try again")
		else:
			print("No STX found, not handled")
		IskraMT174.close()
		return datablock
	except:
		print ("Some error reading data")
		if (IskraMT174.isOpen()):
			IskraMT174.close()
		return ""

def meter_data(datablock, map, header):
	""" takes a datablock as received from the meter and returns a list with requested meter data as set in map
	if header != 0 a list with data type and units is returned """
	line = []
	## initialise line
	for l in range (len(map)):
		if (header == 1):
			line.append(map[l][1])
		elif (map[l][0] == "time"):
			line.append(time.strftime("%Y-%m-%d %H:%M:%S"))
		else:
			line.append("")
	datasets = datablock.split('\n')
	for dataset in datasets:
		if (dataset != ""):
			x = dataset.split('(')
			address = x[0]
			x = x[1][:-2].split(' ') # the standard seems to have a '*' instead of ' ' here
			value = x[0]
			try:
				unit = '['+x[1]+']'
			except:
				unit = ""
			for l in range (len(map)):
				if (map[l][0] == address):
					if (header == 0):
						line[1] = value
					else:
						line[1] = map[l][1] + unit
					break;
	return line

def output(filename, line):
	f = open(filename, "a")
	print(line, file=f)
	f.close()

#map = [
	# The structure of the meter_data() output can be set with this variable
	# first string on each line is the cosim adress of the data you want to safe or "time" to insert the time
	# the second string on each line is a description of the type of data belonging to the cosim address
	# the order of the lines sets the order of the meter_data() output

lectura = read_datablock()
print(lectura)


map = [
	["1-0:0.0.1*255","meterid"],
	["1-0:1.8.0*255","consumo total"],
	["1-0:1.8.1*255","consumo mi pilin"],
	["1-0:1.8.2*255","consumo tu pilin"]
]


def data_parse(datablock, map):
	meassure = {}
	meassure_meterid = {}
	datasets = datablock.split("\n")
	for data in datasets:
		if data != "":
			obisValor = data.split("(")
			obis = obisValor[0]
			value = obisValor[1][:-2]
			for fila in range(len(map)):
				if map[fila][0] == obis:
					if map[fila][1] == "meterid":
						meassure_meterid["meterid"] = value
					else: 
						meassure[map[fila][1]] = value
	meassure["time"] = time.strftime("%Y-%m-%d %H:%M:%S")
	meassure_meterid["meassure"] = meassure


	return meassure_meterid

print (data_parse(lectura, map))

def message_callback():
	print("Enviando los dats recogidos del mdidor")
	socketIO.emit('home', data_parse(lectura, map))

with SocketIO('direccionip', 80, LoggingNamespace) as socketIO:
	socketIO.on('connect', message_callback)
	socketIO.wait(seconds=1)


