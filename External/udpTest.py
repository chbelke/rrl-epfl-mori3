# import socket

# # creates socket object
# sock = socket.socket(socket.AF_INET,
#                   socket.SOCK_STREAM)

# host = socket.gethostname() # or just use (host = '')
# port = 50001

# # s.connect((host, port))
# # sock.bind(('', port))
# # sock.bind(("localhost", port))
# sock.bind(('', port))
# print(sock.getsockname())
# sock.listen(2)
# conn, addr = sock.accept()
# print(conn, "has connected")

# while(True):
#     data= conn.recvfrom(1024).decode("ascii") 
#     print("the time we got from the server is {}".format(data))
# sock.close()


import socket
port = 50001
s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(('',port)) #绑定端口binding the port
print('waiting...')
while True:   #接收数据receiving data
    data,addr=s.recvfrom(1024) 
    print('received:',data,'from',addr)

# import socket
# import threading
# import sys
# host = ''
# port = 50000

# class client(threading.Thread):
#     def __init__(self, conn):
#         super(client, self).__init__()
#         self.conn = conn
#         self.data = ""

#     def run(self):
#         while True:
#             self.data = self.data + self.conn.recv(1024)
#             if self.data.endswith(u"\r\n"):
#                 print(self.data)
#                 self.data = ""

#     def send_msg(self,msg):
#         self.conn.send(msg)

#     def close(self):
#         self.conn.close()

# class connectionThread(threading.Thread):
#     def __init__(self, host, port):
#         super(connectionThread, self).__init__()
#         try:
#             self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#             self.s.bind((host,port))
#             self.s.listen(5)
#         except socket.error:
#             print('Failed to create socket')
#             sys.exit()
#         self.clients = []

#     def run(self):
#         while True:
#             conn, address = self.s.accept()
#             c = client(conn)
#             c.start()
#             c.send_msg(u"\r\n")
#             self.clients.append(c)
#             print('[+] Client connected: {0}'.format(address[0]))



# def main():
#     print("running")
#     get_conns = connectionThread(host, port)
#     get_conns.start()
#     print("here")
#     while True:
#         try:
#             response = input() 
#             for c in get_conns.clients:
#                 c.send_msg(response + u"\r\n")
#         except KeyboardInterrupt:
#             sys.exit()

# if __name__ == '__main__':
#     main()