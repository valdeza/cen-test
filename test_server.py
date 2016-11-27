import sys
import socket
import string

def authentication(sockObj, TournamentPassword, authDictionary):
    sockObj.send("THIS IS SPARTA!\r\n")

    buf = '' + sockObj.recv(1024)
    recieved_password = string.split(string.split(buf, "\r\n")[0], " ")[1]
    if recieved_password == TournamentPassword:
        print "Good Password\n"
    else:
        return 1;

    sockObj.send("Hello!\r\n")

    buf = '' + sockObj.recv(1024)
    recieved_username = string.split(string.split(buf, "\r\n")[0], " ")[2];
    recieved_password = string.split(string.split(buf, "\r\n")[0], " ")[3];
    if recieved_password == authDictionary[recieved_username]:
        print "Authenticated " + recieved_username + "\n"
    else:
        print "Bad client\n"

Host = ""
Port = 50001
TournamentPassword = "derp"
authDictionary = dict([('red', 'baron'), ('herpderp', 'derp')])
PregameTime = 15		# 15 seconds
MoveTimeLimit = 1		# Time per move

s = socket.socket( ) 	# default opts
s.bind((Host, Port))
s.listen(1)
conn, addr = s.accept() # What is addr here?

authentication(conn, TournamentPassword, authDictionary)
