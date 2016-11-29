import sys
import socket
import string
import select

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
    sockObj.setblocking(0)
    ready = select.select([sockObj], [], [], 0.75)
    if ready[0]:
        buf = buf + sockObj.recv(4096)
    return buf

def print_buffer(buf):
    for message in [string.split(buf, "\r\n")[0]]:
        field = string.split(message, " ")
        print field
    split = string.split(buf, "\r\n")
    del split[0]
    return "\r\n".join(map(str,split))


def authentication(sockObj, TournamentPassword, Username, Password, buf):
    buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)

    sockObj.send("JOIN " + TournamentPassword + "\r\n")

    buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)

    sockObj.send("I AM " + Username + " " + Password + "\r\n")

    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    return buf

def challengeStart(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    return buf

def roundStart(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    #print "roundstart heyyyyy"
    buf = print_buffer(buf)
    return buf

def matchStart(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    print "opponent is hey"
    buf = print_buffer(buf)

    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    print "starting tile hey"
    buf = print_buffer(buf)

    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    print "remaing hey"
    buf = print_buffer(buf)

    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    print "match hey"
    buf = print_buffer(buf)
    return buf

def move(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    recieved_gameid = string.split(string.split(buf, "\r\n")[0], " ")[5]
    buf = print_buffer(buf)

    print "game id: " + recieved_gameid
    
    move_number = 1;
    tile = 1;
    move_info = 1 #move_information()
    sockObj.send("GAME " + str(recieved_gameid) + " MOVE " + str(move_number) + " PLACE " + str(tile) + " AT " + str(move_info) + "\r\n")
    
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    return buf

def matchOver(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)

    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)

    return buf

def roundOver(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    return buf

def challengeOver(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
    return buf

def tournamentOver(sockObj, buf):
    buf = buf + better_recv(sockObj, buf)
    #buf = buf + sockObj.recv(1024)
    buf = print_buffer(buf)
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


