#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <array>
#include <algorithm> 
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <optional>

#define PORT 12346

std::array<std::optional<int>, 2> clients; // Fixed size for 2 players, using optional for empty slots
std::array<float, 1078> game_state;          // Server time + Player 1 coords + Player 2 coords
std::mutex state_mutex;                   // Mutex to protect `game_state`
std::mutex client_mutex;                  // Mutex to protect `clients`
bool server_running = true;

// Function to broadcast the game state
void broadcast_game_state() {
    std::ostringstream oss;

    {
        std::lock_guard<std::mutex> lock(state_mutex);
        // Format game state into a comma-separated string
        for (size_t i = 0; i < game_state.size(); ++i) {
            if (i > 0) {
                oss << ",";
            }
            oss << std::fixed << std::setprecision(2) << game_state[i];
        }
    }

    std::string message = oss.str();

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        for (const auto& client : clients) {
            if (client.has_value()) {
                if (send(client.value(), message.c_str(), message.length(), 0) < 0) {
                    std::cerr << "Error sending to client: " << client.value() << std::endl;
                }
            }
        }
    }
}

// Function to handle a client
void handle_client(int client_socket, int player_index) {
    std::cout << "Player " << player_index + 1 << " connected: " << client_socket << std::endl;

    while (server_running) {
        char buffer[16384] = {0};
        int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_read <= 0) {
            break; // Client disconnected or error occurred
        }

        try {
            float x, y, z;

            // Parse coordinates in the format "x,y,z"
            if (sscanf(buffer, "%f,%f,%f", &x, &y, &z) == 3) {
                std::lock_guard<std::mutex> lock(state_mutex);
                game_state[1 + player_index * 3] = x;
                game_state[2 + player_index * 3] = y;
                game_state[3 + player_index * 3] = z;

                std::cout << "Updated Player " << player_index + 1 << " position: ("
                          << x << ", " << y << ", " << z << ")" << std::endl;
            } else {
                std::cerr << "Invalid input from Player " << player_index + 1 << ": " << buffer << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing input from Player " << player_index + 1 << ": " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    close(client_socket);

    {
        std::lock_guard<std::mutex> lock(client_mutex);
        clients[player_index] = std::nullopt; // Free up the player's slot
    }

    {
        std::lock_guard<std::mutex> lock(state_mutex);
        game_state[1 + player_index * 3] = 0.0f; // Reset coordinates to 0
        game_state[2 + player_index * 3] = 0.0f;
        game_state[3 + player_index * 3] = 0.0f;
    }

    std::cout << "Player " << player_index + 1 << " disconnected: " << client_socket << std::endl;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Initialize game state: server time, player 1 coords, player 2 coords
    /*
    
    INDEX | Purpose
    0     | server time
    1-6   | Players coordinates
    7     | Increments
    8     | Random number up to 10
    10-73 | Tile food amounts
    74-77 | Agent coordinates
    78-1077 | 3D grid


    */
    {
        std::lock_guard<std::mutex> lock(state_mutex);
        game_state.fill(0.0f);
        for (int i=0; i<2; i++){
            game_state[i * 2 + 74] = 42.0f;
            game_state[i * 2 + 75] = 2.0f;
        }

        for (int i=78; i<1078; i++) {
            float randFloat = static_cast<float>(std::rand() % 100) / 100.0f; // Random value
            if (randFloat < 0.5f) {
                float randFloat2 = static_cast<float>(std::rand() % 100) / 100.0f;
                if (randFloat2 < 0.5f) {
                    game_state[i] = 1.0f;
                } else {
                    game_state[i] = 0.3f;
                }
                
            }
        }
    }

    // Initialize the client slots
    {
        std::lock_guard<std::mutex> lock(client_mutex);
        clients = {std::nullopt, std::nullopt};
    }

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

    // Thread to broadcast game state
    std::thread broadcaster([&]() {
        while (server_running) {
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - start_time;

            {
                std::lock_guard<std::mutex> lock(state_mutex);
                game_state[0] = static_cast<float>(elapsed_time.count()); // Update server time
            }

            broadcast_game_state();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });

    std::thread game_state_manager([&]() {
        while (server_running) {
            {
                std::lock_guard<std::mutex> lock(state_mutex);
                
                // Example: Update game_state values (indexes 5 and 6) with dummy logic
                game_state[7] += 1.0f; // Increment a value
                game_state[8] = static_cast<float>(std::rand() % 100) / 10.0f; // Random value
                int randomNumber = 10 + std::rand() % (73 - 10);
                game_state[randomNumber] = static_cast<float>(std::rand() % 10000) / 1000.0f;
                for (int i=0; i<2; i++){
                    game_state[i * 2 + 74] += static_cast<float>(std::rand() % 1000) / 1000.0f - 0.5f;
                    if (game_state[i * 2 + 74] > 48.0f) {
                        game_state[i * 2 + 74] = 48.0f;
                    } else if (game_state[i * 2 + 74] < 40.0f) {
                        game_state[i * 2 + 74] = 40.0f;
                    }
                    game_state[i * 2 + 75] += (static_cast<float>(std::rand() % 2000) / 1000.0f - 1.0f) * 0.1f;
                    if (game_state[i * 2 + 75] > 8.0f) {
                        game_state[i * 2 + 75] = 8.0f;
                    } else if (game_state[i * 2 + 75] < 0.0f) {
                        game_state[i * 2 + 75] = 0.0f;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Wait
        }
    });

    // Main server loop to accept clients
    while (server_running) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket >= 0) {
            std::lock_guard<std::mutex> lock(client_mutex);

            // Find the first available slot
            auto it = std::find(clients.begin(), clients.end(), std::nullopt);
            if (it != clients.end()) {
                int player_index = std::distance(clients.begin(), it);
                clients[player_index] = new_socket;

                std::thread(handle_client, new_socket, player_index).detach();
            } else {
                std::cerr << "Server is full. Rejecting client: " << new_socket << std::endl;
                close(new_socket);
            }
        } else {
            perror("Accept failed");
        }
    }

    // Cleanup
    server_running = false;
    close(server_fd);
    broadcaster.join();

    return 0;
}
