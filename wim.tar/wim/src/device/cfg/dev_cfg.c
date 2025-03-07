#define _GNU_SOURCE
#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Arete Linux SDK */

#include <pthread.h>
#include <sched.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "main.h"
#include "project.h"
#include "app_debug.h"

#include "dev_cfg.h"
#include "license.h"

char dev_cfg_net_cfg_def_dyanmic_file_content[] =
"[Match]\n\
Name=eth0\n\
\n\
[Network]\n\
DHCP=ipv4\n\
DHCPServer=no\n\
LinkLocalAddressing=yes\n\
\n\
[DHCP]\n\
RouteMetric=10";
int dev_cfg_net_cfg_create_def_dynamic_file(void)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	fp = fopen(DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE, "w+");

	if(fp == NULL) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error opening file %s\n", DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE_NAME);
		return -1;
     }

	ret = fwrite(dev_cfg_net_cfg_def_dyanmic_file_content, sizeof(dev_cfg_net_cfg_def_dyanmic_file_content) -1 , 1, fp);
	if(ret != 1) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error writing file %s : %zu\n", DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}
char dev_cfg_net_cfg_def_static_file_content[] =
"[Match]\n\
Name=eth0\n\
\n\
[Network]\n\
Address=192.168.0.15/24\n\
Gateway=192.168.0.1\n\
DNS=1.2.3.4\n\
LinkLocalAddressing=yes";
int dev_cfg_net_cfg_create_def_static_file(void)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;

	fp = fopen(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, "w+");

	if(fp == NULL) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error opening file %s\n", DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME);
		return -1;
     }

	ret = fwrite(dev_cfg_net_cfg_def_static_file_content, sizeof(dev_cfg_net_cfg_def_static_file_content) -1 , 1, fp);
	if(ret != 1) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error writing file %s : %zu\n", DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME, ret);
		return -1;
	}

	fclose(fp);

	return 0;
}

int dev_cfg_net_cfg_create_new_static_file(dev_cfg_net_cfg_t* new_cfg)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

    FILE *fp;
    size_t ret;
    char new_cfg_content[1024];
    char new_cfg_cidr[64];

    if(new_cfg == NULL)
        return -1;

    fp = fopen(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, "w+");

    if(fp == NULL) {
        APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error opening file %s\n", DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME);
        return -1;
    }

    ip_and_mask_str_to_cidr(new_cfg->ip_addr, new_cfg->ip_addr_subnet_mask, new_cfg_cidr);

    snprintf(new_cfg_content, sizeof(new_cfg_content),
"[Match]\n\
Name=eth0\n\
\n\
[Network]\n\
Address=%s\n\
Gateway=%s\n\
DNS=%s\n\
LinkLocalAddressing=yes", new_cfg_cidr, new_cfg->ip_addr_gateway, new_cfg->ip_addr_dns_server);

    ret = fwrite(new_cfg_content, strlen(new_cfg_content) , 1, fp);
    if(ret != 1) {
        APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error writing file %s : %zu\n", DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME, ret);
        return -1;
    }

    fclose(fp);
    return 0;
}

int dev_cfg_net_cfg_read_static_file_info(dev_cfg_net_cfg_info_t* curr_cfg)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	FILE *fp;
	size_t ret;
	size_t i = 0;
	char c;
	char cfg_content[1024];
	int mask_len;

	if(curr_cfg == NULL)
		return -1;

	fp = fopen(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, "r");

	if(fp == NULL) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Error opening file %s\n", DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME);
		return -1;
    }

	while((c=fgetc(fp))!=EOF) {
		//printf("%c\n", c);
		cfg_content[i++] = c;
	}
	cfg_content[i++] =  '\0';
	//printf("%s", cfg_content);

	sscanf(cfg_content,
"[Match]\n\
Name=eth0\n\
\n\
[Network]\n\
Address=%s/%d\n\
Gateway=%s\n\
DNS=%s\n\
LinkLocalAddressing=yes", curr_cfg->ip_addr, &mask_len, curr_cfg->ip_addr_gateway, curr_cfg->ip_addr_dns_server);

	//printf("->%s:%d:%s:%s \n", curr_cfg->ip_addr, &mask_len, curr_cfg->ip_addr_gateway, curr_cfg->ip_addr_dns_server );


	fclose(fp);

	return 0;
}
int cidr_to_ip_and_mask(const char* cidr, uint32_t* ip, uint32_t* mask)
{
    uint8_t a, b, c, d, bits;
    if (sscanf(cidr, "%hhu.%hhu.%hhu.%hhu/%hhu", &a, &b, &c, &d, &bits) < 5) {
        return -1; /* didn't convert enough of CIDR */
    }
    if (bits > 32) {
        return -1; /* Invalid bit count */
    }
    *ip =
        (a << 24UL) |
        (b << 16UL) |
        (c << 8UL) |
        (d);
    *mask = (0xFFFFFFFFUL << (32 - bits)) & 0xFFFFFFFFUL;

	return 0;
}

