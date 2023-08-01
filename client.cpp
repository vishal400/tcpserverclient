#include <iostream>
#include <winsock2.h>
#include <conio.h>  // Include the conio.h header for _getch()
#include <bits/stdc++.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create the socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // Use the server's IP address
    serverAddress.sin_port = htons(12345);  // Use the same port as the server

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Loop to handle multiple requests
    while (true) {
        // Prompt the user for input or read data from another source
        int exchangeNumber;
        long long stockName;
        cout << "Enter exchange number: ";
        cin >> exchangeNumber;
        cout << "Enter stock name(number): ";
        cin >> stockName;

        if (stockName == 0) {
            break;  // Exit the loop if the user enters 'q'
        }

        // Send data to the server
        char buffer[12];
        memcpy(buffer, &exchangeNumber, sizeof(exchangeNumber));
        memcpy(buffer + sizeof(exchangeNumber), &stockName, sizeof(stockName));

        if(send(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR){
            std::cerr << "Failed to send data." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

    }

    // Close the socket
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();

    std::cout << "Press any key to exit...";
    _getch();  // Wait for a key press before exiting

    return 0;
}
