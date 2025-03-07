#define _GNU_SOURCE
#include <config.h>
#include <stdio.h>
#include <errno.h>

/* Arete Linux SDK */
#include <alsdk.h>

/* WIM Application */
#include "main.h"
#include "device.h"
#include "watchdog.h"
#include "app_debug.h"
#include "dev_cfg.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>


#include <ifaddrs.h>
#include <linux/rtnetlink.h>
volatile device_err_t dev_err;

int device_hello(void)
{
  printf("%s", "Device World!\n");
  return 0;
}

int device_get_if_ipaddress(char* ifc, char* ipaddress)
{
     int fd;
     struct ifreq ifr;

     // replace with your interface name
     // or ask user to input


     fd = socket(AF_INET, SOCK_DGRAM, 0);

     //Type of address to retrieve - IPv4 IP address
     ifr.ifr_addr.sa_family = AF_INET;

     //Copy the interface name in the ifreq structure
     strncpy(ifr.ifr_name , ifc , IFNAMSIZ-1);

     ioctl(fd, SIOCGIFADDR, &ifr);

	 snprintf(ipaddress, 15, "%s", inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr));

     close(fd);

}

/* 169.254.0.0 */
#define AUTOIP_NET_MASK (0X0000FEA9)

void show_address_info(struct ifaddrs* ifa)
{
	struct sockaddr_in *s4;
    /* ipv6 addresses have to fit in this buffer */
    char buf[64];

	if(AF_INET == ifa->ifa_addr->sa_family) {
      s4 = (struct sockaddr_in *)(ifa->ifa_addr);
		if((s4->sin_addr.s_addr & 0X0000FFFF) == AUTOIP_NET_MASK) {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "is link local\n");
		}else {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "not link local\n");
		}
      if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))) {
        APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s: inet_ntop failed!\n", ifa->ifa_name);
      }else {
        APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IPv4 addr %s: %s\n", ifa->ifa_name, buf);
      }
    }
}


int device_print_ip(void)
{
	struct ifaddrs *myaddrs, *ifa;
    int status;

    status = getifaddrs(&myaddrs);
    if(status != 0) {
		APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "getifaddrs failed!");
		freeifaddrs(myaddrs);
		return -1;
	}

    for(ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {

		if(strcmp(ifa->ifa_name, "eth0")) {
			continue;
		}

		if(NULL == ifa->ifa_addr) {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "NULL\n");
			continue;
		}
		if((ifa->ifa_flags & IFF_UP) == 0) {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "!IFF_UP\n");
			continue;
		}else {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IFF_UP\n");
		}
		if((ifa->ifa_flags & IFF_RUNNING) == 0) {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "!IFF_RUNNIG\n");
			//continue;
		}else {
			APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IFF_RUNNING\n");
		}

		show_address_info(ifa);
	}
    freeifaddrs(myaddrs);
    return 0;
}

int device_get_link_status(char* ifc_name)
{
	return 0;
}

int device_get_link_local_ip(char* ifc_name, char* ip_address)
{
	struct ifaddrs *myaddrs, *ifa;
    int status;

	struct sockaddr_in *s4;

	if(ifc_name == NULL || ip_address == NULL) {
		return -1;
	}

	snprintf(ip_address, 15, "%s", "0.0.0.0");
    status = getifaddrs(&myaddrs);
    if(status != 0) {
		APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "getifaddrs failed!");
		freeifaddrs(myaddrs);
		return -1;
	}

    for(ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {

		if(strcmp(ifa->ifa_name, ifc_name)) {
			continue;
		}

		if(NULL == ifa->ifa_addr) {
			continue;
		}

		if((ifa->ifa_flags & IFF_UP) == 0) {
			continue;
		}
		if((ifa->ifa_flags & IFF_RUNNING) == 0) {
		//	continue;
		}

        if(AF_INET == ifa->ifa_addr->sa_family) {
			s4 = (struct sockaddr_in *)(ifa->ifa_addr);

			if((s4->sin_addr.s_addr & 0X0000FFFF) == AUTOIP_NET_MASK) {
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), ip_address, INET_ADDRSTRLEN)) {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s : inet_ntop failed! : %s \n", ifa->ifa_name, strerror(errno));
				}else {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "LL %s : %s\n", ifa->ifa_name, ip_address);
				}
			}else {
#if 0
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))) {
					//printf("%s: inet_ntop failed!\n", ifa->ifa_name);
				}else {
					//printf("IPv4 addr %s: %s\n", ifa->ifa_name, buf);
				}
