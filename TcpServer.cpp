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

list<Video> VideoWatchingUsersList[100]; // List of videos that each user is watching, indexed by user ID ,global variable.
int recommendedVideos[10] = {0};         // Initialize the global array
int userIdFlag = -1;                     // List of recommended videos for user who report on watching some video,global variable.

// Function to sort each list in VideoWatchingUsersList by views in descending order
void sortVideosByViews()
{
    for (int userId = 0; userId < 100; ++userId)
    { // Iterate over each user ID
        if (!VideoWatchingUsersList[userId].empty())
        { // Check if the user's list is not empty
            // Sort the list using a lambda function to compare videos by views
            VideoWatchingUsersList[userId].sort([](const Video &a, const Video &b)
                                                {
                                                    return a.getViews() > b.getViews(); // Sort in descending order
                                                });
        }
    }
}

// send the list to the client
void sendList(int client_sock)
{
    if (userIdFlag == -1)
    {
        send(client_sock, "", 0, 0);
    }
    // Serialize the array into a buffer
    char buffer[sizeof(recommendedVideos)];
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, recommendedVideos, sizeof(recommendedVideos));

    // Send the buffer to the client
    ssize_t bytesSent = send(client_sock, buffer, sizeof(buffer), 0);
    if (bytesSent < 0)
    {
        perror("send");
        // Handle send error
    }
    else if (bytesSent != sizeof(buffer))
    {
        fprintf(stderr, "Warning: Not all data was sent\n");
        // Handle partial send
    }
}

// update the global
void updateRecommendedVideos(int videoId)
{
    int i = 0;
    // Clear the global array before populating it
    std::fill(std::begin(recommendedVideos), std::end(recommendedVideos), 0);

    for (int userId = 1; userId < 100; ++userId)
    {
        if (VideoWatchingUsersList[userId].empty())
        {
            continue;
        }

        // Check if the videoId is found in the user's list
        if (std::any_of(VideoWatchingUsersList[userId].begin(), VideoWatchingUsersList[userId].end(),
                        [videoId](const Video &video)
                        {
                            return video.getId() == videoId;
                        }))
        {
            for (const Video &video : VideoWatchingUsersList[userId])
            {
                if (i < 10) // Ensure we do not exceed the array bounds
                {
                    recommendedVideos[i] = video.getId();
                    i++;
                }
                else
                {
                    break; // Exit the loop if we have added 10 videos
                }
            }
        }

        if (i >= 10)
        {
            break; // Exit the outer loop if we have added 10 videos
        }
    }
}

/*
This function returns a list of recommended videos for a user who is watching a specific video.
*/
int *getRecommendedVideos(int videoId)
{
    int i = 0;
    // Clear the recommendedVideos array before populating it
    std::fill(std::begin(recommendedVideos), std::end(recommendedVideos), 0); // notice: Added to clear the array

    for (int userId = 1; userId < 100; ++userId)
    {
        if (VideoWatchingUsersList[userId].empty())
        {
            continue;
        }

        // Check if the videoId is found in the user's list
        if (std::any_of(VideoWatchingUsersList[userId].begin(), VideoWatchingUsersList[userId].end(),
                        [videoId](const Video &video)
                        {
                            return video.getId() == videoId;
                        }))
        {
            for (const Video &video : VideoWatchingUsersList[userId])
            {
                if (i < 10) // Ensure we do not exceed the array bounds //notice: Added bound check
                {
                    recommendedVideos[i] = video.getId();
                    i++;
                }
                else
                {
                    break; // Exit the loop if we have added 10 videos //notice: Added break condition
                }
            }
        }

        if (i >= 10)
        {
            break; // Exit the outer loop if we have added 10 videos //notice: Added break condition
        }
    }

    return recommendedVideos;
}

