#define _GNU_SOURCE
#include <config.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <limits.h>
#include <pwd.h>
#include <nss.h>
#include <sys/mount.h>

/* Arete Linux SDK */
#include "alsdk.h"
#include "alsdk_ringbuf.h"

/* WIM Application */
#include "project.h"
#include "main.h"
#include "libwim.h"
#include "system.h"
#include "app_debug.h"
#include "cfg_active.h"
#include "log.h"
#include "log_sd.h"
#include "server.h"
#include "license.h"

int log_sd_get_current_year(void)
{
     time_t tictoc;
     struct tm* local_tm;

     tictoc = time(NULL);
	 localtime_r(&tictoc, &local_tm);
	 return((local_tm->tm_year + 1900));
}
int log_sd_get_current_year_str(char* year)
{
	return 0;
}
int log_sd_get_current_month(month)
{
     time_t tictoc;
     struct tm* local_tm;

     tictoc = time(NULL);
	 localtime_r(&tictoc, &local_tm);
	 return((local_tm->tm_mon + 1));
	return 0;
}
int log_sd_get_current_month_str(char* month)
{
	return 0;
}

int log_sd_get_current_date(month)
{
     time_t tictoc;
     struct tm* local_tm;

     tictoc = time(NULL);
	 localtime_r(&tictoc, &local_tm);
	 return((local_tm->tm_year + 1900));
	return 0;
}
int log_sd_get_current_day_str(char* month)
{
	return 0;
}


int log_check_year_log_dir(void)
{
	mode_t process_mask = umask(0);
	int result_code = mkdir("/usr/local/logs", S_IRWXU | S_IRWXG | S_IRWXO);
	umask(process_mask);
	return 0;
}

int log_check_month_log_dir(void)
{
	return 0;

}

int log_check_curr_log_file(void)
{
	return 0;

}

int log_schedule_eod_timer(void)
{
	return 0;

}
#if 0
        int minute = 50;
        int second = 59;

        /* 2. Have the program print the time, as given by the three variables. */
        System.out.println("The time is "+hour+":"+minute+":"+second);

        /* 3. Make the program calculate and print the number of seconds since midnight. */
        int SEC_PER_MINUTE = 60;
        int SEC_PER_HOUR = 60 * SEC_PER_MINUTE;
        int SEC_PER_DAY = 24 * SEC_PER_HOUR;
        int secondsElapsedSinceMidnight = hour*SEC_PER_HOUR + minute*SEC_PER_MINUTE + second;
        System.out.println("Seconds since midnight: " + secondsElapsedSinceMidnight);

        /* 4. Make the program calculate and print the number of seconds remaining in the
           day. */
        System.out.println(SEC_PER_DAY - secondsElapsedSinceMidnight + " seconds remain.");
#endif


#if 0
/**
 * Get the user identifier from the username.
 *
 */
uid_t privuid(const char* username)
{

    if(username) {
        /* Lookup the user id in /etc/passwd */
		if(getpwnam_r("pvsarath", &pwd, buf, sizeof buf, &pwdp)) {
            printf("[%s] unable to get user id for %s: %s",
                logstr, username, strerror(s));
			return -1;
		}else {
           return (pwd.pw_uid);
		}
    }else {
        return -1;
    }
}
#endif
int log_sd_mount(void)
{
	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);
	struct passwd pwd;
	struct passwd *pwdp;
	char buf[1024];
	uid_t uid;
	gid_t gid;

	sync();

	if(umount2(LOG_SD_ROOT, MNT_FORCE)) {
	   perror("unmount failed");
	}

	system("/usr/sbin/fsck.vfat  -aw /dev/mmcblk1p1");

	if(getpwnam_r(LOG_SD_USER_NAME, &pwd, buf, sizeof(buf), &pwdp)) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "unable to get user id for %s", LOG_SD_USER_NAME);
		return -1;
	}

	uid = pwd.pw_uid;
	gid = pwd.pw_gid;
	//snprintf(buf, 1023, "uid=%zu,gid=%zu,dmask=007,fmask=117", uid, gid);
	snprintf(buf, 1023, "uid=%zu,gid=%zu,dmask=0002,fmask=0113", uid, gid);
	//snprintf(buf, 1023, "uid=%zu,gid=%zu,dmask=0000,fmask=0000", uid, gid);
	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s:%s:%s\n", LOG_SD_DEV, LOG_SD_ROOT, buf);

	   if(mount(LOG_SD_DEV, LOG_SD_ROOT, "vfat", MS_DIRSYNC | MS_SYNCHRONOUS, buf)) {
	  //if(mount(LOG_SD_DEV, LOG_SD_ROOT, "vfat", MS_DIRSYNC, buf)) {
		   perror("mount failed");
		//	return -1;
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "Mount created at %s...\n", LOG_SD_ROOT);
			return 0;
		}
	//system("/usr/sbin/fsck.msdos -aw /dev/mmcblk1p1");

   return -1;
}

