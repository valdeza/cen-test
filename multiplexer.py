import sys
import socket
import string

class move_information(object):
    x = 0
    y = 0
    orientation = 0
    attribute = ""

    def __init__(self,x,y,orientation,attribute):
        self.x = x
        self.y = y
        self.orientation = orientation
        self.attribute = attribute

def move_information(x,y,orientation,attribute):
    make_move = move_information(x,y,orientation,attribute)
    return make_move

def authentication(sockObj, TournamentPassword, Username, Password):
    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    sockObj.send("JOIN " + TournamentPassword + "\r\n")

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    sockObj.send("I AM " + Username + " " + Password + "\r\n")

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def challengeStart(sockObj):

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def roundStart(sockObj):
    
    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def matchStart(sockObj):

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def move(sockObj, gameid, move_number, tile, move_information):

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    sockObj.send("GAME " + gameid + " MOVE " + move_number + " PLACE " + tile + " AT " + move_information + "\r\n")

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def matchOver(sockObj):

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def roundOver(sockObj):
    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def challengeOver(sockObj):
    
    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

def tournamentOver(sockObj):

    buf = '' + sockObj.recv(1024)
    for message in string.split(buf, "\r\n"):
        field = string.split(message, " ")
        print field

Host = ""                     # Make a command line option.
Port = 50001                  # This too
TournamentPassword = "derp"   # Mhmm
Username = "herpderp"         # Mhmm
Password = "derp"             # Mhmm

s = socket.socket( )
s.connect((Host, Port))

authentication(s, TournamentPassword, Username, Password)
challengeStart(s)
roundStart(s)
matchStart(s)


