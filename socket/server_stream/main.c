#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

int main() {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;

    if (getifaddrs(&ifap) == -1) {
        perror("Không thể lấy địa chỉ IP");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
            if (strcmp(ifa->ifa_name, "ens33") == 0) {
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                char* ipAddr = inet_ntoa(sa->sin_addr);
                printf("Địa chỉ IP của ens33: %s\n", ipAddr);
                break;
            }
        }
    }

    freeifaddrs(ifap);

    return 0;
}
