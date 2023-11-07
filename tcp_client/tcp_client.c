#include "tcp_client.h"

#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

static const char *TAG = "TCP_CLIENT";

int tcp_client_init(const char* host_ip, uint16_t port)
{
    int addr_family = 0;
    int ip_protocol = 0;
    int sock;

    struct sockaddr_in dest_addr;
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    while(1) {
        // Attempt to create the socket
        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGI(TAG, "Unable to create socket, retrying socket creation in 1 second...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // Attempt to connect
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGI(TAG, "Socket unable to connect, retrying socket creation in 1 second...");
            close(sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        break;
    }
    ESP_LOGI(TAG, "Successfully connected");
    return sock;
}

bool tcp_client_send(int sock, const char* data)
{
    int err = send(sock, data, strlen(data), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return false;
    }

    return true;
}

bool tcp_client_receive(int sock, char* data, size_t max_len)
{
    int len = recv(sock, data, max_len, 0);
    // Error occurred during receiving
    if (len < 0) {
        ESP_LOGE(TAG, "recv failed: errno %d", errno);
        return false;
    }

    return true;
}