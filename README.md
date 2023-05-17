# 431FTPProject
Project for CMPE431 By Amir Rabiu
./server -d directory -p port -u passwordFile
Commands:
USER <username> <password>
  to login
  it checks for the given username and password in the passwordFile specified using the -u command line arguement
LIST
  to list files in the directory given with the -d command line arguement
QUIT  
  to close server
GET <filename>
  gets content of specified file
  so for instance if you say GET password.txt it gives you 
    

