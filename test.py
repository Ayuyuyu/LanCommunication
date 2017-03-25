#!/usr/bin/python
# -*- coding: utf-8 -*-
import socket
'''
HOST="127.0.0.1"
PORT=5099
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)      #定义socket类型，网络通信，TCP
s.connect(("172.31.50.248",4423))
while 1:
    aes_str = raw_input("Please AES_str: ")
    s.sendall(aes_str)        
    #data=s.recv(1024)
    try:
        data = s.recv(1024)
        print data
    except:
        print "test"
s.close()   #关闭连接

'''
BUFSIZE = 1024  
  
ADDR = ("172.31.50.248", 4423)  
  
udpCliSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  
i = 0
while True:  
    #data = raw_input('>')
    data = '{"type":"UDP_MSG_NORMAL","msg":"test"}'
    if not data:  
        break
    if i > 10:
        break
    i+=1
    udpCliSock.sendto(data,ADDR)  
    #data,ADDR = udpCliSock.recvfrom(BUFSIZE)  
    #if not data:  
    #    break  
    #print data  
  
udpCliSock.close()  

