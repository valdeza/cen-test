import sys
import socket
import string

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

Host = ""                     # Make a command line option.
Port = 50001                  # This too
TournamentPassword = "derp"   # Mhmm
Username = "herpderp"         # Mhmm
Password = "derp"             # Mhmm

s = socket.socket( )
s.connect((Host, Port))

authentication(s, TournamentPassword, Username, Password)
