import android
import socket
import struct 
import time, datetime
# import pickle

"""
Acceleroemeter data
measures accelerateion force in m/s^2
applied to a device on (x,y,z), including
force of gravity
"""

# globals
UDP_IP = "192.168.1.163"
UDP_PORT = 5005

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    droid = android.Android()
    record_accel_data(sock, droid)


def record_accel_data(sock, droid):
    dt = 100 # time to sleep
    end_time = 10000
    time_sensed = 0
    droid.startSensingTimed(2, dt)
    while time_sensed <= end_time:
        t = datetime.datetime.now()
        xyz = droid.sensorsReadAccelerometer().result
        if not any(e is None for e in xyz):
            print(xyz)
            print(t)
            send(sock, xyz, str(t))
        time.sleep(dt/1000.0)
        time_sensed += dt
    droid.stopSensing()


def send(sock, xyz, t):
    global UDP_PORT, UDP_IP
    data = struct.pack('f'*len(xyz), *xyz)
    data += (bytes(t, 'utf-8'))
    print(len(data))
    sock.sendto(data, (UDP_IP, UDP_PORT))
    # sock.sendto(bytes(t, 'utf-8'), (UDP_IP, UDP_PORT))


if __name__ == "__main__":
    main()
