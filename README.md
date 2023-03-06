This repo is demo of some key aspects of a P2P network

It mainly focuses on the aspect of having a tracking server that keeps tracks of clients.

To run the demo first build the client and server by running the build_server.sh script and the build_client.sh script. Note the IP of the server. This needs to be provided to the client program when requested. 

There are some important things to note if you are runnning the script in WSL;
    If you are running WSL 2:
        You can only be the client. You cannot run the server code on WSL 2 due to chnaged microsoft made to the way network adapters work in WSL 2.
    
    If you are running WSL 1 or Linux:
        You should be able to run both scripts.

Recommended Way to run these files:
RUN THIS IN WSL 1 OR IN LINUX. THE SERVER WILL NOT RUN IN WSL 2
For server:
 1. Open up a terminal on one laptop and run "chmod +x build_server.sh". This marks the build script as executable
 2. On another laptop, open a terminal and run "chmod +x build_client.sh". This marks the build script as executable
 3. Run the build scripts on each laptop.
 4. Run the server script first by running "./server" in the terminal where you built the server. Note the IP address that gets printed.
 5. Run the client script by running "./client" in the terminal where you built the client script. Put in the server IP and a unique identifier when promted.
 