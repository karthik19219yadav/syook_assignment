#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#define TAG_ADDRESS "XX:XX:XX:XX:XX:XX"  // Replace with your BLE Tag's address

void parse_accelerometer_data(char *packet, short *acceleration) {
    // Parse accelerometer data from the packet
    // You might need to adjust the byte indices based on your specific data format
    sscanf(packet, "%4hx%4hx%4hx", acceleration[0], acceleration[1], acceleration[2]);
}

int is_moving(short *acceleration) {
    // Your logic to determine if the tag is moving or stationary
    // Example: Check if the sum of absolute values of accelerometer readings is above a threshold
    int i, threshold;
    int sum = 0;
    for (i = 0; i < 3; i++) {
        sum = abs(acceleration);
    }
    return sum > threshold;
}

int main() {
    struct sockaddr_rc addr = {0};
    int s, status;
    char dest[18];

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(TAG_ADDRESS, &addr.rc_bdaddr);

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    if (status == 0) {
        char buffer[1024];
        short acceleration[3];

        while (1) {
            // Receive raw data packet
            status = read(s, buffer, sizeof(buffer));
            if (status < 0) {
                perror("Read failed");
                break;
            }

            // Parse accelerometer data
            parse_accelerometer_data(buffer, acceleration);

            // Check if the tag is moving
            if (is_moving(acceleration)==0) {
                printf("Tag is moving\n");
            } else {
                printf("Tag is stationary\n");
            }
        }

        // Close the Bluetooth connection
        close(s);
    } else {
        perror("Connection failed");
    }

    return 0;
}
