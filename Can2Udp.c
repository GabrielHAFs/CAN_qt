#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/can.h>
#include <sys/uio.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <syslog.h>

#include "Can2Udp.h"

#define MAX(i, j) ((i>j)?i:j)

int open_can(char * bus)
{
	int s;
	struct sockaddr_can addr;
	struct ifreq ifr;

	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0)
	{
		ERROR("%m: socket can");
		return -1;
	}
	strcpy(ifr.ifr_name, bus );
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0)
	{
		ERROR("%m: ioctl can");
		return -1;
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		ERROR("%m: bind can");
		return -1;
	}

	return s;
}

int loop(struct sockaddr_in *saddr, int cansocket, int udpsocket)
{
	fd_set readfds;
	int nf = MAX(cansocket, udpsocket)+1;
	struct can_frame frame;
	char buf[14];

	FD_ZERO(&readfds);

	for (;;)
	{
		FD_SET(cansocket, &readfds);
		FD_SET(udpsocket, &readfds);
		if (select(nf, &readfds, NULL, NULL, NULL) < 0)
		{
			ERROR("%m: select");
			return -1;
		}
		if(FD_ISSET(cansocket, &readfds))
		{
			if (read(cansocket, &frame, sizeof(struct can_frame)) < 0)
			{
				ERROR("%m: read can");
				return -1;
			}
		//	fastprefix(frame.data[0], frame.data[1]);
			VERBOSE("can: id %x len %d %x %x %x %x %x %x %x %x\n", frame.can_id, frame.can_dlc, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
			buf[0] = 0;
			buf[1] = frame.can_id>>24;
			buf[2] = frame.can_id>>16;
			buf[3] = frame.can_id>>8;
			buf[4] = frame.can_id;
			buf[5] = frame.can_dlc;
			memcpy(buf+6, frame.data, frame.can_dlc);
			if (sendto(udpsocket, buf, frame.can_dlc+6, 0, (struct sockaddr*)saddr, sizeof(struct sockaddr_in)) < 0)
			{
				ERROR("%m: write udp");
				return -1;
			}
		}
		if(FD_ISSET(udpsocket, &readfds))
		{
			if (recvfrom(udpsocket, buf, sizeof(buf), 0, NULL, NULL) < 0)
			{
				ERROR("%m: read udp");
				return -1;
			}
			if (buf[0] == 0)
			{
				frame.can_id = buf[1]<<24|buf[2]<<16|buf[3]<<8|buf[4];
				frame.can_dlc = buf[5];
				VERBOSE("udp: id %x len %d %x %x %x %x %x %x %x %x\n", frame.can_id, frame.can_dlc, frame.data[0], frame.data[1], frame.data[2], frame.data[3], frame.data[4], frame.data[5], frame.data[6], frame.data[7]);
				memcpy(frame.data, buf+6, frame.can_dlc);
				if (write(cansocket, &frame, sizeof(struct can_frame)) < 0)
				{
					ERROR("%m: write can");
					return -1;
				}
			}
			else
			{
				VERBOSE("wrong version %d\n", buf[0]);
			}
		}
	}
}

int open_udp(struct sockaddr_in *saddr, char* address, int port)
{
	int s;
	int true = 1;
	int false = 0;
	struct sockaddr_in raddr;
	struct ip_mreqn group;

	VERBOSE("socket\n");
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		ERROR("%m: socket");
		return -1;
	}
	VERBOSE("setsockopt SO_REUSEADDR\n");
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true)) < 0)
	{
		ERROR("%m: SO_REUSEADDR");
		return -1;
	}

	group.imr_multiaddr.s_addr = inet_addr(address);
	group.imr_address.s_addr = htonl(INADDR_ANY);
	group.imr_ifindex = 0;
	VERBOSE("setsockopt IP_ADD_MEMBERSHIP\n");
	if(setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP ,&group, sizeof(group)) < 0)
	{
		ERROR("%m: IP_ADD_MEMBERSHIP");
		return -1;
	}
	VERBOSE("setsockopt IP_MULTICAST_LOOP\n");
	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP ,&false, sizeof(false)) < 0)
	{
		ERROR("%m: IP_MULTICAST_LOOP");
		return -1;
	}

	memset(&raddr, 0, sizeof(raddr));
	raddr.sin_family = AF_INET;
	raddr.sin_addr.s_addr = htonl(INADDR_ANY);
	raddr.sin_port = htons(port);
	if (bind(s, (struct sockaddr *)&raddr, sizeof(struct sockaddr)) < 0)
	{
		ERROR("%m: bind");
		return -1;
	}
	return s;
}

int get_address(struct sockaddr_in *saddr, char* address, int port)
{
	struct hostent *h; 

	h=gethostbyname(address);
	if(h == NULL)
	{
		ERROR("%m: gethostbyname");
		return -1;
	}
	VERBOSE("gethostbyname %s\n", address);
	memset(saddr, '\0', sizeof(struct sockaddr));
	saddr->sin_family = AF_INET;
	saddr->sin_port = htons(port); 
	memcpy((char *) &(saddr->sin_addr.s_addr), h->h_addr_list[0], h->h_length);
	return 1;
}