#include <iostream>
#include <winsock2.h>
#include <thread>
#include <conio.h>
#include <bits/stdc++.h>
#include <mutex>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

class Node{
    public:
    long long val;
    Node* left;
    Node* right;

    Node(long long value){
        val = value;
        left = nullptr;
        right = nullptr;
    }
};

//global variables
// create BST root node
Node* root = nullptr;
// flag for checking if any client was connected to this server after server is up and running
bool neverConnected = true;
//connection count
int connections = 0;
bool active = true;
mutex mtx;

void insert(Node*& root, long long value){
    if(root == nullptr){
        root = new Node(value);
        return;
    }

    Node* node = root;

    while(node->left != nullptr or node->right != nullptr){
        if(value < node->val){
            node = node->left;
        }else{
            node = node->right;
        }
    }

    if(value < node->val){
        node->left = new Node(value);
    }else{
        node->right = new Node(value);
    }
}

//print tree
void print(Node*& node){
    if (node == NULL){
        return;
    }
    print(node->left);
    cout << node->val << " ";
    print(node->right);
}

void handleClient(SOCKET clientSocket)
{
    connections++;
    neverConnected = false;
    char buffer[12];
    // Receive and print incoming data
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == 0 or bytesReceived == SOCKET_ERROR)
        {
            //cerr << "Connection closed or receive error. " << connections << ::endl;
            closesocket(clientSocket);
            break;
        }

        if (bytesReceived != sizeof(buffer))
        {
            cerr << "Received incomplete packet." << std::endl;
            closesocket(clientSocket);
            break;
        }

        int exch;
        long long stockname;

        memcpy(&exch, buffer, sizeof(exch));
        memcpy(&stockname, buffer + sizeof(exch), sizeof(stockname));

        // std::cout << "Received data from client!!" << endl;
        // cout << "Exchange number: " << exch << endl;
        // cout << "Stock name: " << stockname << endl;

        //if exchange number is 0 close the socket
        if(stockname == 0){
            break;
        }

        //insert data in BST
        lock_guard<mutex> lock(mtx);
        insert(root, stockname);

        // // Echo back the received data
        // send(clientSocket, buffer, bytesRead, 0);

        memset(buffer, 0, sizeof(buffer));
    }
    connections--;
    if(connections == 0) {
        active = false;
        cout << "printing..." << endl;
        print(root);
        _getch();
    }
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create the socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to a specific IP address and port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Use any available network interface
    serverAddress.sin_port = htons(12345);      // Use port 12345

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen for connections." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started. Listening for connections..." << std::endl;

    // Accept incoming connections and handle them in separate threads
    while (active)
    {
        // if(connections == 0 and neverConnected == false){
        //     break;
        // }
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept connection." << std::endl;
            break;
        }
        // Handle client connection in a separate thread
        thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    //cout << "printing..." << endl;
    // print data
    //print(root);
    //cout << "node value" << (root == NULL) << " " << root->val;

    // Close the listening socket
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();

    _getch();

    return 0;
}
