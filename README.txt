./server -d directory -p port -u passwordFile
Commands:
USER <username> <password>
  to login
  it checks for the given username and password in the passwordFile specified using the -u command line arguement
LIST
  to list files in the directory given with the -d command line arguement
   it terminates it with a full stop i.e .
QUIT
  it sends "Goodbye!" to the client then closes the server
GET <filename>
  gets content of specified file
  so for instance if you say GET password.txt it gives you 
   amir:iamamir12
   alice:qwerty
   bob:2021.sockets
   devrim:seral
  and terminates it with "\r\n.\r\n"
  the <filename> should be in the -d specified directory
 