int log_sd_unmount(log_sd_t* s)
{
	if(s == NULL) {
		return -1;
	}
	syncfs(s->fd);
	close(s->fd);
    return 0;

	return (umount2(LOG_SD_ROOT, MNT_FORCE));
}

int log_sd_init(log_sd_t* s, char const* root, char* wbuf, size_t wbuf_size)
{

	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	if(s == NULL || root == NULL) {
		errno = EINVAL;
		return -1;
	}

	if(wbuf == NULL || wbuf_size <= 0) {
		errno = EINVAL;
		return -1;
	}

	if(alsdk_ringbuf_init(&s->wrb, wbuf, wbuf_size -1));
	//assert(alsdk_ringbuf_buffer_size(&s->wrb) == wbuf_size);
	alsdk_pthread_mutex_init(&s->wrb_mutex, NULL);
	alsdk_pthread_mutex_init(&s->mutex, NULL);
	s->null_fd =  open("/dev/null", O_WRONLY);
	if(project_info.hw.sdcard.use)
		s->use = 1;
	else
		s->use = 0;

    s->present = 0;

    s->root = root;
    s->fd = 0;
    s->fd_year = 0;
    s->fd_month = 0;
    s->fd_day = 0;
	s->buff_index = 0;

	return 0;
}


typedef struct __attribute__((packed)) log_sd_status_s {
	uint8_t present;
    uint64_t size;
    uint64_t free_size;
    float free_perc;
}log_sd_status_t;
static log_sd_status_t log_sd_status ={0, 0, 0, 0.0f};


int log_sd_1_status_get_server(srv_sd_status_t* svr_sd_status)
{
	struct statvfs vfsbuf;
	struct stat st = {0};

	double cap_mb;
	double free_mb;
	double free_mb_per;

    if(!access("/sys/block/mmcblk1/size", F_OK) == 0 ) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "/sys/block/mmcblk1/size dosenot exists\n");
		log_sd_status.present = 0;
		log_sd_status.size = 0;
		log_sd_status.free_size = 0;
		log_sd_status.free_perc = 0;
	} else {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "/sys/block/mmcblk1/size exists\n");

		/*check sd card mount dir ans stats*/
		if(statvfs(LOG_SD_ROOT, &vfsbuf) == -1) {
			//alsdk_err_quit("statvfs :");
			log_sd_status.present = 0;
			log_sd_status.size = 0;
			log_sd_status.free_size = 0;
			log_sd_status.free_perc = 0;
		}else {
			//printf("each block is %d bytes big\n", vfsbuf.f_frsize);
			//printf("there are %d blocks available out of a total of %d\n", vfsbuf.f_bavail, vfsbuf.f_blocks);
			double cap_b;
			double free_b;
			double free_per;

			double cap_mb;
			double free_mb;

			cap_b = ((double)vfsbuf.f_blocks * vfsbuf.f_frsize);
			free_b = ((double)vfsbuf.f_bavail * vfsbuf.f_frsize);

			cap_mb = ((double)vfsbuf.f_blocks * vfsbuf.f_frsize)/(1024*1024);
			free_mb = ((double)vfsbuf.f_bavail * vfsbuf.f_frsize)/(1024*1024);
			free_per = (free_mb/cap_mb) * 100;
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "cap  %.0f\n", cap_mb);
			//printf("SDC OK : %.0f MB (A), %.2f\% (F)\n", free_mb, free_per);
			log_sd_status.present = 1;
			log_sd_status.size = (int64_t) cap_b;
			log_sd_status.free_size = free_b;
			log_sd_status.free_perc = free_per;
		}
	}

	svr_sd_status->present = log_sd_status.present;
	svr_sd_status->size = log_sd_status.size;
	svr_sd_status->free_size = log_sd_status.free_size;
	svr_sd_status->free_perc = log_sd_status.free_perc;

	return 0;
}


