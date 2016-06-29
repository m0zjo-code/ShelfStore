# Jonathan Rawlinson - M0ZJO - 2016
# https://github.com/m0zjo-code/ShelfStore

import socket
import sys
import thread
import MySQLdb
import datetime
import threading
import time
import os
import Housekeep
HOST = ''
PORT = 8950
hKeep_interval = 10




def process_data(data):
	db = MySQLdb.connect("localhost","stockit","stockit","StockItV2")
	cursor = db.cursor()
	data = data.split(":")
	
	if data[0] == "05":
		print data
		now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		sqlq = "SELECT * FROM Shelf_Live WHERE Shelf_ID = '%s'" % int(data[1])
		sqlI = "INSERT INTO Shelf_Live (Shelf_ID, Temp, MassReading, Timestamp) VALUES ('%s', '%s', '%s' ,'%s')" % (int(data[1]), data[2], data[3], now)
		sqlU = "UPDATE Shelf_Live SET Temp='%s', MassReading='%s', Timestamp='%s' WHERE Shelf_ID='%s'" % (data[2], data[3], now, int(data[1]))
		cursor.execute(sqlq)
		results = cursor.fetchone()
		#print "DATA", data
		
		try:
			if results != None:
				#print 'Record Updated'
				cursor.execute(sqlU)
			else:
				#print 'Record Inserted'
				cursor.execute(sqlI)
			#cursor.execute(sqlIH)
			db.commit()

		except Exception, e:
			print str(e)
			print "Rollback", data
			db.rollback()
		cursor.execute(sqlq)
		results = cursor.fetchone()
		db.commit()

		sqlIH = "INSERT INTO Shelf_History (Shelf_ID, Temp, MassReading, NFC0, NFC1, NFC2, NFC3, NFC4, NFC5, NFC6, NFC7, Timestamp) VALUES ('%s', '%s', '%s' ,'%s', '%s', '%s', '%s' ,'%s','%s', '%s', '%s', '%s')" % (int(results[0]), results[1], results[2], results[3], results[4], results[5], results[6], results[7], results[8], results[9], results[10], results[11])
		cursor.execute(sqlIH)
		db.commit()

	elif data[0] == "06":
		print data
		now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		sqlU = "UPDATE Shelf_Live SET NFC%s='%s', Timestamp='%s' WHERE Shelf_ID='%s'" % (data[2], data[3], now, data[1])

		try:
			cursor.execute(sqlU)
				
			#cursor.execute(sqlIH)
			db.commit()
		except Exception, e:
			print str(e)
			print "Rollback", data
			db.rollback()	

	db.close()
	

def count_tags(cursor, Shelf_ID):
	sqlq = "SELECT * FROM Tag_Info WHERE Shelf_ID = '%s'" % Shelf_ID
	cursor.execute(sqlq)
	results = cursor.fetchone()
	print results

def clientthread(conn):
	#conn.send("Welcome to the Server")

	while True:
		
		data = conn.recv(1024)
		#print len(data)
		#print data
		process_data(data)
		reply = "ACK" #ACK here
		if not data:
			break

		conn.sendall(reply + ":[end]@")
	conn.close()

def Housekeep_w():
	print "Housekeep"
	Housekeep.main()	
	threading.Timer(10, Housekeep_w).start()
	#Get UID List
	#See if timed out

def check_timeout(db, cursor):
	sqlq = "SELECT * FROM Tag_Info"
	
	cursor.execute(sqlq)
	results = cursor.fetchall()
	now = datetime.datetime.now()
	interval = datetime.timedelta(seconds = 60)
	for i in results:
		print now - i[3], now - i[3]>interval
		if now - i[3]>interval:
			sqlU = "UPDATE Tag_Info SET Location='%s', Timestamp='%s' WHERE Tag_ID='%s'" % (0, now, int(i[0]))
			print sqlU
			try:
				cursor.execute(sqlU)
				print "Housekeep Update"
				db.commit()
			except:
				print "HKeep Rollback"
				db.rollback()


##Main##

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


print "Socket Created"

try:
	s.bind((HOST, PORT))
except socket.error as msg:
	print "Bind Failed - Error: " + str(msg[0]) + " Message: " + msg[1]
	sys.exit()

print "Socket Bind Complete"

s.listen(10)
print "Socket now Listening"
	
Housekeep_w()
previousTime = 0
while True:

	conn, addr = s.accept()
	print "Connected with " + addr[0] + ":" + str(addr[1])
	thread.start_new_thread(clientthread ,(conn,))
	


