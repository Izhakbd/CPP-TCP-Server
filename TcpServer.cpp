#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <list>
#include <jsoncpp/json/json.h> // Include JSON library
#include "Video.cpp"
#include <algorithm> // Include for sort

using namespace std;

list <Video> VideoWatchingUsersList[100]; // List of videos that each user is watching, indexed by user ID ,global variable.
int recommendedVideos[10]; // List of recommended videos for user who report on watching some video,global variable.





// Function to sort each list in VideoWatchingUsersList by views in descending order
void sortVideosByViews() {
    for (int userId = 0; userId < 100; ++userId) { // Iterate over each user ID
        if (!VideoWatchingUsersList[userId].empty()) { // Check if the user's list is not empty
            // Sort the list using a lambda function to compare videos by views
            VideoWatchingUsersList[userId].sort([](const Video& a, const Video& b) {
                return a.getViews() > b.getViews(); // Sort in descending order
            });
        }
    }
}



/*
This function returns a list of recommended videos for a user who is watching a specific video.
*/
int* getRecommendedVideos(int videoId) {
    int i = 0;
    // Implement the recommendation system logic here
    for (int userId = 1; userId < 100; ++userId) { // Iterate over each user ID
        if (VideoWatchingUsersList[userId].empty()) { // Check if the user's list is empty, and continue to the next user if it is.
            continue;

       
        //check if the videoId is found in the user's list
        if (std::any_of(VideoWatchingUsersList[userId].begin(), VideoWatchingUsersList[userId].end(), 
            [videoId](const Video& video) { 
                return video.getId() == videoId; 
            })) {

                for (Video& video : VideoWatchingUsersList[userId]) {//add the users videos who also wathcing the videoId to the recommendedVideos list. 
                    recommendedVideos[i] = video.getId();
                    i++;
                    if (i == 9) {
                        break;
                    }
                }
            }
            
        }
    }
    return recommendedVideos;
}





/*
This function prints the list of videos that each user is watching to the console.
*/
void printAllUsersVideoList() {
    std::cout << "Printing VideoWatchingUsersList for all users:" << std::endl;

    for (int userId = 0; userId < 100; ++userId) { // Iterate over each user ID
        if (!VideoWatchingUsersList[userId].empty()) { // Check if the user's list is not empty
            std::cout << "User ID " << userId << " is watching the following videos:" << std::endl;

            for (Video& video : VideoWatchingUsersList[userId]) {
                std::cout << video << std::endl; // Print each video using the overloaded << operator
            }

            std::cout << std::endl; // Add a newline for readability between users
        }
    }
}



void handleClient(int client_sock) {
    int videoExistInList = 0;

    // Get the user ID from the client
    std::cout << "Client connected." << std::endl;
    // char bufForUserId[4096];
    // int expected_data_len = sizeof(bufForUserId);
    // int read_bytes = recv(client_sock, bufForUserId, expected_data_len, 0);//wait for client to send his userId.
    // int userId = atoi(bufForUserId);
    // if (read_bytes < 0) {
    //     perror("error receiving data");
    //     close(client_sock);
    //     return;
    // }
    // std::cout << "User ID: " << userId << std::endl;

    // run until client said he wants to disconnect (client report on videos he watcing)
    while (true)
        {
        char buffer[4096];
        int expected_data_len = sizeof(buffer);
        int read_bytes = recv(client_sock, buffer, expected_data_len, 0);//wait for client to send data.
        printf("read_bytes: %d\n", read_bytes);
        if (read_bytes == 0) { //client ask for disconnection
            std::cout << "Client disconnected." << std::endl;
            close(client_sock);
            break;  // Break the inner loop to accept a new connection
        } 
        
        else if (read_bytes < 0) {
            perror("error receiving data");
            close(client_sock);
            break;  // Break the inner loop on error to accept a new connection
        } 
        
        else {
            // Parse JSON data from client
            std::string jsonData(buffer, read_bytes);
            Json::Value root;
            Json::Reader reader;

            //if JSON is not valid
            if (!reader.parse(jsonData, root)) {
                std::cerr << "Failed to parse JSON: " << reader.getFormattedErrorMessages();
                continue;
            }

            printf("Received JSON data: %s\n", jsonData.c_str());
            // Extract video data from JSON
            int videoId = root["videoId"].asInt();
            printf("videoId: %d\n", videoId);
            int videoViews = root["videoViews"].asInt();
            printf("videoViews: %d\n", videoViews);
            int userId = root["userId"].asInt();
            printf("userId: %d\n", userId);

            // Print the received video data to the console
            std::cout << "Received Video ID: " << videoId << ", Views: " << videoViews << std::endl;
            std::cout << std::endl; // Add a newline for readability between printing list of all users.

            if (VideoWatchingUsersList[userId].empty()) {// if the user's list is empty add the video to the list
                // Add the video to the appropriate user's list
                VideoWatchingUsersList[userId].push_back(Video(videoId, videoViews));
            } 

            for (Video& video : VideoWatchingUsersList[userId]) {
                if(video.getId() == videoId){ // Check if the video ID already exists in the user's list
                    videoExistInList = 1; //flag that the video exist in the list
                    video.setViews(videoViews);
                    break;
                } 
            }

            if(videoExistInList == 0){ // If the video ID does not exist in the user's list
                // Add the video to the user's list
                VideoWatchingUsersList[userId].push_back(Video(videoId, videoViews));
            }

            printAllUsersVideoList(); // Print the list of videos for all users 
            sortVideosByViews(); // Sort the list of videos for all users by views in descending order
            int* recommendedVideos = getRecommendedVideos(videoId); // Get the recommended videos for the user who reported watching the video

            // Send an acknowledgment to the client
            std::string ack = "Video data received: " + std::to_string(videoId);
            // send(client_sock, ack.c_str(), ack.size(), 0);


            // here we need to implement the logic of recommendation system and return to node.js server ,list of recommended video.

            // Create a JSON object for the response
            Json::Value responseJson;

            // Create a JSON array for recommended videos
            Json::Value recommendedVideosArray(Json::arrayValue);

            // Populate the array with recommended video IDs
            for (int i = 0; i < sizeof(recommendedVideos)/sizeof(int); i++)
            {
                recommendedVideosArray.append(recommendedVideos[i]);
            }

            // Add the array to the response JSON object
            responseJson["recommendedVideos"] = recommendedVideosArray;

            // Convert the JSON object to a string
            Json::FastWriter writer;
            std::string jsonString = writer.write(responseJson);

            // Send the JSON string to the client
            send(client_sock, jsonString.c_str(), jsonString.length(), 0);

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
