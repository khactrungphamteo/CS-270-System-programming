1. UDPClient.c contains code that performs Type0 message transaction between the client and the test servevr
2. UDPServer parse and sanity-check incoming message from server and prepares appropriate Type0 message to be sent to the 
client
3. a file called output.txt records successful Type0 interaction between client and the test server
4. output.txt also record succesful Type0 transaction between the UDPClient and UDPServer that was included in UDPServer.c file
5. A Makefile that compiles your code into client and server executable files, called UDPclient
and UDPserver, respectively
6. output.txt file will record the output the following format

magic=270 
length= 30
xid=4a 77 cb b3
version=2
flags=0x2 
port=0 
result=0 
variable part=5480417

- first output will reflect succesful interaction between client and test server
- second output will reflect succesful interaction between client and server written by students