int log_sd_check_exists(char* file_name)
{
	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", __func__);

	if(file_name == NULL)
		return -1;

	int rval;
	/* Check file existence. */
	rval = access(file_name, F_OK);
	if(rval == 0) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_STATE, "%s exists\n", file_name);
	}else {
		if(errno == ENOENT) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_STATE, "%s does not exist\n", file_name);
		}else if(errno == EACCES) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_STATE, "%s is not accessible\n", file_name);
		}
		return -1;
	}
	return 0;
}

int log_sd_write_file_header(log_sd_t* s)
{
	return 0;
}

int log_sd_open(log_sd_t* s)
{
	struct statvfs vfsbuf;
	struct stat st = {0};
	char fs_uri[PATH_MAX+1];
	int file_exists;

    char year[8];
    char month[8];
    char log_file[32];

	time_t raw_time;
	long ms;
    struct tm now_tm;

	int fd;

	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	if(s == NULL || s->root == NULL) {
		errno = EINVAL;
		return -1;
	}
	if(s->use) {
	/* Handle No SD Card present Case*/

		if(access("/sys/block/mmcblk1/size", F_OK ) == 0 ) {
			s->present = 1;
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "/sys/block/mmcblk1/size exists \n");
			if(log_sd_mount()) {
				s->mounted = 1;
				APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "log_sd_mount e\n");
			}else {
				s->mounted = 1;
				APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "log_sd_mount s\n");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "/sys/block/mmcblk1/size dosenot exists \n");
			s->present = 0;
			//return -1;
		}

		/*check sd card mount dir ans stats*/
		if(statvfs(s->root, &vfsbuf) == -1) {
			alsdk_err_quit("statvfs :");
		}else {
			//printf("each block is %d bytes big\n", vfsbuf.f_frsize);
			//printf("there are %d blocks available out of a total of %d\n", vfsbuf.f_bavail, vfsbuf.f_blocks);
			double cap_mb;
			double free_mb;
			double free_mb_per;
			cap_mb = ((double)vfsbuf.f_blocks * vfsbuf.f_frsize)/(1024*1024);
			free_mb = ((double)vfsbuf.f_bavail * vfsbuf.f_frsize)/(1024*1024);
			free_mb_per = (free_mb/cap_mb) * 100;
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "cap  %.0f\n", cap_mb);
			//printf("SDC OK : %.0f MB (A), %.2f\% (F)\n", free_mb, free_mb_per);
		}

		/* Retrieve the current time */
		raw_time = time(NULL);
		localtime_r(&raw_time, &now_tm);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "log_sd_open:%04d:%02d:%02d\n", now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday);

		/*check root dir*/
		//printf("%s\n", s->root);

		strftime(year, sizeof(year), "%Y", &now_tm);
		//printf("%s\n", year);
		strftime(month, sizeof(month), "%b", &now_tm);
		//printf("%s\n", month);
		strftime(log_file, sizeof(log_file), "Log_%d_%b_%Y", &now_tm);
		//printf("%s\n", log_file);

		/*check year dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s", s->root, year);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				//system_do_chown(fs_uri,"sdcard", "sdcard");
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("chown Y");
				}
			}else{
				perror("mkdir Y");
				alsdk_err_quit("mkdir  year :");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
		}

		/*check month dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s", s->root, year, month);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("chown M");
				}
			}else{
				alsdk_err_quit("mkdir month :");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
		}

		/*check log_file */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s/%s.txt", s->root, year, month, log_file);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		file_exists = !log_sd_check_exists(fs_uri);

		fd = open(fs_uri,  O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
		if(fd == -1) {
			perror("open file open :");
			alsdk_err_quit("open file open:");
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s opened\n", fs_uri);
		}

		//system_do_chown(fs_uri,"sdcard", "sdcard");
		if(system_do_chown(fs_uri,"arete", "arete")) {
			perror("chown D");
		}

		alsdk_pthread_mutex_lock(&s->mutex);
		s->fd = fd;
		s->fd_year = now_tm.tm_year + 1900;
		s->fd_month = now_tm.tm_mon;
		s->fd_day = now_tm.tm_mday;
		if(!file_exists) {
			char tmp_buf[256];
			size_t len;
			len = snprintf(tmp_buf, 256, "[%u-%s][%s-%s]\n\n", license_device_info.board_ser_no, license_device_info.module_ser_no,
					cfg_active_machine.location_id,  cfg_active_machine.lane_id);
			write(s->fd, tmp_buf, len);
		}
		alsdk_pthread_mutex_unlock(&s->mutex);
	}else {
		s->present = 0;
		s->mounted = 0;
	}
		return(0);
}


