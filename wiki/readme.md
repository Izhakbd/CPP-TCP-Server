# Compiling the TCP and running it

First , compile the TcpServer.cpp file with the command : "g++ TcpServer.cpp -ljsoncpp -o TcpServer"
![צילום מסך 2024-09-04 101334](https://github.com/user-attachments/assets/6d77bca1-bdb1-4eb3-bde9-7bee0bf9740a)

This command will create exe file called "TcpServer".
![צילום מסך 2024-09-04 101400](https://github.com/user-attachments/assets/85820233-4f4c-48cd-aaf7-5f7b64b51842)

run this file with the command : ./TcpServer.
![צילום מסך 2024-09-04 101413](https://github.com/user-attachments/assets/f6a0fe56-0077-4aca-827e-b4a774e74888)

Great! Now the Tcp server in on and wait for clients.

The server listening in port 5555.

# Runnig the nodeJs server

Inorder to run this code - you should load to the mongo the json files - to have default users and videos.

Before that you need to download all the moudles of the npm - to do that write in CL npm install.
![צילום מסך 2024-09-04 102401](https://github.com/user-attachments/assets/43483bf2-78e6-42b2-8b95-96ebae72ce5f)

For load the data - first you need to open mongo compass and connect.

Then you need to run the server application by writing to the CL 'npm start' , that will initialize the database in mogno compass - this is not loading the default users and videos, that will done manualy.

Now - load manualy the data to the mongo schemas - to do that - take this files and in mongo, in test/users click on add data and load from this json file -
wiki/user (2).json .
Do the same for the videos (but under test/videos) wiki/vid (2).json .
![צילום מסך 2024-09-04 102740](https://github.com/user-attachments/assets/d279d82a-f78b-4d59-bb95-6b694eb667c2)
![צילום מסך 2024-09-04 102728](https://github.com/user-attachments/assets/e58b496e-3205-49b4-bbad-d488924dc98f)
![צילום מסך 2024-09-04 102700](https://github.com/user-attachments/assets/e5303aff-9642-4ce3-9feb-1b41d377ffb6)
![צילום מסך 2024-09-04 102640](https://github.com/user-attachments/assets/91ffb277-6b82-4e30-b800-34c4183c81c4)
![צילום מסך 2024-09-04 102545](https://github.com/user-attachments/assets/44704f03-f3bd-4575-9d91-8033051c3ee1)

Than you should run the server by writing in CL 'npm start' and then you should open in your browser this URL - http://localhost:12345/ .
![צילום מסך 2024-09-04 102808](https://github.com/user-attachments/assets/aaf93944-6819-4324-be24-d321d8c3aa51)

# Andorid 

The code for part 3 is under branch part3Main.
the implemntion of API for work with the server is under api directory (in src->main->java->api).
clone the project and run him from part3Main branch.

***important note:***

there maybe some functionality that not working in the second try (for example ,if you try to login and you put some wrong details, the server send you error mesaage and if you try to fix the wrong detail and send again to server it's sometimes send you again that somthing wrong despite the details are correct).

in this case please run whole app again and now things are work fine.

***Dont conclude from the first attempt that things do not work.***



To work in your network please enter to res->values->strings.xml ,and change ths string called "BaseUrl" ,in the part of ip definition (after "http") ,to your ip address in your network.

**Dont** change the Port number. For example:
<string name="BaseUrl">http://111.111.11.111:12345/api/</string>
the network is http://111.111.11.111 and the port is 12345. Change obly the 111.111.11.111 to your network (find by cmd).

Now - load manualy the data to the mongo schemas - to do that - take this file wiki/videos.json
 and insert to mongo schema tests/videos as described.
-- if you check the part 2 (Web) too ,**Dont** use the same videos there to this part!! , ***its not working.***
You need to run the server as described.

**So for conclusion follow this order:**

1.load the videos.json to mongo.

2.run the server.

3.run the Android Application.
