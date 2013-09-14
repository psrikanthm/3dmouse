#!/usr/bin/env python 

""" 
A simple echo server 
""" 

import socket 

readings_file = '/Users/srikanth/Documents/3dmouse/readings.txt'
host = '' 
port = 50002
backlog = 5 
size = 4096 
file_write = open(readings_file, 'w')
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.bind((host,port)) 
s.listen(backlog) 
print "Waiting for Connections on: " + str(port) 
while 1: 
    client, address = s.accept()
    data = client.recv(size) 
    value = data[:data.index(',')]
    print  "Data Received: " + value 
    file_write.write(value + '\n')
    client.close()