#endif
			}


		}

	}
    freeifaddrs(myaddrs);
    return 0;
}
int device_get_ip(char* ifc_name, char* ip_address)
{
	struct ifaddrs *myaddrs, *ifa;
    int status;

	struct sockaddr_in *s4;

	if(ifc_name == NULL || ip_address == NULL) {
		return -1;
	}

	snprintf(ip_address, 15, "%s", "0.0.0.0");
    status = getifaddrs(&myaddrs);
    if(status != 0) {
		APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "getifaddrs failed!");
		freeifaddrs(myaddrs);
		return -1;
	}

    for(ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {

		if(strcmp(ifa->ifa_name, ifc_name)) {
			continue;
		}

		if(NULL == ifa->ifa_addr) {
			continue;
		}
		if((ifa->ifa_flags & IFF_UP) == 0) {
			continue;
		}
		if((ifa->ifa_flags & IFF_RUNNING) == 0) {
		//	continue;
		}

        if(AF_INET == ifa->ifa_addr->sa_family) {
			s4 = (struct sockaddr_in *)(ifa->ifa_addr);

			if((s4->sin_addr.s_addr & 0X0000FFFF) == AUTOIP_NET_MASK) {
#if 0
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))) {
					//printf("%s: inet_ntop failed!\n", ifa->ifa_name);
				}else {
					//printf("IPv4 addr %s: %s\n", ifa->ifa_name, buf);
				}
#endif

			}else {
				//printf("not link local\n");
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), ip_address, INET_ADDRSTRLEN)) {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s : inet_ntop failed! : %s \n", ifa->ifa_name, strerror(errno));
				}else {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IP %s : %s\n", ifa->ifa_name, ip_address);
				}
			}
		}

	}
    freeifaddrs(myaddrs);
    return 0;
}
int device_get_ip_and_mask(char* ifc_name, char* ip_address , char* ip_mask)
{
	struct ifaddrs *myaddrs, *ifa;
    int status;

	struct sockaddr_in *s4;
	struct sockaddr_in *s5;

	if(ifc_name == NULL || ip_address == NULL || ip_mask == NULL) {
		return -1;
	}

	snprintf(ip_address, 15, "%s", "0.0.0.0");
	snprintf(ip_mask, 15, "%s", "0.0.0.0");
    status = getifaddrs(&myaddrs);
    if(status != 0) {
		APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "getifaddrs failed!");
		freeifaddrs(myaddrs);
		return -1;
	}

    for(ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {

		if(strcmp(ifa->ifa_name, ifc_name)) {
			continue;
		}

		if(NULL == ifa->ifa_addr) {
			continue;
		}
		if((ifa->ifa_flags & IFF_UP) == 0) {
			continue;
		}
		if((ifa->ifa_flags & IFF_RUNNING) == 0) {
		//	continue;
		}

        if(AF_INET == ifa->ifa_addr->sa_family) {
			s4 = (struct sockaddr_in *)(ifa->ifa_addr);

			if((s4->sin_addr.s_addr & 0X0000FFFF) == AUTOIP_NET_MASK) {
#if 0
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), buf, sizeof(buf))) {
					//printf("%s: inet_ntop failed!\n", ifa->ifa_name);
				}else {
					//printf("IPv4 addr %s: %s\n", ifa->ifa_name, buf);
				}
#endif

			}else {
				//printf("not link local\n");
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s4->sin_addr), ip_address, INET_ADDRSTRLEN)) {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s : inet_ntop failed! : %s \n", ifa->ifa_name, strerror(errno));
				}else {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IP %s : %s\n", ifa->ifa_name, ip_address);
				}
				s5 = (struct sockaddr_in *)(ifa->ifa_netmask);
				if(NULL == inet_ntop(ifa->ifa_addr->sa_family, (void *)&(s5->sin_addr), ip_mask, INET_ADDRSTRLEN)) {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s : inet_ntop failed! : %s \n", ifa->ifa_name, strerror(errno));
				}else {
					APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "IP %s : %s\n", ifa->ifa_name, ip_mask);
				}
			}
		}

	}
    freeifaddrs(myaddrs);
    return 0;
}


#define BUFSIZE 8192

struct route_info {
	struct in_addr dst_addr;
	struct in_addr src_addr;
	struct in_addr gw_addr;
	char if_name[IF_NAMESIZE];
};