int	log_sd_handle_eod(log_sd_t* s)
{
	struct statvfs vfsbuf;
	struct stat st = {0};
	char fs_uri[PATH_MAX+1];
	int file_exists;

    char year[8];
    char month[8];
    char log_file[32];

    time_t raw_time;
    struct tm now_tm;

	int fd;

	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

	if(s == NULL || s->root == NULL) {
		errno = EINVAL;
		return -1;
	}

	/*check sd card mount dir ans stats*/
	if(statvfs(s->root, &vfsbuf) == -1) {
		alsdk_err_quit("statvfs :");
	}else {
		//printf("each block is %d bytes big\n", vfsbuf.f_frsize);
		//printf("there are %d blocks available out of a total of %d\n", vfsbuf.f_bavail, vfsbuf.f_blocks);
		double cap_mb;
		double free_mb;
		double free_mb_per;
		cap_mb = ((double)vfsbuf.f_blocks * vfsbuf.f_frsize)/(1024*1024);
		free_mb = ((double)vfsbuf.f_bavail * vfsbuf.f_frsize)/(1024*1024);
		free_mb_per = (free_mb/cap_mb) * 100;
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "cap  %.0f\n", cap_mb);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "SDC OK : %.0f MB (A), %.2f\% (F)\n", free_mb, free_mb_per);
	}

    // Using close system Call
	fsync(s->fd);
    if(close(s->fd) < 0) {
		alsdk_err_quit("close sd :");
    }

	/* Retrieve the current time */
    raw_time = time(NULL);
    localtime_r(&raw_time, &now_tm);
	APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "log_sd_handle_eod:%04d:%02d:%02d\n", now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday);

	/*check root dir*/
    APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", s->root);

    strftime(year, sizeof(year), "%Y", &now_tm);
    //printf("%s\n", year);
	strftime(month, sizeof(month), "%b", &now_tm);
    //printf("%s\n", month);
	strftime(log_file, sizeof(log_file), "Log_%d_%b_%Y", &now_tm);
    //printf("%s\n", log_file);


	/*check year dir */
	snprintf(fs_uri, PATH_MAX+1, "%s/%s", s->root, year);
    APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

	if(stat(fs_uri, &st) == -1) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
		if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
			//system_do_chown(fs_uri,"sdcard", "sdcard");
			if(system_do_chown(fs_uri,"arete", "arete")) {
				perror("chown Y");
			}
        }else{
			perror("mkdir Y");
			alsdk_err_quit("mkdir  year :");
        }
	}else {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
	}


	/*check month dir */
	snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s", s->root, year, month);
    APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

	if(stat(fs_uri, &st) == -1) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
		if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
			//system_do_chown(fs_uri,"sdcard", "sdcard");
			if(system_do_chown(fs_uri,"arete", "arete")) {
				perror("chown M");
			}
        }else{
			perror("mkdir M");
			alsdk_err_quit("mkdir month :");
        }
	}else {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
	}

	/*check log_file */
	snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s/%s.txt", s->root, year, month, log_file);
    APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

	file_exists = !log_sd_check_exists(fs_uri);

	fd = open(fs_uri, O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
	if(fd == -1) {
		perror("open file eod :");
		alsdk_err_quit("open file eod :");
	}else {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s opened\n", fs_uri);
	}

	//system_do_chown(fs_uri,"sdcard", "sdcard");
	if(system_do_chown(fs_uri,"arete", "arete")) {
		perror("chown D");
	}

    alsdk_pthread_mutex_lock(&s->mutex);
	s->fd = fd;
	s->fd_year = now_tm.tm_year + 1900;
	s->fd_month = now_tm.tm_mon;
	s->fd_day = now_tm.tm_mday;

	if(!file_exists) {
		char tmp_buf[256];
		size_t len;
		len = snprintf(tmp_buf, 256, "[%u-%s][%s-%s]\n\n", license_device_info.board_ser_no, license_device_info.module_ser_no,
				cfg_active_machine.location_id,  cfg_active_machine.lane_id);
		write(s->fd, tmp_buf, len);
    }

    alsdk_pthread_mutex_unlock(&s->mutex);

	return 0;
}

