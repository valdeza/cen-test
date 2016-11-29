import sys
import socket
import string

#class move_information(object):
#x = 0
#y = 0
#orientation = 0
#attribute = ""

#def __init__(self,x,y,orientation,attribute):
#self.x = x
#self.y = y
#self.orientation = orientation
#self.attribute = attribute

#def move_information(x,y,orientation,attribute):
#make_move = move_information(x,y,orientation,attribute)
#return make_move


def better_recv(sockObj, buf):
    if "\r\n" not in buf:
        buf = buf + sockObj.recv(4096)
    split = string.split(buf, "\r\n")
    message = split[0]
    del split[0]
    buf = "\r\n".join(map(str, split))
    return message, buf

def authentication(sockObj, TournamentPassword, Username, Password, buf):
    message, buf = better_recv(sockObj, buf)
    print message

    sockObj.send("JOIN " + TournamentPassword + "\r\n")

    message, buf = better_recv(sockObj, buf)
    print message

    sockObj.send("I AM " + Username + " " + Password + "\r\n")

    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def challengeStart(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def roundStart(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def matchStart(sockObj, buf):
    print "opponent is hey"
    message, buf = better_recv(sockObj, buf)
    print message

    print "starting tile hey"
    message, buf = better_recv(sockObj, buf)
    print message

    print "remaing hey"
    message, buf = better_recv(sockObj, buf)
    print message

    print "match hey"
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def move(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message

    recieved_gameid = string.split(message, " ")[5]
    print "game id: " + recieved_gameid
    
    move_number = 1;
    tile = 1;
    move_info = 1 #move_information()
    sockObj.send("GAME " + str(recieved_gameid) + " MOVE " + str(move_number)
            + " PLACE " + str(tile) + " AT " + str(move_info) + "\r\n")
    
    message, buf = better_recv(sockObj, buf)
    print message
    
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def matchOver(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message

    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def roundOver(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def challengeOver(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

def tournamentOver(sockObj, buf):
    message, buf = better_recv(sockObj, buf)
    print message
    return buf

Host = ""                     # Make a command line option.
Port = 50001                  # This too
TournamentPassword = "derp"   # Mhmm
Username = "herpderp"         # Mhmm
Password = "derp"             # Mhmm

s = socket.socket( )
s.connect((Host, Port))

buf = ''

buf = authentication(s, TournamentPassword, Username, Password, buf)
buf = challengeStart(s, buf)
buf = roundStart(s, buf)
buf = matchStart(s, buf)
buf = move(s, buf)


