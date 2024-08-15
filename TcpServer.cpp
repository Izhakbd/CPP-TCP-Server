#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
using namespace std;

void handleClient(int client_sock) {
    
    std::cout << "Client connected." << std::endl;
    // run until client said he wants to disconnect
    while (true)
        {
        char buffer[4096];
        int expected_data_len = sizeof(buffer);
        int read_bytes = recv(client_sock, buffer, expected_data_len, 0);//wait dor client to send data.
        if (read_bytes == 0) { //client ask for disconnection
            std::cout << "Client disconnected." << std::endl;
            close(client_sock);
            break;  // Break the inner loop to accept a new connection
        } else if (read_bytes < 0) {
            perror("error receiving data");
            close(client_sock);
            break;  // Break the inner loop on error to accept a new connection
        } else {
            std::cout << buffer << std::endl;
            int sent_bytes = send(client_sock, buffer, read_bytes, 0); //send data to client.
            if (sent_bytes < 0) {
                perror("error sending to client");
            }
        }
    }
   }

int main() {
    const int server_port = 5555;//define server port
    int sock = socket(AF_INET, SOCK_STREAM, 0); //create main server socket
    if (sock < 0) {
        perror("error creating socket");
        return 1;
    }

    //define server stuff
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    //bind socket to port
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("error binding socket");
        close(sock);
        return 1;
    }

    //listen for incoming connections
    if (listen(sock, 5) < 0) {
        perror("error listening to socket");
        close(sock);
        return 1;
    }

    std::cout << "Server is listening on port " << server_port << std::endl;
    std::vector<std::thread> client_threads; // Create vector who hold all client's threads. 


    //server running and wait for client to connect
    while (true){
        sleep(1);
        std::cout << "Waiting for client to connect" << std::endl;
        struct sockaddr_in client_sin;
        unsigned int addr_len = sizeof(client_sin);
        int client_sock = accept(sock, (struct sockaddr *)&client_sin, &addr_len);  //accept incoming connection
        if (client_sock < 0) {
            perror("error accepting client");
            close(sock);
            return 1;
        }

        client_threads.push_back(thread(handleClient, client_sock));// create and add current client thread to the vector        
    }
   
    // Join all client threads before shutting down
    for (auto& t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    close(sock);  // Close the server socket
    return 0;
     
}
