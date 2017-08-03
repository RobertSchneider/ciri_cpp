import socket
import sys

HOST = socket.gethostbyname(socket.gethostname())
socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((HOST, 6789))
socket.send(sys.argv[1])
print socket.recv(1024)
