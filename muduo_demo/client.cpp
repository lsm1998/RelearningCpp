#include <arpa/inet.h>
#include <array>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <thread>

constexpr const int CLIENT_NUM = 100;
constexpr const int SERVER_PORT = 2007;
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr const char* MESSAGE = "Hello from client";

void sendEcho(int client_id)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Client " << client_id << ": socket creation failed\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Client " << client_id << ": invalid address\n";
        close(sock);
        return;
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Client " << client_id << ": connection failed\n";
        close(sock);
        return;
    }

    send(sock, MESSAGE, strlen(MESSAGE), 0);
    std::cout << "Client " << client_id << " sent: " << MESSAGE << std::endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0)
    {
        std::cout << "Client " << client_id << " received: " << buffer << std::endl;
    }
    else
    {
        std::cerr << "Client " << client_id << ": receive failed or server closed\n";
    }

    close(sock);
}

int main()
{
    std::cout << "开始发送连接请求" << "\n";

    std::array<std::thread, CLIENT_NUM> workers;

    for (int i = 0; i < CLIENT_NUM; ++i)
    {
        workers[i] = std::thread(sendEcho, i);
    }

    for (auto& t : workers)
    {
        if (t.joinable())
            t.join();
    }
    return 0;
}