int log_sd_close(log_sd_t* s)
{

	return 0;
}
int log_sd_destroy(log_sd_t* s)
{

	return 0;
}
 // buf needs to store 30 characters
#define LOG_SD_TS_BUF_LEN (16)
int log_sd_send(log_sd_t* s, char* data, size_t len)
{
	APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);
	int             ret;
	char            buf[LOG_SD_TS_BUF_LEN];
	char            ch;
	size_t          len_skip;
	size_t          i;
	struct timespec ts;
	struct tm       t;
	long            ms; // Milliseconds

	if(s == NULL) {
		errno = EINVAL;
		return -1;
	}

	clock_gettime((clockid_t)CLOCK_REALTIME_COARSE, &ts);

    ms = (long)round((double) (ts.tv_nsec/1.0e6)); // Convert nanoseconds to milliseconds
	if(ms > 999) {
		ts.tv_sec++;
		ms = 0;
	}
	//tzset();
	if(localtime_r(&(ts.tv_sec), &t) == NULL) {
		errno = EINVAL;
		return -1;
	}
	if(strftime(buf, LOG_SD_TS_BUF_LEN, " %H_%M_%S", &t) == 0) {
		errno = EINVAL;
		return -1;
	}
    ret = snprintf(buf + 9, (LOG_SD_TS_BUF_LEN - 9), ".%03ld\n", ms);
	if(ret >= (LOG_SD_TS_BUF_LEN - 8)) {
		errno = EINVAL;
		return -1;
	}
	//printf("%s\n", data);
	//printf("%s\n", buf);
	//
	len_skip = 0;
	for(i = len - 1 ; i >= 0 ; i--) {
		ch = data[i];
		if(ch == '\0' || ch == '\n' || ch == '\r') {
			len_skip++;
			if(i == 0) break;
		}else {
			break;
		}
	}

    alsdk_pthread_mutex_lock(&s->mutex);
	if(s->use && s->present){
	    ret = write(s->fd, data, len - len_skip);
		ret += write(s->fd, buf, 14);
	}else{
	    ret = write(s->null_fd, data, len - len_skip);
		ret += write(s->null_fd, buf, 14);
	}
    alsdk_pthread_mutex_unlock(&s->mutex);

    return ret;
}