int cidr_to_ip_and_mask_str(const char* cidr, char* ip_str, char* mask_str)
{
	uint32_t ip;
	uint32_t mask;
    uint8_t a, b, c, d, bits;
	struct in_addr internal;
	int result;


    if (sscanf(cidr, "%hhu.%hhu.%hhu.%hhu/%hhu", &a, &b, &c, &d, &bits) < 5) {
        return -1; /* didn't convert enough of CIDR */
    }
    if (bits > 32) {
        return -1; /* Invalid bit count */
    }
    ip =
        (a << 24UL) |
        (b << 16UL) |
        (c << 8UL) |
        (d);
    mask = (0xFFFFFFFFUL << (32 - bits)) & 0xFFFFFFFFUL;


	internal.s_addr = ntohl(ip);
	result = inet_ntop (AF_INET, &internal, ip_str, 16);
	internal.s_addr = ntohl(mask);
	result = inet_ntop (AF_INET, &internal, mask_str, 16);

	//printf("%s:%s\n", ip_str, mask_str);

	return 0;

}

int ip_and_mask_str_to_cidr(const char* ip_str, const char* mask_str, const char* cidr)
{
	uint32_t ip;
	uint32_t mask;
    uint8_t a, b, c, d, bits;
	uint32_t n;
	int result;

	inet_pton(AF_INET, mask_str, &n);

    int i = 0;
    while (n > 0) {
            n = n >> 1;
            i++;
    }

	//printf("---->%s:%s\n", ip_str, mask_str);
	//printf("---->%s:%x\n", ip_str, n);
	//printf("---->%s/%u\n", ip_str, i);
	snprintf(cidr, 18 , "%s/%u", ip_str, i);


	return 0;

}

int dev_cfg_get_net_cfg_info_print(dev_cfg_net_cfg_info_t* net_cfg_info)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(net_cfg_info == NULL)
		return -1;

	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "---DEV_NET_CFG_INFO---\n");
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "MAC  :%s\n", net_cfg_info->ip_eth0_mac);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "MODE :%s\n",  (net_cfg_info->ip_addr_mode)? "DHCP" : "STATIC");
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "IP   :%s\n", net_cfg_info->ip_addr);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "MASK :%s\n", net_cfg_info->ip_addr_subnet_mask);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "GATE :%s\n", net_cfg_info->ip_addr_gateway);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "DNS  :%s\n", net_cfg_info->ip_addr_dns_server);

	return 0;

}

int dev_cfg_get_net_cfg_print(dev_cfg_net_cfg_t* net_cfg)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(net_cfg == NULL)
		return -1;

	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "---DEV_NET_CFG---\n");
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "MODE :%s\n", (net_cfg->ip_addr_mode)? "DHCP" : "STATIC");
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "IP   :%s\n", net_cfg->ip_addr);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "MASK :%s\n", net_cfg->ip_addr_subnet_mask);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "GATE :%s\n", net_cfg->ip_addr_gateway);
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "DNS  :%s\n", net_cfg->ip_addr_dns_server);

	return 0;
}


