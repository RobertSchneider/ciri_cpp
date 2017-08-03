import socket
HOST = socket.gethostbyname(socket.gethostname())
print HOST

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((HOST, 6789))
sock.listen(10)