int log_sd_send_direct(log_sd_t* s, char* data , size_t len)
{
	APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

    int             ret;
	char            buf[LOG_SD_TS_BUF_LEN];
	char            ch;
	size_t          len_skip = 0;
	size_t          tlen;
	size_t           i;
	struct timespec ts;
	struct tm       t;
	long            ms; // Milliseconds
	int file_exists;

    if(s == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }

	clock_gettime((clockid_t)CLOCK_REALTIME_COARSE, &ts);
    ms = (long)round((double) (ts.tv_nsec/1.0e6)); // Convert nanoseconds to milliseconds
	if(ms > 999) {
		ts.tv_sec++;
		ms = 0;
	}
	//tzset();
	localtime_r(&(ts.tv_sec), &t);
	snprintf(buf, LOG_SD_TS_BUF_LEN, " %02d_%02d_%02d.%03ld\n", t.tm_hour, t.tm_min, t.tm_sec, ms);
	//printf("data %d : %d: %s\n", len, strlen(data), data);
	//printf("buf  %d:%s\n", strlen(buf), buf);
	//printf("CURR:%02d:%02d:%02d:%02d.%03ld\n", t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, ms);

	len_skip = 0;
	for(i = len - 1 ; i >= 0 ; i--) {
		ch = data[i];
		if(ch == '\0' || ch == '\n' || ch == '\r') {
			len_skip++;
			if(i == 0) break;
		}else {
			break;
		}
	}
	alsdk_pthread_mutex_lock(&s->mutex);
	if(((t.tm_year + 1900)!= s->fd_year) || (t.tm_mon != s->fd_month) || (t.tm_mday != s->fd_day)) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "C:%04d:%02d:%02d\n", t.tm_year + 1900, t.tm_mon, t.tm_mday);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "P:%04d:%02d:%02d\n", s->fd_year, s->fd_month, s->fd_day);
		//log_sd_handle_eod(s);
		/* -- */
		struct stat st = {0};
		int fd;
		char fs_uri[PATH_MAX+1];

		char year[8];
		char month[8];
		char log_file[32];

		/* --->write previous */
		if(s->buff_index) {
			ret = write(s->fd, &s->buff, s->buff_index);
			s->buff_index = 0;
		}

		// Using close system Call
		fsync(s->fd);
		if(close(s->fd) < 0) {
			alsdk_err_quit("close sd :");
		}

		/*check root dir*/
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", s->root);

		strftime(year, sizeof(year), "%Y", &t);
		strftime(month, sizeof(month), "%b", &t);
		strftime(log_file, sizeof(log_file), "Log_%d_%b_%Y", &t);

		/*check year dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s", s->root, year);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);
		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("SD:chown Y");
					alsdk_err_quit("chown  year:");
				}
			}else{
				perror("SD:mkdir Y");
				alsdk_err_quit("mkdir  year:");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
		}

		/*check month dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s", s->root, year, month);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("SD:chown M");
					alsdk_err_quit("chown month:");
				}
			}else{
				perror("SD:mkdir M");
				alsdk_err_quit("mkdir month:");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE,"%s exists \n", fs_uri);
		}

		/*check log_file */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s/%s.txt", s->root, year, month, log_file);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		file_exists = !log_sd_check_exists(fs_uri);

		fd = open(fs_uri,  O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
		if(fd == -1) {
			perror("open file direct eod :");
			alsdk_err_quit("open file direct eod :");
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s opened\n", fs_uri);
		}
		if(system_do_chown(fs_uri,"arete", "arete")) {
			perror("SD:chown D");
			alsdk_err_quit("chown day:");
		}

		s->fd = fd;
		s->fd_year = t.tm_year + 1900;
		s->fd_month = t.tm_mon;
		s->fd_day = t.tm_mday;

		if(!file_exists) {
			char tmp_buf[256];
			size_t len;
			len = snprintf(tmp_buf, 256, "[%u-%s][%s-%s]\n\n", license_device_info.board_ser_no, license_device_info.module_ser_no,
					cfg_active_machine.location_id,  cfg_active_machine.lane_id);
			write(s->fd, tmp_buf, len);
		}
		/*------*/
	}

    //ret = write(s->fd, data, (size_t) (len - len_skip));
    //ret += write(s->fd, buf, 14);

	/* --->write now */
	tlen = (size_t) (len - len_skip);
	if((tlen + 14) > (LOG_SD_BUFF_SIZE - s->buff_index)) {
		if(cfg_active_wim_core.sd_log) {
			if(s->use && s->present) {
				ret = write(s->fd, &s->buff, s->buff_index);
			}else {
				ret = write(s->null_fd, &s->buff, s->buff_index);
			}
		}
		s->buff_index = 0;
	}
	memcpy(&s->buff[s->buff_index], data, tlen);
	s->buff_index += tlen;
	memcpy(&s->buff[s->buff_index], buf, 14);
	s->buff_index += 14;

    alsdk_pthread_mutex_unlock(&s->mutex);

    return ret;
}

