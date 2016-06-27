import MySQLdb
import datetime

def update(db, cursor, UID, Temp, Mass, Location):
	now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
	sqlq = "SELECT * FROM Tag_Data WHERE Tag_ID = '%s'" % int(UID)
	sqlI = "INSERT INTO Tag_Data (Tag_ID,Temp,Mass,Location,Timestamp) VALUES ('%s','%s', '%s', '%s' ,'%s')" % (int(UID), Temp, Mass, Location, now)
	sqlU = "UPDATE Tag_Data SET Temp='%s', Mass='%s', Location='%s', Timestamp='%s' WHERE Tag_ID='%s'" % (Temp, Mass, Location, now, int(UID))
	cursor.execute(sqlq)
	results = cursor.fetchone()
	print results
	
	#print sqlI
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
		print "Rollback"
		db.rollback()


	
def UID_Search(lst, val):
	j = 0;
	for i in lst:
		#print lst[j], val
		if i == val:
			return j-3
		j = j+1
	print "Search error"
	return -2

def history_gen(db, cursor, UID, timestamp_old, action):
	sqlq = "SELECT * FROM Tag_Data WHERE Tag_ID = '%s'" % int(UID)
	cursor.execute(sqlq)
	results = cursor.fetchone()
	print "TS", timestamp_old
	sqlIH = "INSERT INTO Tag_Data_History (Tag_ID,Temp,Mass,Location,Timestamp,Action) VALUES ('%s','%s', '%s', '%s' ,'%s' ,'%s')" % (results[0], results[1], results[2],results[3], timestamp_old, action)
	try:
		cursor.execute(sqlIH)
		db.commit()
	except Exception, e:
		print str(e)
		print "Rollback"
		db.rollback()

# Open database connection
db = MySQLdb.connect("localhost","stockit","stockit","StockItV2")

# prepare a cursor object using cursor() method
cursor = db.cursor()
# Prepare SQL query to INSERT a record into the database.
sql = "SELECT * FROM Shelf_History"
try:
   # Execute the SQL command
   cursor.execute(sql)
   # Fetch all the rows in a list of lists.
   results = cursor.fetchall()
   for x in range(0, len(results)-1):
	#print "-------------"
	#print results[x]
	#print results[x+1]

	delta1 = list(set(results[x+1][3:10]) - set(results[x][3:10]))
	#print "Added", delta1
	
	delta2 = list(set(results[x][3:10]) - set(results[x+1][3:10]))
	#print "Removed", delta2
	
	
	if len(delta1)+len(delta2) > 1:
		print "Error - too many changes"
	elif len(delta1) == 1:
		Mass = results[x+1][2]-results[x][2]
		print "Added", "UID", delta1[0], "Mass Delta" , Mass
		#print Mass, delta1
		#print results
		Location = UID_Search(results[x+1], delta1[0])
		update(db, cursor, delta1[0], results[x][1], Mass, Location)
		history_gen(db, cursor, delta1[0], results[x+1][11], "Added")

	elif len(delta2) == 1:
		Mass = -(results[x+1][2]-results[x][2])
		print "Removed", "UID", delta2[0], "Mass Delta" , results[x+1][2]-results[x][2]
		update(db, cursor, delta2[0], results[x][1], Mass, -1)
		history_gen(db, cursor, delta2[0], results[x+1][11], "Removed")
		
	print "-------------", results[x][11]
except KeyboardInterrupt, e:
	print "Exception"
	print str(e)

# disconnect from server
db.close()





