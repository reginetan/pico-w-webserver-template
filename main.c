#include "lwip/apps/httpd.h" // for the web server
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "cgi.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/ip4_addr.h"

// Mobile Hotspot Credentials
const char WIFI_SSID[] = "onsen"; // Replace with your hotspot's SSID
const char WIFI_PASSWORD[] = "urfatherlesbian"; // Replace with your hotspot's password

int main()
{
    // Initialize standard input/output
    stdio_init_all();
    sleep_ms(3000);
    printf("Initializing Wi-Fi connection...\n");

    // Initialize Wi-Fi chip (CYW43) on the Pico W
    if (cyw43_arch_init()) {
        printf("Failed to initialize Wi-Fi chip\n");
        return -1;
    }
    
    printf("Wi-Fi chip initialized successfully\n");

    // Enable Wi-Fi station mode (client mode)
    cyw43_arch_enable_sta_mode();
    printf("Wi-Fi station mode enabled\n");

    // Attempt to connect to the Wi-Fi network
    int attempts = 0;
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        attempts++;
        printf("Attempt %d: Failed to connect, retrying...\n", attempts);

        if (attempts >= 5) { // Limit the number of retries
            printf("Failed to connect after 5 attempts. Exiting.\n");
            cyw43_arch_deinit(); // Clean up and exit
            return -1;
        }

        sleep_ms(5000); // Wait 5 seconds before retrying
    }

    // Print a success message once connected
    printf("Connected to Wi-Fi network '%s'!\n", WIFI_SSID);

    // Get the network interface
    struct netif *netif = netif_list;

    // Check if DHCP has assigned an IP address
    if (netif_is_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
        printf("IP Address: %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
    } else {
        printf("Failed to get IP address\n");
    }

    // Start HTTP server (if needed)
    httpd_init();
    printf("HTTP server initialized\n");

   
    cgi_init();
    printf("CGI Handler initialized\n");

    // Infinite loop to keep the program running
    while (1)
    {
        sleep_ms(1000); // Add a small delay to avoid busy-waiting
    }

    return 0;
}
