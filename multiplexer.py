import sys
import socket
import string
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
	return buf
	
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
	print message
	return buf
	
def roundOver(sockObj, buf):
	#Server: END ROUND <rid> OF <rounds>
	# or
	#Server: END ROUND <rid> OF <rounds> PLEASE WAIT FOR THE NEXT MATCH
	message, buf = better_recv(sockObj, buf)
	print message
	return buf	

# ***** MATCH PROTOCOL *****	
def matchStart(sockObj, buf):
	#Server: YOUR OPPONENT IS PLAYER <pid>
	message, buf = better_recv(sockObj, buf)
	print message

	#Server: STARTING TILE IS <tile> AT <x> <y> <orientation>
	message, buf = better_recv(sockObj, buf)
	# tile = string.split(message, " ")[3]
	# translate_tile_network_to_native(tile)
	print message

	#Server: THE REMAINING <number_tiles> TILES ARE [ <tiles> ]
	message, buf = better_recv(sockObj, buf)
	print message

	#Server: MATCH BEGINS IN <time_plan> SECONDS
	message, buf = better_recv(sockObj, buf)
	print message
	return buf
	
def matchOver(sockObj, buf):
	#Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
	message, buf = better_recv(sockObj, buf)
	print message

	#Server: GAME <gid> OVER PLAYER <pid> <score> PLAYER <pid> <score>
	message, buf = better_recv(sockObj, buf)
	print message
	return buf	

# ***** MOVE PROTOCOL *****	
def move(sockObj, buf):
	#Sent to active player
	#Server: MAKE YOUR MOVE IN GAME <gid> WITHIN <time_move> SECOND: MOVE <#> PLACE <tile>
	# or
	#Server: MAKE YOUR MOVE IN GAME <gid> WITHIN <time_move> SECONDS: MOVE <#> PLACE <tile>
	message, buf = better_recv(sockObj, buf)
	print message

	recieved_gameid = string.split(message, " ")[5]
	# print "game id: " + recieved_gameid
	recieved_tile = string.split(message, " ")[12]
	translate_tile_network_to_native(recieved_tile)
    

	move_number = 1;
	tile = translate_tile_native_to_network(recieved_tile);
	move_info = 1 #move_information()
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
	sockObj.send("GAME " + str(recieved_gameid) + " MOVE " + str(move_number)
			+ " PLACE " + str(tile) + " AT " + str(move_info) + "\r\n")
    move_number = move_number + 1
	#Sent to both players
	#Server: GAME <gid> MOVE <#> PLAYER <pid> <move> 
	# or
	#Server: GAME <gid> MOVE <#> PLAYER <pid> FORFEITED: <flag>
	# <flag>s include: ILLEGAL TILE PLACEMENT, ILLEGAL MEEPLE PLACEMENT, INVALID MEEPLE PLACMENT, TIMEOUT, ILLEGAL MESSAGE RECEIVED 
	message, buf = better_recv(sockObj, buf)
	print message
    
	#See above
	message, buf = better_recv(sockObj, buf)
	print message
	return buf

Host = ""                     # Make a command line option.
Port = 50001 
Port2 = 50002                # This too
TournamentPassword = "derp"   # Mhmm
Username = "herpderp"         # Mhmm
Password = "derp"             # Mhmm

listener = socket.socket( ) 	# default opts
listener.bind((Host, Port2))
listener.listen(2)
client1, addr = listener.accept()
client2, addr = listener.accept()

s = socket.socket( )
s.connect((Host, Port))


buf = ''

buf = authentication(s, TournamentPassword, Username, Password, buf)
buf = challengeStart(s, buf)
buf = roundStart(s, buf)
buf = matchStart(s, buf)
buf = move(s, buf)
buf = matchOver(s, buf)
buf = roundOver(s, buf)
buf = challengeOver(s, buf)
buf = tournamentOver(s, buf)


