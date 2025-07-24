// This C++ console application demonstrates basic TCP client functionality using Winsock2.
// It is designed to be compatible with Windows 7.
// IMPORTANT: This is NOT a VPN application and does NOT provide any security or tunneling features.
// It only shows how to establish a basic network connection and send/receive data.

// --- IMPORTANT WINSOCK HEADER FIX ---
// Define WIN32_LEAN_AND_MEAN to exclude rarely-used stuff from Windows headers,
// which can speed up compilation and reduce conflicts.
#define WIN32_LEAN_AND_MEAN
// Define _WINSOCKAPI_ to prevent windows.h from including the older winsock.h.
// This ensures that only winsock2.h (which we explicitly include) is used.
#define _WINSOCKAPI_
// --- END WINSOCK HEADER FIX ---

#include <iostream> // For standard input/output operations (e.g., std::cout, std::cerr)
#include <string>   // For using std::string
#include <windows.h> // General Windows API functions. Must be included AFTER _WINSOCKAPI_ if using Winsock2.
#include <winsock2.h> // Core Winsock 2.2 functions.
#include <ws2tcpip.h> // For modern TCP/IP functions like getaddrinfo.

// Link with Ws2_32.lib for Winsock functions
#pragma comment(lib, "Ws2_32.lib")

// Function to set console text color (Windows-specific)
// This helps to visually brand the console output.
void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Reset console text color to default
void resetConsoleColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int main() {
    // Set the console window title to reflect the application's brand.
    SetConsoleTitleA("Celestei CONNECT");

    // Display the branded header for the application.
    std::cout << "****************************************" << std::endl;
    std::cout << "* *" << std::endl;
    setConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Bright Blue color for "celestei"
    std::cout << "* Celestei CONNECT            *" << std::endl;
    resetConsoleColor(); // Reset color to default
    std::cout << "* *" << std::endl;
    std::cout << "****************************************" << std::endl;
    std::cout << std::endl;

    std::cout << "Initializing network client..." << std::endl;

    // --- Winsock Initialization ---
    WSADATA wsaData; // Structure to hold Winsock data
    // Initialize Winsock 2.2. MAKEWORD(2,2) requests version 2.2.
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "ERROR: WSAStartup failed with error: " << iResult << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        getchar();
        return 1; // Indicate an error occurred
    }
    std::cout << "Winsock initialized successfully." << std::endl;

    // --- Server Configuration ---
    // Define the server's IP address and port.
    // YOU MUST REPLACE "127.0.0.1" with the actual IP address of your VPS server (celestei.freevps.xyz).
    // YOU MUST REPLACE "8080" with the port your server application is listening on.
    const char* SERVER_IP = "127.0.0.1"; // Placeholder: Replace with your VPS IP (e.g., "YOUR_VPS_IP_HERE")
    const char* SERVER_PORT = "8080";    // Placeholder: Replace with your server's listening port

    SOCKET connectSocket = INVALID_SOCKET; // Socket for connecting to the server
    struct addrinfo *result = NULL, *ptr = NULL, hints; // Structures for address information

    // Zero out the hints structure to ensure no garbage values.
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6 address
    hints.ai_socktype = SOCK_STREAM; // Specify a stream socket (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Specify TCP protocol

    // Resolve the server address and port.
    // getaddrinfo translates hostnames/service names into socket addresses.
    iResult = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "ERROR: getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup(); // Clean up Winsock resources
        std::cout << "Press any key to exit..." << std::endl;
        getchar();
        return 1;
    }

    // --- Attempt to Connect to Server ---
    // Loop through all address results returned by getaddrinfo and try to connect.
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to the server.
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket == INVALID_SOCKET) {
            std::cerr << "ERROR: Socket creation failed with error: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result); // Free the address info structure
            WSACleanup();
            std::cout << "Press any key to exit..." << std::endl;
            getchar();
            return 1;
        }

        std::cout << "Attempting to connect to " << SERVER_IP << ":" << SERVER_PORT << "..." << std::endl;
        // Connect to the server.
        iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connectSocket); // Close the failed socket
            connectSocket = INVALID_SOCKET; // Mark as invalid to try next address
            continue; // Try the next address in the list
        }
        break; // Successfully connected, exit loop
    }

    freeaddrinfo(result); // Free the memory allocated by getaddrinfo

    // Check if a connection was successfully established.
    if (connectSocket == INVALID_SOCKET) {
        std::cerr << "ERROR: Unable to connect to server at " << SERVER_IP << ":" << SERVER_PORT << "!" << std::endl;
        WSACleanup();
        std::cout << "Press any key to exit..." << std::endl;
        getchar();
        return 1;
    }

    std::cout << "Successfully connected to " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    // --- Send Data to Server ---
    const char* sendbuf = "Hello from Celestei CONNECT Client!"; // Message to send
    // Send the message over the socket.
    iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "ERROR: send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(connectSocket);
        WSACleanup();
        std::cout << "Press any key to exit..." << std::endl;
        getchar();
        return 1;
    }
    std::cout << "Sent " << iResult << " bytes: \"" << sendbuf << "\"" << std::endl;

    // --- Receive Data from Server (Optional) ---
    char recvbuf[512]; // Buffer to hold received data
    int recvbuflen = 512; // Size of the receive buffer

    std::cout << "Waiting to receive data from server..." << std::endl;
    // Receive data from the server.
    iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        recvbuf[iResult] = '\0'; // Null-terminate the received data to treat it as a C-string
        std::cout << "Received " << iResult << " bytes: \"" << recvbuf << "\"" << std::endl;
    } else if (iResult == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        std::cerr << "ERROR: recv failed with error: " << WSAGetLastError() << std::endl;
    }

    // --- Shutdown and Cleanup ---
    // Shutdown the connection for sending; no more data will be sent from client.
    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "ERROR: shutdown failed with error: " << WSAGetLastError() << std::endl;
    }

    closesocket(connectSocket); // Close the socket
    WSACleanup(); // Clean up Winsock resources

    std::cout << "Client finished. Press any key to exit..." << std::endl;
    getchar(); // Pause console to allow user to read output

    return 0; // Indicate successful execution
}
