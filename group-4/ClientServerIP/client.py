#!/usr/bin/env python

import socket
import sys
import time
import select


TCP_PORT = 11999 #TCP port to listen from
TCP_IP = 'address.to.server'
BUFFER_SIZE = 1024

def get_car_ip():
    '''
    '''
    MESSAGE = "get_car_ip"

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((TCP_IP, TCP_PORT))
        s.send(MESSAGE.encode())
        data = s.recv(BUFFER_SIZE)
        s.close()

        print("The cars ip is:", data.decode())
    except:
        print("Could not connect to server! I blame Shan!")

def register_car():
    '''
    '''
    MESSAGE = "register_car"

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    try:
        while True:
            ready_to_read, ready_to_write, in_error = \
                select.select([s,], [s,], [], 5)
            if len(ready_to_write) > 0:
                print("Sending ip to server!")
                s.send(MESSAGE.encode())
                read_ready, _, _ = \
                    select.select([s,], [], [], 5)
                if read_ready:
                    data = s.recv(BUFFER_SIZE)
                    if (not data.decode() == "ok"):
                        break
                else:
                    break
            time.sleep(6)
    finally:
        print("Error: Something is wrong! (dunno why thou)")
        s.close()
    
if __name__ == '__main__':
    commands = {
            'car': register_car,
            'ip' : get_car_ip
            }
    if (len(sys.argv) > 1):
        try:
            commands[sys.argv[1]]()
        except:
            pass
    else:
        commands['ip']()
