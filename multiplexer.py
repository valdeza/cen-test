import argparse
import socket
import string
import sys
from network_translation import *

# class move_information(object):
# 	x = 0
# 	y = 0
# 	orientation = 0
# 	attribute = ""

# 	def __init__(self,x,y,orientation,attribute):
# 		self.x = x
# 		self.y = y
# 		self.orientation = orientation
# 		self.attribute = attribute

def move_information(x,y,orientation,attribute):
	make_move = move_information(x,y,orientation,attribute)
	return make_move


def better_recv(sockObj, buf):
	if "\r\n" not in buf:
		buf = buf + sockObj.recv(4096)
	split = string.split(buf, "\r\n")
	message = split[0]
	del split[0]
	buf = "\r\n".join(map(str, split))
	return message, buf

# ***** TOURNAMENT PROTOCOL *****
def tournamentOver(sockObj, buf):
	#Server: THANK YOU FOR PLAYING! GOODYE
	message, buf = better_recv(sockObj, buf)
	print message
	return buf	

# ***** AUTHENTICATION PROTOCOL *****	
def authentication(sockObj, TournamentPassword, Username, Password, buf):
	#Server: THIS IS SPARTA!
	message, buf = better_recv(sockObj, buf)
	print message

	#Client: JOIN <tournament password>
	sockObj.send("JOIN " + TournamentPassword + "\r\n")

	#Server: HELLO!
	message, buf = better_recv(sockObj, buf)
	print message

	#Client: I AM <username> <password>
	sockObj.send("I AM " + Username + " " + Password + "\r\n")

	#Server: WELCOME <pid> PLEASE WAIT FOR THE NEXT CHALLENGE
	message, buf = better_recv(sockObj, buf)
	print message
	return buf

# ***** CHALLENGE PROTOCOL *****	
def challengeStart(sockObj, buf):
	#Server: NEW CHALLENGE <cid> YOU WILL PLAY <rounds> MATCH
	# or
	#Server: NEW CHALLENGE <cid> YOU WILL PLAY <rounds> MATCHES
	message, buf = better_recv(sockObj, buf)
	print message
        challenges = int(str.split(message, " ")[6])
	return buf, challenges
	
def challengeOver(sockObj, buf):
	#Server: END OF CHALLENGES
	# or
	#Server: PLEASE WAIT FOR THE NEXT CHALLENGE TO BEGIN
	message, buf = better_recv(sockObj, buf)
	print message
	return buf	

# ***** ROUND PROTOCOL *****	
def roundStart(sockObj, buf):
	#Server: BEGIN ROUND <rid> OF <rounds>
	message, buf = better_recv(sockObj, buf)
        rounds = int(str.split(message, " ")[4])
	return buf, rounds
	
def roundOver(sockObj, buf):
	#Server: END ROUND <rid> OF <rounds>
	# or
	#Server: END ROUND <rid> OF <rounds> PLEASE WAIT FOR THE NEXT MATCH
	message, buf = better_recv(sockObj, buf)
	print message
	return buf	

# ***** MATCH PROTOCOL *****	
def matchStart(sockObj, buf, clientSocks):
	#Server: YOUR OPPONENT IS PLAYER <pid>
	message, buf = better_recv(sockObj, buf)

	#Server: STARTING TILE IS <tile> AT <x> <y> <orientation>
	message, buf = better_recv(sockObj, buf)
	tile = str.split(message, " ")[3]
	print message

	translate_tile_network_to_native(tile)

	#Server: THE REMAINING <number_tiles> TILES ARE [ <tiles> ]
	message, buf = better_recv(sockObj, buf)
        numtiles = int(str.split(message, " ")[2])
        for x in (0, numtiles - 1):
            for y in clientSocks:
                y.send(translate_tile_network_to_native(
                    str.split(message, " ")[x + 6])
                )
	print message

	#Server: MATCH BEGINS IN <time_plan> SECONDS
	message, buf = better_recv(sockObj, buf)
	print message
	return buf, numtiles
	
def matchOver(sockObj, buf):
	#Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
	message, buf = better_recv(sockObj, buf)
	print message

	#Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
	message, buf = better_recv(sockObj, buf)
	print message
        return buf	

def send_move(socket, tile, x, y, rotation, meeple):
    chosen_socket.send("GAME " + str(recieved_gameid)
            + " MOVE " + str(move_number) + " PLACE " + str(tile)
            + " AT " + x + " " + y + " " + rotation + "\r\n")

def get_other_player_move(sockObj, buf, player):
    message, buf = better_recv(sockObj, buf)
    gameid = str.split(message, " ")[1]
    movenum = str.split(message, " ")[3]
    rplayer = str.split(message, " ")[5]
    if (rplayer != player):
        message, buf = better_recv(sockObj, buf)
        gameid = str.split(message, " ")[1]
        movenum = str.split(message, " ")[3]
        rplayer = str.split(message, " ")[5]
    tile = str.split(message, " ")[7]
    x = str.split(message, " ")[9]
    y = str.split(message, " ")[10]
    rot = str.split(message, " ")[11]
    meeple = str.split(message, " ")[12]
    pos = ''
    if (meeple != "NONE"):
        pos = str.split(message, " ")[13]
    return gameid, movenum, player, tile, x, y, rot, meeple, pos, buf

