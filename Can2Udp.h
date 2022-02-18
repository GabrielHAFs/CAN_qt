#ifndef _CAN2UDP_H

#define _CAN2UDP_H
#define VERBOSE(...) if (verbose) fprintf(stderr, __VA_ARGS__)
#define ERROR(...)  syslog(LOG_USER|LOG_ERR, __VA_ARGS__)

extern int verbose;

int open_can(char* bus);
int loop(struct sockaddr_in *saddr, int cansocket, int udpsocket);
int open_udp(struct sockaddr_in *saddr, char* address, int port);
int get_address(struct sockaddr_in *saddr, char* address, int port);

#endif /* _CAN2UDP_H */