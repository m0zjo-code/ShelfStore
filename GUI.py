#!/usr/bin/env python
# my_first_ncurses_menu_ui.py - simple interactive command line
# program generated using 'ncurses ui'.
# This example has four options:
#    > option 1:   prompts for more input
#    > option 2&3: execute basic bash commands.
#    > option 4:   exits back to bash prompt.
# This can be a good base to build a more sophisticated interative program.
 
import os
import curses
import MySQLdb
import datetime
import time
 
 
def get_param(prompt_string):
	"""Brings up new screen asking user to enter a single parameter.	
	Args:
	    prompt_string: Prompt which asks/tells user what to enter (string).
	"""
	screen.clear()
	screen.border(0)
	screen.addstr(2, 2, prompt_string)
	screen.refresh()
	input = screen.getstr(10, 10, 60)
	return input
 
 
def execute_cmd(cmd_string):
  	"""Clears the screen, executes the given system command and shows the result.
 	
  	Args:
  	    cmd_string: Unix/system command to execute.
  	"""
	os.system('clear')
	a = os.system(cmd_string)
	print ''
	if a == 0:
		print 'Command executed correctly: ' + cmd_string
	else:
		print 'Command terminated with error'
		raw_input('Press enter')
		print ''
 
# Init screen with global var:
screen = curses.initscr()
curses.start_color()
 
def main():
  	opt = 0
  	while opt != ord('q'):

		screen.clear()
		screen.border(0)
		screen.addstr(2, 2, 'Please enter a number...')
		screen.addstr(4, 4, 'Live Shelf Info')
		screen.addstr(5, 4, 'Live Tag Info')
		#screen.addstr(6, 4, '3 - Show disk space                (df -h)')
		screen.addstr(7, 4, 'q - Exit')
		screen.refresh()
 	
		opt = screen.getch()  # Wait for user to enter a character.
		#opt = '6'
		if opt == ord('1'):
			names = get_param('Enter your name, or comma-seperated names')
			age = get_param('Enter your age')
			curses.endwin()
			print '\n\nHappy birthday ' + names + '. You are ' + age + '!\n\n'
			raw_input('Press enter (to return to main)')
		if opt == ord('2'):
			tag_screen()
		if opt == ord('3'):
			curses.endwin()
			execute_cmd('df -h')
 
		curses.endwin()

def tag_screen():
	opt = 0
	k = 3
	now = ""
	curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLACK)
	curses.init_pair(2, curses.COLOR_BLUE, curses.COLOR_BLACK)
	curses.init_pair(3, curses.COLOR_CYAN, curses.COLOR_BLACK)
	try:
		while True:
			db = MySQLdb.connect("localhost","stockit","stockit","StockItV2")
			# prepare a cursor object using cursor() method
			cursor = db.cursor()
			sqlq = "SELECT * FROM Tag_Data"
			cursor.execute(sqlq)
			results = cursor.fetchall()
			screen.clear()	
			now_old = now
			now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
			j = 4
			
			screen.border(curses.ACS_VLINE, curses.ACS_VLINE,curses.ACS_DIAMOND, curses.ACS_DIAMOND)
			
			screen.addstr(j-1, 8, "Tag ID   II Temperature II Mass Reading II Location II Timestamp (Last Updated)", curses.color_pair(k))
			#for i in range(0, 20):
			#	screen.addch(3,i,curses.ACS_HLINE)
 			for i in results:
				j = j + 1
				if int(i[3]) == -1:
					location = "Removed"
				elif int(i[3]) >= 0:
					location = int(i[3])
				data = str(i[0]).ljust(8) + " II " + str(i[1]).ljust(11) + " II "  + str(i[2]).ljust(12) + " II "  + str(location).ljust(8) + " II "  + i[4].strftime('%Y-%m-%d %H:%M:%S')
				screen.addstr(j, 8, data)
			
			screen.addstr(j+1, 8, "Current Time: " + now)
			screen.addstr(j+3, 8, "Ctrl-C for Exit!")
			db.close()
			if now != now_old: screen.refresh()
			#opt = screen.getch()
	except KeyboardInterrupt:
		return
 
if __name__ == '__main__':
    main()