ssize_t read_netlink_socket (int sock_fd, char *buffer, size_t buffer_sz, uint32_t seq, uint32_t pid)
{
	struct nlmsghdr *nl_hdr;
	ssize_t read_len = 0, msg_len = 0;

     do {
         if ((size_t) msg_len > buffer_sz) {
             perror ("No size in buffer");
             return -1;
         }

         /* Recieve response from the kernel */
         read_len = recv (sock_fd, buffer, buffer_sz - msg_len, 0);
         if (read_len < 0) {
             perror ("SOCK READ: ");
             return -1;
         }

         nl_hdr = (struct nlmsghdr *)buffer;

         /* Check if the header is valid */
         if (!NLMSG_OK (nl_hdr, read_len)
             || (nl_hdr->nlmsg_type == NLMSG_ERROR)) {
             perror ("Error in recieved packet");
             return -1;
         }

         /* Check if the its the last message */
         if (nl_hdr->nlmsg_type == NLMSG_DONE)
             break;
         else {
         /* Else move the pointer to buffer appropriately */
             buffer += read_len;
             msg_len += read_len;
         }

         /* Check if its a multi part message */
         if (!(nl_hdr->nlmsg_flags & NLM_F_MULTI))
         /* return if its not */
             break;

     } while ((nl_hdr->nlmsg_seq != seq)
              || (nl_hdr->nlmsg_pid != pid));

     return msg_len;
 }

 /* parse the route info returned */
 int parse_routes (struct nlmsghdr *nl_hdr, struct route_info *rt_info)
 {
     struct rtmsg *rt_msg;
     struct rtattr *rt_attr;
     int rt_len;

     rt_msg = (struct rtmsg *)NLMSG_DATA (nl_hdr);

     /* If the route is not for AF_INET or
      * does not belong to main routing table then return. */
     if ((rt_msg->rtm_family != AF_INET)
         || (rt_msg->rtm_table != RT_TABLE_MAIN))
         return -1;

     /* get the rtattr field */
     rt_attr = (struct rtattr *)RTM_RTA (rt_msg);
     rt_len = RTM_PAYLOAD (nl_hdr);

     for (; RTA_OK (rt_attr,rt_len); rt_attr = RTA_NEXT (rt_attr,rt_len)) {
         switch (rt_attr->rta_type) {
         case RTA_OIF:
             if_indextoname (*(int *)RTA_DATA (rt_attr), rt_info->if_name);
             break;
         case RTA_GATEWAY:
             memcpy (&rt_info->gw_addr, RTA_DATA(rt_attr),
                     sizeof (rt_info->gw_addr));
             break;
         case RTA_PREFSRC:
             memcpy (&rt_info->src_addr, RTA_DATA(rt_attr),
                     sizeof (rt_info->src_addr));
             break;
         case RTA_DST:
             memcpy (&rt_info->dst_addr, RTA_DATA(rt_attr),
                     sizeof (rt_info->dst_addr));
             break;
         }
     }

     return 0;
 }
 int
 get_gatewayip (char *gatewayip, socklen_t size)
 {
     int found_gatewayip = 0;

     struct nlmsghdr *nl_msg;
     struct route_info route_info;
     char msg_buffer[BUFSIZE]; // pretty large buffer
     int sock, len, msg_seq = 0;

     /* Create Socket */
     sock = socket (PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
     if (sock < 0) {
         perror("Socket Creation: ");
         return -1;
     }

     /* Initialize the buffer */
     memset (msg_buffer, 0, sizeof (msg_buffer));

     /* point the header and the msg structure pointers into the buffer */
     nl_msg = (struct nlmsghdr *)msg_buffer;

     /* Fill in the nlmsg header*/
     nl_msg->nlmsg_len = NLMSG_LENGTH (sizeof (struct rtmsg)); // Length of message.
     nl_msg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
     nl_msg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
     nl_msg->nlmsg_seq = msg_seq++; // Sequence of the message packet.
     nl_msg->nlmsg_pid = getpid(); // PID of process sending the request.

     /* Send the request */
     if (send (sock, nl_msg, nl_msg->nlmsg_len, 0) < 0) {
         fprintf(stderr, "Write To Socket Failed...\n");
         return -1;
     }

     /* Read the response */
     len = read_netlink_socket (sock, msg_buffer, sizeof (msg_buffer),
                                msg_seq, getpid ());
     if (len < 0) {
         fprintf(stderr, "Read From Socket Failed...\n");
         return -1;
     }
     close (sock);


     /* Parse and print the response */
     for (; NLMSG_OK (nl_msg, len); nl_msg = NLMSG_NEXT (nl_msg, len)) {
         memset (&route_info, 0, sizeof(route_info));
         if (parse_routes (nl_msg, &route_info) < 0)
             continue;  // don't check route_info if it has not been set up

         // Check if default gateway
         if (strstr ((char *)inet_ntoa (route_info.dst_addr), "0.0.0.0")) {
             // copy it over
             inet_ntop (AF_INET, &route_info.gw_addr, gatewayip, size);
             found_gatewayip = 1;
             printf ("ifname: %s\n", route_info.if_name);
             break;
         }
     }

     return found_gatewayip;
 }



int  device_get_eth0_mac(char* eth0_mac)
{
#define MAC_SIZE 17
	int fd;
	char buf[MAC_SIZE+1];
    size_t nread = 0;
	ssize_t res = 0;

	if(eth0_mac == NULL)
		return -1;
	if(access("/sys/class/net/eth0/address", F_OK)) {
		return -2;
	}
	if(access("/sys/class/net/eth0/address", R_OK)) {
		return -3;
	}

	fd = open("/sys/class/net/eth0/address", O_RDONLY);
	if(fd < 0) {
		perror("/sys/class/net/eth0/address");
		return -4;
	}

	while(nread < MAC_SIZE) {
        res = read(fd, buf + nread, MAC_SIZE - nread);
        if(res == 0) break;
        if(res == -1) return -5;
        nread += res;
	}
	buf[MAC_SIZE] = '\0';
	//printf("mac:%s\n", buf);
	snprintf(eth0_mac, MAC_SIZE + 1 ,"%s", buf);

	return 0;
}
volatile uint32_t m_srno;
int device_get_serial_no(char* ser_no)
{
#define SER_NO_SIZE 8
	int fd;
	char buf[SER_NO_SIZE+1];
    size_t nread = 0;
	ssize_t res = 0;

	if(ser_no == 0)
		return -1;

	if(access("/proc/device-tree/serial-number", F_OK)) {
		return -2;
	}
	if(access("/proc/device-tree/serial-number", R_OK)) {
		return -3;
	}

	fd = open("/proc/device-tree/serial-number", O_RDONLY);
	if(fd < 0) {
		perror("/proc/device-tree/serial-number");
		return -4;
	}
	while(nread < SER_NO_SIZE) {
        res = read(fd, buf + nread, SER_NO_SIZE - nread);
        if(res == 0) break;
        if(res == -1) return -5;
        nread += res;
	}
	buf[SER_NO_SIZE] = '\0';
	//printf("ser:%s\n", buf);
	snprintf(ser_no, SER_NO_SIZE + 1,"%s", buf);
	m_srno = strtoul(buf, NULL, 10);
	//printf("%s\n", buf);
	//printf("%lu\n", m_srno);

	return 0;
}

/* NETWORK PARAMETERS*/
#if 0



int device_set_network_params(dev_network_params_t* dev_net_params)
{
	return 0;
}
//device_get_link_local_ip("eth0", &guio_link_local_ip_addres);
//device_get_ip("eth0", &guio_ip_addres);


int device_get_network_params(dev_network_params_get_t* dev_net_params_get)
{
#if 0
	int fd;
	char buf[SER_NO_SIZE+1];
    size_t nread = 0;
	ssize_t res = 0;


	if(dev_net_params == NULL)
		return -1;
	if(access(DEV_NETWORK_FILE, F_OK)) {
		return -2;
	}
	if(access(DEV_NETWORK_FILE, R_OK)) {
		return -3;
	}

	fd = open(DEV_NETWORK_FILE, O_RDONLY);
	if(fd < 0) {
		perror("DEV_NETWORK_FILE");
		return -4;
	}
#endif

	return 0;
}

/* network_config lisencing  */
license_device_info_t license_device_info;
static license_t			 license_gen;
int license_get_lisence_info(void)
{
	char lic_seed[65] = "arete";
	char lic_string[1024];
    alsdk_md5_context ctx;

	if(device_get_serial_no(license_device_info.serial_no)) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "get serial fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s\n", license_device_info.serial_no);
	}
	if(device_get_eth0_mac(license_device_info.eth0_mac)) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "get mac fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s\n", license_device_info.eth0_mac);
	}

	snprintf(lic_string, sizeof(lic_string), "%s%s%s", license_device_info.serial_no, license_device_info.eth0_mac, lic_seed);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "lic_string:%s\n", lic_string);

    alsdk_md5_init(&ctx);
    alsdk_md5_update(&ctx, (uint8_t*) lic_string, strlen(lic_string));
    alsdk_md5_finalize(&ctx);
