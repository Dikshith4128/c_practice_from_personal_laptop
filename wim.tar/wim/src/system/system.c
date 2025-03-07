#define _GNU_SOURCE
#include <config.h>
#include <stdint.h>
#include <stdio.h>

#include <time.h>

#include <pthread.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/random.h>
#include <pwd.h>
#include <grp.h>


/* Arete Linux SDK */
#include <alsdk.h>

/* WIM Application */
#include "main.h"
#include "system.h"
#include "app_debug.h"

volatile system_err_t sys_err;

int system_hello(void)
{
  printf("%s", "System World!\n");
  return 0;
}

int system_gettime(time_t* utc_sec)
{
	struct timespec walltime;

    APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "%s\n", __func__);
	//printf("%s\n", __func__);

	if(clock_gettime(CLOCK_REALTIME, &walltime)) {
		perror("clock_gettime");
		return -1;
	}
    //printf("Current wall time:%s\n", ctime(&walltime.tv_sec));
	return 0;
}

int system_settime(time_t utc_sec)
{
	struct timespec walltime;

    APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	//walltime.tv_sec = utc_sec + 19080;
	walltime.tv_sec = utc_sec;
	walltime.tv_nsec = 0;

    APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "Setting wall time to :%s\n", ctime(&walltime.tv_sec));
	if(clock_settime(CLOCK_REALTIME, &walltime)) {
		perror("clock_settime");
		return -1;
	}
	system("/sbin/hwclock --utc --systohc");
	return 0;
}
int system_get_serial_module(char *mac_addr)
{

	return 0;
}
int system_get_serial_user(char *mac_addr)
{

	return 0;
}
int system_get_serial(char *mac_addr)
{

	return 0;
}

int system_get_ip_address(char *ip_addr)
{

	return 0;
}
int system_get_net_mask(char *net_mask)
{

	return 0;
}
int system_get_mac_address(char *mac_addr)
{

	return 0;
}

int system_install_fw(char *file_name)
{
	int res;
	int res_install;

	char buf[128];

	if(file_name == NULL) {
		return -1;
	}else if(strlen(file_name) > 31) {
		return -1;
	}

	snprintf(buf, 128, "opkg install /home/firmware/%s", file_name);
	printf("file :%s\n", buf);

	res = system("opkg remove \"wim*\"");
	APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "system_install_fw : %d \n", res);
	res_install = system(buf);
	APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "system_install_fw : %d \n", res);
	res = system("sync");
	APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "system_install_fw : %d \n", res);

	return res_install;
}

int system_do_chown(const char *file_path, const char *user_name, const char *group_name)
{
	uid_t          uid;
	gid_t          gid;
	struct passwd *pwd;
	struct group  *grp;

	pwd = getpwnam(user_name);
	if(pwd == NULL) {
		return -1;
	}

	uid = pwd->pw_uid;
	grp = getgrnam(group_name);

	if(grp == NULL) {
		return -1;
	}

	gid = grp->gr_gid;
	if(chown(file_path, uid, gid) == -1) {
		APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "chown fail\n");
		return -1;
	}
	return 0;
}
int system_rt_reboot(uint8_t reboot)
{
    APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "%s : %u\n", __func__ , reboot);

	return(system("/sbin/reboot"));
}
int system_rt_restart(uint8_t restart)
{
    APP_DEBUGF(SYSTEM_DEBUG|APP_DBG_TRACE, "%s : %u\n", __func__ , restart);
	system("sync");
	exit(EXIT_SUCCESS);
}

#if 0
float system_get_1_min_cpu_load_avg(void)
{
	struct sysinfo si;
	float f_load = 1.f / (1 << SI_LOAD_SHIFT);

	/* Obtain system statistics. */
	sysinfo(&si);

    printf("load average (1 min): %.2f (%.0f%% CPU)\n",
         si.loads[0] * f_load,
         si.loads[0] * f_load * 100/get_nprocs());

	 return (si.loads[0] * f_load * 100/get_nprocs());
}
float system_get_1_min_cpu_load_avg(void)
{

     char str[100];
     const char d[2] = " ";
     char *token;
     int i = 0;

     long int user, nice, system, idle, iowait, irq, softirq;
     float avg_cpu_perc;
     float prev_avg_cpu_perc;

     FILE *fp = fopen("/proc/stat", "r");

     i = 0;
     fgets(str, 100, fp);
     token = strtok(str, d);
     while(token != NULL) {
         token = strtok(NULL, d);
         if(token != NULL) {
             if(i == 0) {
                 user = atoi(token);
             }else if( i == 1 ) {
                 nice = atoi(token);
             }else if( i == 2 ) {
                 system = atoi(token);
             }else if(i == 3) {
                 idle = atoi(token);
             }else if(i == 4) {
                 iowait = atoi(token);
             }else if(i == 5) {
                 irq = atoi(token);
             }else if(i == 6) {
                 softirq = atoi(token);
             }
             i++;
			 if(i > 6) break;
         }
     }
     fclose(fp);
     //printf("%ld : %ld : %ld : %ld : %ld : %ld : %ld\n", user, nice, system, idle, iowait, irq, softirq);

     avg_cpu_perc = (100  -  (idle / (user + nice + system + idle + iowait + irq + softirq)) * 100);
     prev_avg_cpu_perc = avg_cpu_perc;
     printf("%f\n", avg_cpu_perc);

     return (avg_cpu_perc);
}
float system_get_1_min_cpu_load_avg(void)
{
     return (50.0f);
}
#endif
struct cpust{
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;
	unsigned long iowait;
	unsigned long irq;
	unsigned long softirq;
	unsigned long steal;
	unsigned long guest;
	unsigned long guest_nice;
};
#if 0

