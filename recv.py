import socket# pickle
import struct
import datetime

UDP_IP = "192.168.1.163"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# sock.bind((socket.gethostname(), 5005))
sock.bind((UDP_IP, 5005))

print(socket.gethostname())

while True:
    data, addr = sock.recvfrom(1024)
    t = datetime.datetime.now()
    print(len(data))

    # unpacked = struct.unpack('i', data[0])
    unpacked = struct.unpack('fff26c', data)
    print(len(data))
    print(data)

#TODO: write to a file with time down to microseconds/milisih
