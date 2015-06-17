from __future__ import print_function
from socketIO_client import SocketIO, LoggingNamespace

import serial
import time


def send(port, bytes, tr):
    """
    sends an command to serial and reads and checks the echo
    port  - the open serial port
    bytes - the string to be send
    tr    - the responce time
    """

    # print ("start send")
    port.write(bytes)
    time.sleep(tr)
    echo = port.read(len(bytes))
    if (echo != bytes):
        print ("echo is not same as send:", bytes, "vs", echo)
    # print("end send")


def read_datablock():
    ack = '\x06'
    stx = '\x02'
    etx = '\x03'
    tr = 0.2
    """does all that's needed to get meter data form the meter device """
    try:
        mt174 = serial.Serial(
            port='/dev/ttyAMA0',
            baudrate=300,
            bytesize=7,
            parity='E',
            stopbits=1,
            timeout=1.5
        )  # open port at specified speed
        # 1->
        time.sleep(tr)
        request_message = '/?!\r\n'  # IEC 62056-21:2002(E) 6.3.1
        send(mt174, request_message, tr)

        # 2 <-
        time.sleep(tr)
        identification_message = mt174.readline()  # IEC 62056-21:2002(E) 6.3.2
        if(
            len(identification_message) < 1 or identification_message[0] != '/'
        ):
            print("no Identification message")
            mt174.close()
            return ""
        if (len(identification_message) < 7):
            print("Identification message to short")
            mt174.close()
            return""
        if (identification_message[4].islower()):
            tr = 0.02
        manufacturers_id = identification_message[1:4]
        if (identification_message[5] == '\\'):
            identification = identification_message[7:-2]
        else:
            identification = identification_message[5:-2]
        speed = identification_message[4]
        # print ("speed = ", speed)
        if (speed == "1"):
            new_baud_rate = 600
        elif (speed == "2"):
            new_baud_rate = 1200
        elif (speed == "3"):
            new_baud_rate = 2400
        elif (speed == "4"):
            new_baud_rate = 4800
        elif (speed == "5"):
            new_baud_rate = 9600
        elif (speed == "6"):
            new_baud_rate = 19200
        else:
            new_baud_rate = 300
            speed = "0"
        print (manufacturers_id, " ", identification, " speed=", speed)

        # 3 ->
        # IEC 62056-21:2002(E) 6.3.3
        acknowledgement_message = ack + '0' + speed + '0\r\n'
        send(mt174, acknowledgement_message, tr)
        mt174.baudrate = new_baud_rate
        time.sleep(tr)

        # 4 <-
        datablock = ""
        if (mt174.read() == stx):
            x = mt174.read()
            bcc = 0
            while (x != '!'):
                bcc = bcc ^ ord(x)
                datablock = datablock + x
                x = mt174.read()
            while (x != etx):
                bcc = bcc ^ ord(x)  # ETX itself is part of block check
                x = mt174.read()
            bcc = bcc ^ ord(x)
            x = mt174.read()  # x is now the Block Check Character
            # last character is read, could close connection here
            if (bcc != ord(x)):  # received correctly?
                datablock = ""
                print("Result not OK, try again")
        else:
            print("No STX found, not handled")
        mt174.close()
        return datablock
    except:
        print ("Some error reading data")
        if (mt174.isOpen()):
            mt174.close()
        return ""

# map

lectura = read_datablock

map = [
    ["1-0:0.0.1*255", "serial"],
    ["1-0:15.8.0*255", "Energia Activa Total"],
    ["1-0:15.6.0*255", "Demanda Maxima"],
    ["1-0:31.7.0*255", "Corriente en L1"],
    ["1-0:51.7.0*255", "Corriente en L2"],
    ["1-0:71.7.0*255", "Corriente en L3"]
]


def data_parse(datablock, map):
    meassure = {}
    meassure_meterid = {}
    datasets = datablock.split("\n")
    for data in datasets:
        if data != "":
            obis_valor = data.split("(")
            obis = obis_valor[0]
            value = obis_valor[1][:-2]
            for fila in range(len(map)):
                if map[fila][0] == obis:
                    if map[fila][1] == "serial":
                        meassure_meterid["serial"] = value
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