/*
This function prints the list of videos that each user is watching to the console.
*/
void printAllUsersVideoList()
{
    std::cout << "Printing VideoWatchingUsersList for all users:" << std::endl;

    for (int userId = 0; userId < 100; ++userId)
    { // Iterate over each user ID
        if (!VideoWatchingUsersList[userId].empty())
        { // Check if the user's list is not empty
            std::cout << "User ID " << userId << " is watching the following videos:" << std::endl;

            for (Video &video : VideoWatchingUsersList[userId])
            {
                std::cout << video << std::endl; // Print each video using the overloaded << operator
            }

            std::cout << std::endl; // Add a newline for readability between users
        }
    }
}

void handleClient(int client_sock)
{
    int videoExistInList = 0;

    // Get the user ID from the client
    std::cout << "Client connected." << std::endl;

    // run until client said he wants to disconnect (client report on videos he watcing)
    while (true)
    {
        char buffer[4096] = {0}; // notice: Initialize buffer to zero
        int expected_data_len = sizeof(buffer);
        int read_bytes = recv(client_sock, buffer, expected_data_len, 0);
        printf("read_bytes: %d\n", read_bytes);
        if (read_bytes == 0)
        {
            std::cout << "Client disconnected." << std::endl;
            close(client_sock);
            break;
        }
        else if (read_bytes < 0)
        {
            perror("error receiving data");
            close(client_sock);
            break;
        }
        else
        {
            // Parse JSON data from client
            std::string jsonData(buffer, read_bytes);
            Json::Value root;
            Json::Reader reader;

            if (!reader.parse(jsonData, root))
            {
                std::cerr << "Failed to parse JSON: " << reader.getFormattedErrorMessages();
                continue;
            }

            printf("Received JSON data: %s\n", jsonData.c_str());

            std::string requestType = root["requestType"].asString();
            printf("%s\n", requestType.c_str());
            if (requestType == "giveMeList")
            {
                sendList(client_sock);
            }
            else
            {
                // Extract video data from JSON
                if (!root.isMember("videoId") || !root.isMember("videoViews") || !root.isMember("userId")) // notice: Added checks for JSON members
                {
                    std::cerr << "Invalid JSON format: missing required fields" << std::endl;
                    continue;
                }

                int videoId = root["videoId"].asInt();
                int videoViews = root["videoViews"].asInt();
                int userId = root["userId"].asInt();
                userIdFlag = userId;

                printf("videoId: %d\n", videoId);
                printf("videoViews: %d\n", videoViews);
                printf("userId: %d\n", userId);

                if (userId >= 0 && userId < 100) // notice: Added range check for userId
                {
                    // ... (rest of the code remains the same)
                }
                else
                {
                    std::cerr << "Invalid userId: " << userId << std::endl;
                }
            }
        }
    }
}

int main()
{
    const int server_port = 5555;               // define server port
    int sock = socket(AF_INET, SOCK_STREAM, 0); // create main server socket
    if (sock < 0)
    {
        perror("error creating socket");
        return 1;
    }

    // define server stuff
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    // bind socket to port
    if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("error binding socket");
        close(sock);
        return 1;
    }

    // listen for incoming connections
    if (listen(sock, 5) < 0)
    {
        perror("error listening to socket");
        close(sock);
        return 1;
    }

    std::cout << "Server is listening on port " << server_port << std::endl;
    std::vector<std::thread> client_threads; // Create vector who hold all client's threads.

    // server running and wait for client to connect
    while (true)
    {
        sleep(1);
        std::cout << "Waiting for client to connect" << std::endl;
        struct sockaddr_in client_sin;
        unsigned int addr_len = sizeof(client_sin);
        int client_sock = accept(sock, (struct sockaddr *)&client_sin, &addr_len); // accept incoming connection
        if (client_sock < 0)
        {
            perror("error accepting client");
            close(sock);
            return 1;
        }

        client_threads.push_back(thread(handleClient, client_sock)); // create and add current client thread to the vector
    }

    // Join all client threads before shutting down
    for (auto &t : client_threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    close(sock); // Close the server socket
    return 0;
}
