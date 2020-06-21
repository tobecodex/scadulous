import socket

f = open("../../test_files/teapot_ascii.stl").read()
print(f)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("172.18.192.1", 4242))
s.send(f.encode())

