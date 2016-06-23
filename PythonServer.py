# Jonathan Rawlinson - M0ZJO - 2016

import socket
import sys
import thread
import MySQLdb
import datetime
import threading
HOST = ''
PORT = 8950

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


def process_data(data):
	db = MySQLdb.connect("localhost","stockit","stockit","Shelf_Data")
	cursor = db.cursor()
	data = data.split(":")
	if data[0] == "01":
		print data
		now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		sqlq = "SELECT * FROM Shelf_Data WHERE Shelf_ID = '%s'" % data[1]
		sqlI = "INSERT INTO Shelf_Data (Shelf_ID, Temp, Humidity, Mass, Timestamp) VALUES ('%s', '%s', '%s', '%s' ,'%s')" % (data[1], data[2], data[3], data[4], now)
		sqlU = "UPDATE Shelf_Data SET Temp='%s', Humidity='%s', Mass='%s', Timestamp='%s' WHERE Shelf_ID='%s'" % (data[2], data[3], data[4], now, int(data[1]))
		sqlIH = "INSERT INTO Shelf_Data_Hist (Shelf_ID, Temp, Humidity, Mass, Timestamp) VALUES ('%s', '%s', '%s', '%s' ,'%s')" % (data[1], data[2], data[3], data[4], now)
		cursor.execute(sqlq)
		results = cursor.fetchone()
		#print "DATA", data[1]
		#print results
		try:
			if results != None:
				print 'Record Updated'
				cursor.execute(sqlU)

			else:
				print 'Record Inserted'
				cursor.execute(sqlI)
				
			cursor.execute(sqlIH)
			db.commit()
		except:
			print "Rollback"
			db.rollback()


	elif data[0] == "02":
		print data
		now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

		sqlq = "SELECT * FROM Tag_Info WHERE Tag_ID = '%s'" % data[1]

		sqlI = "INSERT INTO Tag_Info (Tag_ID, Shelf_ID, Location, Timestamp) VALUES ('%s', '%s', '%s', '%s')" % (data[1], data[2], data[3], now)
		sqlU = "UPDATE Tag_Info SET Shelf_ID='%s', Location='%s', Timestamp='%s' WHERE Tag_ID='%s'" % (data[2], data[3], now, int(data[1]))
		sqlIH = "INSERT INTO Tag_Info_Hist (Tag_ID, Shelf_ID, Location, Timestamp) VALUES ('%s', '%s', '%s', '%s')" % (data[1], data[2], data[3], now)
		cursor.execute(sqlq)
		results = cursor.fetchone()
		#print "DATA", data[1]
		print results
		try:
			if results != None:
				print 'Record Updated'
				cursor.execute(sqlU)
			
			else:
				print 'Record Inserted'
				cursor.execute(sqlI)

			cursor.execute(sqlIH)
			db.commit()
		except:
			db.rollback()
			print "Rollback"
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

def Housekeep():
	print "Housekeep"
	#Get UID List
	#See if timed out
	threading.Timer(5, Housekeep).start ()

while True:
	Housekeep()
	conn, addr = s.accept()
	print "Connected with " + addr[0] + ":" + str(addr[1])
	thread.start_new_thread(clientthread ,(conn,))

s.close()
