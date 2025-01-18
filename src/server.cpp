#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>

#define PORT 12346

std::vector<int> clients; // List of connected client sockets
std::mutex client_mutex;  // Mutex to protect client list
bool server_running = true;

// Function to broadcast messages to all clients
void broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(client_mutex);
    for (auto it = clients.begin(); it != clients.end();) {
        if (send(*it, message.c_str(), message.length(), 0) < 0) {
            std::cerr << "Error sending to client: " << *it << ". Removing client." << std::endl;
            close(*it); // Close the socket
            it = clients.erase(it); // Remove client from list
        } else {
            ++it;
        }
    }
}

// Function to handle individual client
void handle_client(int client_socket) {
    std::cout << "Client connected: " << client_socket << std::endl;
    while (server_running) {
        char buffer[1024] = {0};
        int bytes_read = recv(client_socket, buffer, 1024, 0);
        if (bytes_read <= 0) {
            break; // Client disconnected or error occurred
        }
        std::cout << "Received from client " << client_socket << ": " << buffer << std::endl;
    }

    close(client_socket);

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        //clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
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
        std::string last_message = ""; // Keep track of the last message sent

        while (server_running) {
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - start_time;

            // Format the elapsed time with fixed precision
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << elapsed_time.count();
            std::string message = oss.str();

            // Ensure the message is different from the last message
            if (message != last_message) {
                broadcast(message);
                last_message = message;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