#!/bin/bash
while :; do
  # Get the first line with aggregate of all CPUs
  cpu_now=($(head -n1 /proc/stat))
  # Get all columns but skip the first (which is the "cpu" string)
  cpu_sum="${cpu_now[@]:1}"
  # Replace the column seperator (space) with +
  cpu_sum=$((${cpu_sum// /+}))
  # Get the delta between two reads
  cpu_delta=$((cpu_sum - cpu_last_sum))
  # Get the idle time Delta
  cpu_idle=$((cpu_now[4]- cpu_last[4]))
  # Calc time spent working
  cpu_used=$((cpu_delta - cpu_idle))
  # Calc percentage
  cpu_usage=$((100 * cpu_used / cpu_delta))

  # Keep this as last for our next read
  cpu_last=("${cpu_now[@]}")
  cpu_last_sum=$cpu_sum

  echo "CPU usage at $cpu_usage%"

  # Wait a second before the next read
  sleep 1
done
#endif
float system_get_1_min_cpu_load_avg(void)
{
	char cpun[255];
	struct cpust cpu_now;
	static struct cpust cpu_last;

	unsigned long cpu_sum;
	static unsigned long cpu_last_sum;

	unsigned long cpu_delta;
	unsigned long cpu_idle;
	unsigned long cpu_used;

	float cpu_usage;

	FILE *fp = fopen("/proc/stat","r");

	/* Get the first line with aggregate of all CPUs and Calculate Sum */
	if(fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", cpun, &(cpu_now.user), &(cpu_now.nice), &(cpu_now.system),
				&(cpu_now.idle), &(cpu_now.iowait), &(cpu_now.irq), &(cpu_now.softirq), &(cpu_now.steal), &(cpu_now.guest), &(cpu_now.guest_nice)) == EOF)
		perror("fscanf");
	fclose(fp);
	cpu_sum =  cpu_now.user + cpu_now.nice + cpu_now.system + cpu_now.idle + cpu_now.iowait + cpu_now.irq + cpu_now.softirq + cpu_now.steal + cpu_now.guest + cpu_now.guest_nice;

	/* Get the delta between two reads */
	cpu_delta = cpu_sum - cpu_last_sum;
	/* Get the idle time Delta */
	cpu_idle =  cpu_now.idle -  cpu_last.idle;
	/* Calc time spent working */
	cpu_used = cpu_delta - cpu_idle;

	/* Keep this as last for our next read */
	cpu_last =  cpu_now;
	cpu_last_sum = cpu_sum;

	/* Calc percentage */
	cpu_usage = 100.0 * ((double)cpu_used / cpu_delta);

	return (cpu_usage);
}

#if 0
float system_get_1_min_cpu_load_avg(void)
{st.user + st.nice + st.system + st.idle + st.iowait + st.irq + st.softirq + st.steal + st.guest + st.guest_nice)
     char cpun[255];
	 struct cpust st;

     FILE *fp = fopen("/proc/stat","r");
     if(fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", cpun, &(st.user), &(st.nice), &(st.system),
				 &(st.idle), &(st.iowait), &(st.irq), &(st.softirq), &(st.steal), &(st.guest), &(st.guest_nice)) == EOF)
         perror("fscanf");
     fclose(fp);
     return ((100.0 - ((double)(st.idle * 100) / ((double)(st.user + st.nice + st.system + st.idle + st.iowait + st.irq + st.softirq + st.steal + st.guest + st.guest_nice)))));
}

#endif
uint32_t system_getrandom6(void)
{
	uint64_t tmp;
	uint32_t num;

    if(getrandom(&tmp, 8, GRND_NONBLOCK) == -1 ) {
         perror("getrandom");
    }

	num = (tmp % (900000)) + 100000;

    //num = tmp % 1000000;

	return num;
}

/* firmware */