#if 0
	alsdk_print_hash((char*)ctx.digest);
#endif

    memcpy(license_gen.digest, ctx.digest, 16);
#if 0
	alsdk_print_hash((char*)ctx.digest);
	alsdk_print_hash((char*)license_gen.digest);
#endif
	license_gen.crc = alsdk_crc32((uint8_t*)&license_gen, sizeof(license_t) - sizeof(license_gen.crc), 0xFFFFFFFF);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "licence_gen.crc         : %u\n", license_gen.crc);

	return 0;
}


volatile license_t lic_license;
int license_print(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "---------------------------------\n");
	//APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "license->digest      : %s\n", license->digest);
	//alsdk_print_hash((char*)license->digest);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "licence->crc         : %u\n", license->crc);
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "---------------------------------\n");

	return 0;
}
int license_verify(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	uint32_t crc;

	crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	if(crc != license->crc)
		return -1;

	return 0;
}
int license_validate(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	if(memcmp(license_gen.digest, license->digest, 16)){
		return -1;
	}

	return 0;
}
int license_generate(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	memset(license, 0, sizeof(license_t));

	strncpy((char*)&license->digest, LICENSE_DIGEST, sizeof(license->digest));

	license->crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	return 0;
}
int license_update(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    if(license == NULL)
		return -1;

	license->crc = alsdk_crc32((uint8_t*)license, sizeof(license_t) - sizeof(license->crc), 0xFFFFFFFF);

	return 0;
}
int license_check_exists(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;
	/* Check file existence. */
	rval = access(LICENSE_FILE, F_OK);
	if(rval == 0) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s exists\n", LICENSE_FILE);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s does not exist\n", LICENSE_FILE_NAME);
		}else if(errno == EACCES) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is not accessible\n", LICENSE_FILE_NAME);
		}
		return -1;
     }

	return 0;
}
int license_check_r(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	int rval;

	/* Check read write access. */
	rval = access(LICENSE_FILE, R_OK);
	if(rval == 0) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is r\n", LICENSE_FILE_NAME);
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "%s is not r (access denied)\n", LICENSE_FILE_NAME);
	}

	return 0;
}
int license_create(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;

	fp = fopen(LICENSE_FILE, "wrb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error creating file %s\n", LICENSE_FILE_NAME);
		return -1;
	}

	fclose(fp);

	return 0;
}
int license_open(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	return 0;
}
int license_write(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	if(license == NULL)
		return -1;

	fp = fopen(LICENSE_FILE, "wb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error opening file %s\n", CFG_MACHINE_FILE_NAME);
		return -1;
     }

	ret = fwrite(license, sizeof(license_t), 1, fp);
	if(ret != 1) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error writing file %s : %zu\n", CFG_MACHINE_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}
