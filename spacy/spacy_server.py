import socket
import spacy
import contextlib

sock = socket.socket()
sock.bind(('', 6789))
sock.listen(5)

nlp = spacy.load('en')

print '***READY***'

while True:
    conn, addr = sock.accept()
    while True:
        data = conn.recv(1024)
        if not data: break
        doc = nlp(unicode(data, encoding="utf-8"))
        result = '';
        for w in doc:
            result += str(w.i + 1) + " " + w.text + " " + w.pos_  + " " + str(w.head.i + 1) + " " + w.dep_ + " " + w.head.text + "\n"
        conn.sendall(result)
