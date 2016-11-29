import sys
import socket
import string

buf = ''

def better_recv(sockObj, buf):
    if "\r\n" not in buf:
        buf = buf + sockObj.recv(4096)
    split = string.split(buf, "\r\n")
    message = split[0]
    del split[0]
    buf = "\r\n".join(map(str, split))
    return buf, message

def authentication(sockObj, TournamentPassword, authDictionary):
    sockObj.send("THIS IS SPARTA!\r\n")

    buf = ''
    buf, message = better_recv(sockObj, buf)
    print message
    recieved_password = string.split(message, " ")[1]
    if recieved_password == TournamentPassword:
        print "Good Password\n"
    else:
        print "Bad Password\n"
        return 1, buf


    sockObj.send("Hello!\r\n")

    buf, message = better_recv(sockObj, buf)
    recieved_username = string.split(message, " ")[2];
    recieved_password = string.split(message, " ")[3];
    if recieved_password == authDictionary[recieved_username]:
        print "Authenticated " + recieved_username + "\n"
    else:
        print "Bad client\n"

    sockObj.send("WELCOME " + recieved_username + " PLEASE WAIT FOR THE NEXT CHALLENGE\r\n")

    return 0, buf


def challengeStart(sockObj):
    send_challengeid = 1 # we need a global variable
    send_rounds = 1      # we need a global variable
    sockObj.send("NEW CHALLENGE " + str(send_challengeid) + " YOU WILL PLAY " + str(send_rounds) + " MATCH\r\n")

def roundStart(sockObj):
    send_roundid = 1    # we need a glo var
    send_rounds = 1     # we need a glo var
    sockObj.send("BEGIN ROUND " + str(send_roundid) + " OF " + str(send_rounds) + "\r\n")

def matchStart(sockObj):
    send_username = 1 #glo var
    sockObj.send("YOUR OPPONENT IS PLAYER " + str(send_username) + "\r\n")

    send_tile = 1 # glo var
    send_x = 1 # glo var
    send_y = 1 # glo var
    send_orientation = 1 # glo var
    sockObj.send("STARTING TILE IS " + str(send_tile) + " AT " + str(send_x) + " " + str(send_y) + " "+ str(send_orientation) + "\r\n")

    send_numb_tiles = 1 # glo var
    send_tiles_left = 1 # glo var
    sockObj.send("THE REMAINING " + str(send_numb_tiles) + " TILES ARE [" + str(send_tiles_left) + "]\r\n")

    send_time_plan = 1 # glo var
    sockObj.send("MATCH BEGINS IN " + str(send_time_plan) + " SECONDS\r\n")

def move(sockObj):
    send_gameid = 1 # glo var
    send_time_move = 1 # glo var
    send_move_number = 1 # glo var
    send_tile = 1 # glo var
    send_playerid = 1 # glo var
    send_move_information = 1 # glo

    sockObj.send("MAKE YOUR MOVE IN GAME " + str(send_gameid) + " WITHIN " + str(send_time_move) + " SECOND: MOVE " + str(send_move_number) + " PLACE " + str(send_tile) + "\r\n")
    buf, message = better_recv(sockObj, '')
    recieved_gameid = string.split(message, " ")[1]
    recieved_move_number = string.split(message, " ")[3]
    recieved_tile = string.split(message, " ")[5]
    recieved_move_information = string.split(message, " ")[7]

    sockObj.send("GAME " + str(send_gameid) +
            " MOVE " + str(send_move_number) +
            " PLAYER " +  str(send_playerid) +
            " PLACED " + str(send_tile) +
            " AT " + str(send_move_information) + "\r\n")
    sockObj.send("GAME " + str(send_gameid) +
            " MOVE " + str(send_move_number) +
            " PLAYER " +  str(send_playerid) +
            " PLACED " + str(send_tile) +
            " AT " + str(send_move_information) + "\r\n")

def matchOver(sockObj):
    send_gameid = 1; # glo
    send_playerid = 1; # glo
    send_score = 1; # glo

    sockObj.send("GAME " + str(send_gameid) +
            " OVER PLAYER " + str(send_playerid) +
            " " + str(send_score) +
            " PLAYER " + str(send_playerid) +
            " " + str(send_score) + "\r\n")
    sockObj.send("GAME " + str(send_gameid) +
            " OVER PLAYER " + str(send_playerid) +
            " " + str(send_score) +
            " PLAYER " + str(send_playerid) +
            " " + str(send_score) + "\r\n")

def roundOver(sockObj,buf):
    send_roundid = 1 # glo
    send_rounds = 1 # glo

    sockObj.send("END OF ROUND " + str(send_roundid)
            + " OF " + str(send_rounds) + "\r\n")

def challengeOver(sockObj,buf):
    sockObj.send("END OF CHALLENGES\r\n")

def tournamentOver(sockObj,buf):
    sockObj.send("THANK YOU FOR PLAYING! GOODBYE\r\n")


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


derp = authentication(conn, TournamentPassword, authDictionary)
challengeStart(conn)
roundStart(conn)
matchStart(conn)
move(conn)