volatile dev_cfg_net_cfg_info_t  dev_cfg_net_cfg_info;
int dev_cfg_get_net_cfg_static_file_info(dev_cfg_net_cfg_info_t* net_cfg_info)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
#define BUF_SIZE 1024
    char	buf[BUF_SIZE];
	FILE*   stream;
	char*   lineptr = NULL;
	size_t  n = 0;
	ssize_t nread;
	char*	ch;
	size_t element_len;
	int		ret =0;;

     if(net_cfg_info == NULL)
         return -1;

     if(access(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, F_OK )) {
         return -2;
     }
     stream = fopen(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, "r");
     if(stream != NULL) {
         /*[Match]*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("1nr:%zd:%s\n", nread, lineptr);
         /*Name=eth0*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("2nr:%zd:%s\n", nread, lineptr);
         /**/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("3nr:%zd:%s\n", nread, lineptr);
         /*[Network]*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("4nr:%zd:%s\n", nread, lineptr);
		 /*Address*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("5nr:%zd:%s\n", nread, lineptr);
         if(strstr(lineptr, "Address=") == NULL) {
             ret = -1;
			 strncpy(net_cfg_info->ip_addr, "192.168.0.15", sizeof(net_cfg_info->ip_addr_subnet_mask));
			 strncpy(net_cfg_info->ip_addr_subnet_mask, "255.255.255.0" , sizeof(net_cfg_info->ip_addr_subnet_mask));
			 strncpy(net_cfg_info->ip_addr_gateway, "192.168.0.1", sizeof(net_cfg_info->ip_addr_gateway));
			 strncpy(net_cfg_info->ip_addr_dns_server, "1.2.3.4" , sizeof(net_cfg_info->ip_addr_dns_server));
             goto ISSUE;
         }

		 if(cidr_to_ip_and_mask_str(lineptr + 8,  net_cfg_info->ip_addr, net_cfg_info->ip_addr_subnet_mask)) {
			 ret = -1;
			 strncpy(net_cfg_info->ip_addr, "192.168.0.15", sizeof(net_cfg_info->ip_addr_subnet_mask));
			 strncpy(net_cfg_info->ip_addr_subnet_mask, "255.255.255.0" , sizeof(net_cfg_info->ip_addr_subnet_mask));
			 strncpy(net_cfg_info->ip_addr_gateway, "192.168.0.1", sizeof(net_cfg_info->ip_addr_gateway));
			 strncpy(net_cfg_info->ip_addr_dns_server, "1.2.3.4" , sizeof(net_cfg_info->ip_addr_dns_server));
             goto ISSUE;

		 };
         //printf("I:%s\n", net_cfg_info->ip_addr);
         //printf("M:%s\n", net_cfg_info->ip_addr_subnet_mask);

		 /*Gateway*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("6nr:%zd:%s\n", nread, lineptr);
         if(strstr(lineptr, "Gateway=") == NULL) {
			 ret = -1;
			 strncpy(net_cfg_info->ip_addr_gateway, "192.168.0.1", sizeof(net_cfg_info->ip_addr_gateway));
			 strncpy(net_cfg_info->ip_addr_dns_server, "1.2.3.4" , sizeof(net_cfg_info->ip_addr_dns_server));
             goto ISSUE;
         }
		 strncpy(net_cfg_info->ip_addr_gateway, lineptr + 8, sizeof(net_cfg_info->ip_addr_gateway));
         //printf("G:%s\n", net_cfg_info->ip_addr_gateway);
		 /*DNS*/
         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("7nr:%zd:%s\n", nread, lineptr);
         if(strstr(lineptr, "DNS=") == NULL) {
             ret = -1;
			 strncpy(net_cfg_info->ip_addr_dns_server, "1.2.3.4" , sizeof(net_cfg_info->ip_addr_dns_server));
             goto ISSUE;
         }
		 strncpy(net_cfg_info->ip_addr_dns_server, lineptr + 4, sizeof(net_cfg_info->ip_addr_dns_server));
         //printf("D:%s\n", net_cfg_info->ip_addr_dns_server);

         nread = getline(&lineptr, &n, stream);
		 lineptr[nread-1] = '\0';
         //printf("8nr:%zd:%s\n", nread, lineptr);

         free(lineptr);
         fclose(stream);
         return ret;
     }else {
 ISSUE:
         free(lineptr);
         fclose(stream);
		//APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s does not exist\n", file_name);

         return ret;
     }
 EXIT:
     //return ret;
     return 0;
}

int dev_cfg_check_exists(char* file, char* file_name)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	int rval;
	//printf("%s:%s\n", file, file_name);

	/* Check file existence. */
	rval = access(file, F_OK);
	if(rval == 0) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s exists\n", file);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s does not exist\n", file_name);
		}else if(errno == EACCES) {
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s is not accessible\n", file_name);
		}
		return -1;
	}
	return 0;
}

