Runs on Linux environment 
Compile with gcc server.c -o server
./server -d directory -p port -u passwordFile
Note: Password file(password.txt) should be in the same directory as server
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
DEL <filename>
 used to delete the specified file 
 so if i should send DEL test it deletes test file in the running directory and sends a 200 message if file cant be found it sends a 404
 PUT <filename>
  puts text into the specified file and it also(not in append mode so it doesnt save existing file content,it overwrites it)
  if file doesnt exist it creates file and puts text
  it gives a 200 start receiving message to the client 
  client types in content he wants to put and clicks enter 
  client signifies end of file using a . in a line on its own
  server gives 200 message if save is successful and 400 if it is not
  
  
                                                                                                                                              BY RABIU AMIR RABIU
                                                                                                                                              21900084
                                                                                                                                              CMPE431 TERM PROJECT
  
 