int network_cfg_read(license_t* license)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	fp = fopen(LICENSE_FILE, "rb");

	if(fp == NULL) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error opening file %s\n", CFG_MACHINE_FILE_NAME);
		return -1;
	}

	memset(license, 0, sizeof(license_t));

	ret = fread(license, sizeof(license_t), 1, fp);
	if(ret != 1) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_STATE, "Error reading file %s : %zu\n", CFG_MACHINE_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}
int network_cfg_init(void)
{
	APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(license_get_lisence_info()) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "license_get_lisence_info Fail\n");
		return -1;
	}else {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "license_get_lisence_info Succ\n");
	}
	/* Get Local License */
	if(license_check_exists()) {
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "No License File Found\n");
#if 1
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "Exists Genetating\n");
		license_write(&license_gen);
		return  0;
#endif
		return -2;
	}else {
		if(license_read(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License Read Failed\n");
#if 1
		APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "Read Genetating\n");
		license_write(&license_gen);
		return  0;
#endif
			return -3;
		}
		if(license_verify(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License CRC verify Failed\n");
			return  -4;
		}
		license_print(&lic_license);
		if(license_validate(&lic_license)) {
			APP_DEBUGF(LICENSE_DEBUG|APP_DBG_TRACE, "License Validation Failed\n");
			return  -5;
		}
	}
	return 0;
}
#endif


int device_init(void)
{
	APP_DEBUGF(DEVICE_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	device_watchdog_init();

	return (dev_cfg_init());
}