int log_sd_send_buff(log_sd_t* s, char* data , size_t len)
{
	APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

    int             ret;
    int             retry;
	char            buf[LOG_SD_TS_BUF_LEN];
	char            ch;
	size_t          len_skip = 0;
	size_t           i;
	struct timespec ts;
	struct tm       t;
	long            ms; // Milliseconds
	int file_exists;

    if(s == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }


	clock_gettime((clockid_t)CLOCK_REALTIME_COARSE, &ts);
    ms = (long)round((double) (ts.tv_nsec/1.0e6)); // Convert nanoseconds to milliseconds
	if(ms > 999) {
		ts.tv_sec++;
		ms = 0;
	}
	//tzset();
	localtime_r(&(ts.tv_sec), &t);
	snprintf(buf, LOG_SD_TS_BUF_LEN, " %02d_%02d_%02d.%03ld\n", t.tm_hour, t.tm_min, t.tm_sec, ms);
	//printf("data %d : %d: %s\n", len, strlen(data), data);
	//printf("buf  %d:%s\n", strlen(buf), buf);
	//printf("CURR:%02d:%02d:%02d:%02d.%03ld\n", t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, ms);

	len_skip = 0;
	for(i = len - 1 ; i >= 0 ; i--) {
		ch = data[i];
		if(ch == '\0' || ch == '\n' || ch == '\r') {
			len_skip++;
			if(i == 0) break;
		}else {
			break;
		}
	}
	alsdk_pthread_mutex_lock(&s->mutex);
	if(((t.tm_year + 1900)!= s->fd_year) || (t.tm_mon != s->fd_month) || (t.tm_mday != s->fd_day)) {
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "C:%04d:%02d:%02d\n", t.tm_year + 1900, t.tm_mon, t.tm_mday);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "P:%04d:%02d:%02d\n", s->fd_year, s->fd_month, s->fd_day);
		//log_sd_handle_eod(s);
		/* -- */
		struct stat st = {0};
		int fd;
		char fs_uri[PATH_MAX+1];

		char year[8];
		char month[8];
		char log_file[32];

		// Using close system Call
		fsync(s->fd);
		if(close(s->fd) < 0) {
			perror("close s->fd");
			alsdk_err_quit("close sd :");
		}

		/*check root dir*/
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", s->root);

		strftime(year, sizeof(year), "%Y", &t);
		//printf("%s\n", year);
		strftime(month, sizeof(month), "%b", &t);
		//printf("%s\n", month);
		strftime(log_file, sizeof(log_file), "Log_%d_%b_%Y", &t);
		//printf("%s\n", log_file);


		/*check year dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s", s->root, year);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);
		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				//system_do_chown(fs_uri,"sdcard", "sdcard");
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("SD:chown Y");
				}
			}else{
				perror("SD:mkdir Y");
				alsdk_err_quit("mkdir  year :");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s exists \n", fs_uri);
		}

		/*check month dir */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s", s->root, year, month);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		if(stat(fs_uri, &st) == -1) {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s no exists \n", fs_uri);
			if(mkdir(fs_uri, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1) {
				//system_do_chown(fs_uri,"sdcard", "sdcard");
				if(system_do_chown(fs_uri,"arete", "arete")) {
					perror("SD:chown M");
				}
			}else{
				perror("CD:stat2");
				alsdk_err_quit("mkdir month :");
			}
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE,"%s exists \n", fs_uri);
		}


		/*check log_file */
		snprintf(fs_uri, PATH_MAX+1, "%s/%s/%s/%s.txt", s->root, year, month, log_file);
		APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s\n", fs_uri);

		file_exists = !log_sd_check_exists(fs_uri);
