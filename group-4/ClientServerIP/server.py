#!/usr/bin/env python3
import asyncio

clients = []
car = None
car_ip = ""

class CarTcpProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        global clients
        self.transport = transport
        self.peername = transport.get_extra_info("peername")
        print("connection_made: {}".format(self.peername))
        print("ip: {}".format(self.peername[0]))
        clients.append(self)

    def data_received(self, data):
        global clients
        global car
        print("data_received: {}".format(data.decode()))

        if data.decode().startswith("register_car"):
            print("Registring car")
            car = self

        elif data.decode().startswith("get_car_ip"):
            if car:
                self.transport.write(str.encode(car.peername[0]))
            else:
                self.transport.write("0.0.0.0".encode())

    def connection_lost(self, ex):
        global car
        global clients
        print("connection_lost: {}".format(self.peername))
        if self == car:
            car = None
        clients.remove(self)

if __name__ == '__main__':
    print("starting up..")

    loop = asyncio.get_event_loop()
    coro = loop.create_server(CarTcpProtocol, port=1234)
    server = loop.run_until_complete(coro)

    for socket in server.sockets:
        print("serving on {}".format(socket.getsockname()))

    loop.run_forever()
