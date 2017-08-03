import socket
import sys

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect(('', 6789))
socket.send(sys.argv[1])
print socket.recv(1024)