#if 0
		retry = 0;
		do {
			fd = open(fs_uri,  O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
			if(fd == -1) {
				perror("open file buff eod :");
				//alsdk_err_quit("open file buff eod :");
				printf("retry: [%d]\n", retry);
				retry++;
			}else {
				APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s opened\n", fs_uri);
				break;
			}
		}while(retry < 5);

		if(retry < 5) {
		}else {

		}
#endif
		fd = open(fs_uri,  O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IROTH);
		if(fd == -1) {
			perror("open file buff eod :");
			alsdk_err_quit("open file buff eod :");
		}else {
			APP_DEBUGF(LOG_SD_DEBUG|APP_DBG_TRACE, "%s opened\n", fs_uri);
		}

		//system_do_chown(fs_uri,"sdcard", "sdcard");
		if(system_do_chown(fs_uri,"arete", "arete")) {
			perror("SD:chown D");
		}

		s->fd = fd;
		s->fd_year = t.tm_year + 1900;
		s->fd_month = t.tm_mon;
		s->fd_day = t.tm_mday;

		if(!file_exists) {
			char tmp_buf[256];
			size_t len;
			len = snprintf(tmp_buf, 256, "[%u-%s][%s-%s]\n\n", license_device_info.board_ser_no, license_device_info.module_ser_no,
					cfg_active_machine.location_id,  cfg_active_machine.lane_id);
			write(s->fd, tmp_buf, len);
		}
		/*------*/
	}

    alsdk_pthread_mutex_lock(&s->wrb_mutex);
    ret = alsdk_ringbuf_memcpy_into(&s->wrb, data, (size_t) (len - len_skip));
    ret += alsdk_ringbuf_memcpy_into(&s->wrb, buf, 14);
    alsdk_pthread_mutex_unlock(&s->wrb_mutex);
    alsdk_pthread_mutex_unlock(&s->mutex);

    return ret;
}
int log_sd_send_buff_eol(log_sd_t* s, char* data , size_t len)
{
	//APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

    int             ret;
	char            buf[LOG_SD_TS_BUF_LEN];
	char            ch;
	size_t          len_skip = 0;
	size_t           i;
	struct timespec ts;
	struct tm       t;
	long            ms; // Milliseconds

    if(s == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }

	clock_gettime((clockid_t)CLOCK_REALTIME_COARSE, &ts);
    ms = (long)round((double) (ts.tv_nsec/1.0e6)); // Convert nanoseconds to milliseconds
	if(ms > 999) {
		ts.tv_sec++;
		ms = 0;
	}
	//tzset();
	if(localtime_r(&(ts.tv_sec), &t) == NULL) {
		errno = EINVAL;
		return -1;
	}
	if(strftime(buf, LOG_SD_TS_BUF_LEN, " %H_%M_%S", &t) == 0) {
		errno = EINVAL;
		return -1;
	}
    ret = snprintf(buf + 9, (LOG_SD_TS_BUF_LEN - 9), ".%03ld\n", ms);
	if(ret >= (LOG_SD_RB_WBUFF_SIZE - 8)) {
		errno = EINVAL;
		return -1;
	}
	//printf("data %d : %d: %s\n", len, strlen(data), data);
	//printf("buf  %d:%s\n", strlen(buf), buf);

	len_skip = 0;
	for(i = len - 1 ; i >= 0 ; i--) {
		ch = data[i];
		if(ch == '\0' || ch == '\n' || ch == '\r') {
			len_skip++;
			if(i == 0) break;
		}else {
			break;
		}
	}
    alsdk_pthread_mutex_lock(&s->mutex);
    ret = alsdk_ringbuf_memcpy_into(&s->wrb, data, (size_t) (len - len_skip));
    ret += alsdk_ringbuf_memcpy_into(&s->wrb, buf, 14);
    alsdk_pthread_mutex_unlock(&s->mutex);

    return ret;
 }

 int log_sd_get_line_from_buff(log_sd_t* s, char* data, size_t len)
 {
     APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);

     int i;
     char ch;

     if(s == NULL || data == NULL)
         return -EINVAL;

     alsdk_pthread_mutex_lock(&s->wrb_mutex);
     i = 0;
	 //printf("-1-\n");
     while((i< len) && (alsdk_ringbuf_memcpy_from(&ch , &s->wrb, 1))) {
        data[i] = ch;
        if(ch == '\0')  {
            break;
		}
		i++;
     }
     alsdk_pthread_mutex_unlock(&s->wrb_mutex);
 //  printf("data : %s\n", data);

     return 0;
}
int log_sd_flush_line_from_buff(log_sd_t* s)
{

	APP_DEBUGF(LOG_SD_CMD_DEBUG|APP_DBG_TRACE, "%s\n",  __func__);
	int ret;
	char data[LOG_SD_RB_WBUFF_SIZE];
	size_t i;
	char ch;
	size_t tlen;

	if(s == NULL || data == NULL)
		return -EINVAL;

    alsdk_pthread_mutex_lock(&s->wrb_mutex);
    i = 0;
    while((i< LOG_SD_RB_WBUFF_SIZE) && (alsdk_ringbuf_memcpy_from(&ch , &s->wrb, 1))) {
        data[i] = ch;
        if(ch == '\0') {
            break;
		}
		i++;
    }
    alsdk_pthread_mutex_unlock(&s->wrb_mutex);

	//printf("data : %s\n", data);
    alsdk_pthread_mutex_lock(&s->mutex);
	tlen = i;
	if((tlen) > (LOG_SD_BUFF_SIZE - s->buff_index)) {
		if(cfg_active_wim_core.sd_log) {
			if(s->use && s->present) {
				ret = write(s->fd, &s->buff, s->buff_index);
			}else {
				ret = write(s->null_fd, &s->buff, s->buff_index);
			}
		}
		s->buff_index = 0;
	}
	ret = memcpy(&s->buff[s->buff_index], data, tlen);
	s->buff_index += tlen;
	    //write(s->fd, data, i);
    alsdk_pthread_mutex_unlock(&s->mutex);

    return 0;
}

int log_sd_flush(log_sd_t* s)
{
	return 0;
}