int dev_cfg_get_net_cfg_info(dev_cfg_net_cfg_info_t* info)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	strncpy(dev_cfg_net_cfg_info.ip_eth0_mac, license_device_info.module_eth0_mac, sizeof(dev_cfg_net_cfg_info.ip_eth0_mac));

	if(dev_cfg_check_exists(DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE, DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE_NAME)) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s dosenot exists\n",  DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE_NAME);
		if(dev_cfg_check_exists(DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE, DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME)) {
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s dosenot exists\n",  DEV_CFG_NETWORK_CFG_STATIC_CFG_FILE_NAME);
			/*Emergency No network Config make Dynamic File */
			dev_cfg_net_cfg_create_def_dynamic_file();
			info->ip_addr_mode = DEV_CFG_NET_CFG_FILE_DYNAMIC;
			dev_cfg_net_cfg_create_def_static_file();
			return -1;
		}else {
			info->ip_addr_mode = DEV_CFG_NET_CFG_FILE_STATIC;
		}
	}else {
		info->ip_addr_mode = DEV_CFG_NET_CFG_FILE_DYNAMIC;
	}

	if(dev_cfg_get_net_cfg_static_file_info(&dev_cfg_net_cfg_info)) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Fail!\n");
	}else {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Succ!\n");
		dev_cfg_get_net_cfg_info_print(&dev_cfg_net_cfg_info);
	}

	return 0;
}

int dev_cfg_set_net_cfg(dev_cfg_net_cfg_t* net_cfg)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(net_cfg == NULL)
		return -1;

	dev_cfg_get_net_cfg_print(net_cfg);
	/*check Static or dynamic */
	if(dev_cfg_net_cfg_info.ip_addr_mode == DEV_CFG_NET_CFG_FILE_STATIC) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "->EXIST_STATIC\n");
		if(net_cfg->ip_addr_mode == DEV_CFG_NET_CFG_FILE_STATIC) {
			/*Edit New Static File */
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "-->NEW Static File\n");
			if(dev_cfg_net_cfg_create_new_static_file(net_cfg)) {
				return -1;
			}
			/* Remove Dynamic FIle */
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "--->Remove Dynamic File\n");
			remove(DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE);

			system("systemctl restart systemd-networkd");
			dev_cfg_net_cfg_info.ip_addr_mode = DEV_CFG_NET_CFG_FILE_STATIC;
			if(dev_cfg_get_net_cfg_static_file_info(&dev_cfg_net_cfg_info)) {
				APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Fail!\n");
			}else {
				APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Succ!\n");
				dev_cfg_get_net_cfg_info_print(&dev_cfg_net_cfg_info);
			}
		}else if(net_cfg->ip_addr_mode == DEV_CFG_NET_CFG_FILE_DYNAMIC) {
			/*Create Dynamic file */
			if(dev_cfg_net_cfg_create_def_dynamic_file()) {
				return -1;
			}
			system("systemctl restart systemd-networkd");
			dev_cfg_net_cfg_info.ip_addr_mode = DEV_CFG_NET_CFG_FILE_DYNAMIC;
		}else {
			return -1;
		}
	}else if(dev_cfg_net_cfg_info.ip_addr_mode == DEV_CFG_NET_CFG_FILE_DYNAMIC) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "->EXIST_DHCP_MODE\n");
		if(net_cfg->ip_addr_mode == DEV_CFG_NET_CFG_FILE_STATIC) {
			/*Edit New Static File */
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "-->NEW Static File\n");
			if(dev_cfg_net_cfg_create_new_static_file(net_cfg)) {
				return -1;
			}
			/* Remove Dynamic FIle */
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "--->Remove Dynamic File\n");
			remove(DEV_CFG_NETWORK_CFG_DYNAMIC_CFG_FILE);
			system("systemctl restart systemd-networkd");
			dev_cfg_net_cfg_info.ip_addr_mode = DEV_CFG_NET_CFG_FILE_STATIC;
			if(dev_cfg_get_net_cfg_static_file_info(&dev_cfg_net_cfg_info)) {
				APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Fail!\n");
			}else {
				APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, " dev_cfg_get_net_cfg_static_file_info Succ!\n");
				dev_cfg_get_net_cfg_info_print(&dev_cfg_net_cfg_info);
			}
		}else if(net_cfg->ip_addr_mode == DEV_CFG_NET_CFG_FILE_DYNAMIC) {
			/*Ignore */
			APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "-->IGNORE\n");
			dev_cfg_net_cfg_info.ip_addr_mode = DEV_CFG_NET_CFG_FILE_DYNAMIC;
		}else {
			return -1;
		}
	}else {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "->ERROR_MODE\n");
		return -1;
	}

	return 0;
}


int dev_cfg_init(void)
{
	APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(dev_cfg_get_net_cfg_info(&dev_cfg_net_cfg_info)) {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "No CFG Dyn Created\n");
	}else {
		APP_DEBUGF(DEV_CFG_DEBUG|APP_DBG_TRACE, "Cfg %s Exists\n", (dev_cfg_net_cfg_info.ip_addr_mode)? "DHCP" : "STATIC");
	}

	return 0;
}
