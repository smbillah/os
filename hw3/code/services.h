#ifndef _SERVICES_H_
#define _SERVICES_H_


#define INVALID 0
#define CHECKSUM 1
#define COMPRESSION 2
#define DECOMPRESSION 4
#define ENCRYPTION 8
#define DECRYPTION 16
#define NUMBER_OF_JOBS 32
#define STATUS_OF_JOB 64
#define LOG_OF_JOBS 128
#define DEFAULT 1

#define DEBUG_MODE 1


#define JOB_WAITING 0
#define JOB_QUEUED 1
#define JOB_RUNNING 2
#define JOB_FINISHED 4


struct input {	
    uid_t user_id;
	pid_t process_id;
    int job_id;
    int operation;
    char *out_file;
    char *in_file;
    char *scheme;	
};


#endif //_SERVICES_H_