# ***** MOVE PROTOCOL *****	
def move(sockObj, buf, clientSockets, IdList, first):
    byteMe = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])

    #Sent to active player
    #Server: MAKE YOUR MOVE IN GAME <gid> WITHIN <time_move> SECOND: MOVE <#> PLACE <tile>
    # or
    #Server: MAKE YOUR MOVE IN GAME <gid> WITHIN <time_move> SECONDS: MOVE <#> PLACE <tile>
    if (first == 1): # First move special case.
        message, buf = better_recv(sockObj, buf)
        recieved_gameid = string.split(message, " ")[5]
        if recieved_gameid not in IdList:
            IdList.append(recieved_gameid)
        chosen_socket = clientSockets[IdList.index(recieved_gameid)]
        recieved_movenum = string.split(message, " ")[10]
        recieved_tile = string.split(message, " ")[12]
        translated_tile = translate_tile_network_to_native(recieved_tile)
        move_time = int(string.split(message, " ")[7])

        byteMe[0] = 0x01 # I'm first
        byteMe[1] = int(round(move_time * 0.85, 1))
        chosen_socket.send(byteMe)
        byteMe[0] = 0x00 # I'm Second
        clientSockets[1].send(byteMe)
        for x in translated_tile:
            byteMe[x] = translated_tile[x]

        chosen_socket.send(byteMe)
        recieved_tile = bytearray(chosen_socket.recv(17))
        native_tile = bytearray(recieved_tile[0], recieved_tile[1],
                recieved_tile[2], recieved_tile[3], recieved_tile[4],
                recieved_tile[5])

        tile = translate_tile_native_to_network(native_tile);
        x = str(int(recieved_tile[6]) - 76)
        y = str(int(recieved_tile[7]) - 76)
        rotation = str(((4 - int(recieved_tile[8])) % 4) * 90)
        meeple = 'NONE'
        send_move(sockObj, tile, x, y, rotation, meeple)
        first = 0
        return first, buf

    if recieved_gameid not in IdList:
        IdList.append(recieved_gameid)

    chosen_socket = clientSockets[IdList.index(recieved_gameid)]

    #Sent only by the active player
    #Client: GAME <gid> MOVE <#> PLACE <tile> AT <x> <y> <orientation> NONE 
    # or 
    #Client: GAME <gid> MOVE <#> PLACE <tile> AT <x> <y> <orientation> CROCODILE
    # or 
    #Client: GAME <gid> MOVE <#> PLACE <tile> AT <x> <y> <orientation> TIGER <zone>
    # or 
    #Client: GAME <gid> MOVE <#> PLACE <tile> UNPLACEABLE PASS
    # or 
    #Client: GAME <gid> MOVE <#> PLACE <tile> UNPLACEABLE RETRIEVE TIGER AT <x> <y>
    # or 
    #Client: GAME <gid> MOVE <#> PLACE <tile> UNPLACEABLE ADD ANOTHER TIGER TO <x> <y>
    # Not first move general case.

    gameid, movenum, player, tile, x, y, rot, meeple, pos, buf = \
        get_other_player_move(sockObj, buf, player)

    message, buf = better_recv(sockObj, buf)
    recieved_tile = string.split(message, " ")[12]

    byteMe[0] = 0x00 # Game continuing 
    native_tile = translate_tile_network_to_native(recieved_tile)
    native_tile_prev = translate_tile_network_to_native(tile)
    for x in native_tile:
        byteMe[x + 1] = native_tile[x]
    for x in native_tile_prev:
        byteMe[x + 1 + 6] = native_tile_prev[x]

    byteMe[13] = str(int(x) + 76)
    byteMe[14] = str(int(y) + 76)
    byteMe[15] = str((4 - int(rot / 90)) % 4)
    if meeple == 'NONE':
        byteMe[16] = byteMe[17] = str(-1)
    chosen_socket.send(byteMe)

    native_tile = bytearray(chosen_socket.recv(17))
    network_tile = translate_tile_native_to_network(native_tile)
    x = str(int(native_tile[6]) - 76)
    y = str(int(native_tile[7]) - 76)
    rotation = str(((4 - int(recieved_tile[8])) % 4) * 90)
    meeple = 'NONE'
    send_move(sockObj, network_tile, x, y, rotation, meeple)
    return buf

parser = argparse.ArgumentParser(description='Multiplexer')
parser.add_argument('host_arg', type = str, help='Host')
parser.add_argument('remote_port_arg', type = int, help='Host Port')
parser.add_argument('local_port_arg', type = int, help='Local Port')
parser.add_argument('teamName', type = str, help='Local Port')
args = parser.parse_args()

#Host = '192.168.1.19'             # Make a command line option.
Host = args.host_arg
Port = args.remote_port_arg
Port2 = args.local_port_arg
TeamName = args.teamName
TournamentPassword = "TIGERZONE"   # Mhmm
Username = "TEAM" + TeamName       # Mhmm
Password = "IAM" + TeamName        # Mhmm

listener = socket.socket( ) 	# default opts
listener.bind(('localhost', Port2))
listener.listen(2)
client1, addr = listener.accept()
client2, addr = listener.accept()
clients = [client1, client2]

s = socket.socket( )
s.connect((Host, Port))

buf = ''
buf = authentication(s, TournamentPassword, Username, Password, buf)
buf, matches = challengeStart(s, buf)
for a in range (0, matches):
    buf, rounds = roundStart(s, buf)
    #if (rounds % 2 + 1):
    #    rounds -= 1
    for b in range (0, rounds):
        buf, numtiles = matchStart(s, buf, clients)
        first = 1
        IdList = []
        for c in range (0, numtiles):
            buf = move(s, buf, clients, IdList, first)
        buf = matchOver(s, buf)
    buf = roundOver(s, buf)
buf = challengeOver(s, buf)
buf = tournamentOver(s, buf)
