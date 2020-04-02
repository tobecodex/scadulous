import socket

f = open("../../test_files/teapot.stl").read()
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("172.28.144.1", 4242))
s.send(f.encode())

