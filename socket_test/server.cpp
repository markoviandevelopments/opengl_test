#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>

#define PORT 8080

std::vector<int> clients; // List of connected client sockets
std::mutex client_mutex;  // Mutex to protect client list
bool server_running = true;

// Function to broadcast messages to all clients
void broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (auto client : clients) {
        send(client, message.c_str(), message.length(), 0);
    }
}

// Function to handle individual client
void handle_client(int client_socket) {
    std::cout << "Client connected: " << client_socket << std::endl;
    while (server_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    close(client_socket);

    {
        std::lock_guard<std::mutex> lock(client_mutex);

    }

    std::cout << "Client disconnected: " << client_socket << std::endl;
}




int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Get server start time
    auto start_time = std::chrono::steady_clock::now();

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    // Bind to address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Thread to broadcast uptime to all clients
    std::thread broadcaster([&]() {
        while (server_running) {
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - start_time;
            std::string message = "Server uptime: " + std::to_string(elapsed_time.count()) + " seconds\n";
            broadcast(message);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // Accept client connections in the main thread
    while (server_running) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket >= 0) {
            {
                std::lock_guard<std::mutex> lock(client_mutex);
                clients.push_back(new_socket);
            }
            std::thread(handle_client, new_socket).detach(); // Handle client in a separate thread
        }
    }

    // Cleanup
    server_running = false;
    close(server_fd);
    broadcaster.join();
    return 0;
}
