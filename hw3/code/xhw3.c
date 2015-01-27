#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include<pthread.h>
#include <semaphore.h>  /* Semaphore */

#include "services.h"

#define SIG_TEST 44
#define __NR_xjob	349	/* our private syscall number */

void print_usage(void)
{
	fprintf(stderr, "command for file processing:\n");
	fprintf(stderr, "<./xhw3 [flags] outfile infile [key]>\n");
	fprintf(stderr, " where flags are:\n");
    fprintf(stderr, "-c: file checksum\n");
    fprintf(stderr, "-z: file compression\n");
    fprintf(stderr, "-x: file decompression\n");
    fprintf(stderr, "-e: file encryption with key\n");
    fprintf(stderr, "-d: file decryption\n\n");
	
	fprintf(stderr, "command for job status enquiry:\n");
	fprintf(stderr, "./xhw3 [flags] jobid\n");
	fprintf(stderr, " where flags are:\n");
	fprintf(stderr, "-N: number of job request\n");
	fprintf(stderr, "-S: status of requested jobs\n");
	fprintf(stderr, "-L: log of user processed jobs\n");
	fprintf(stderr, "-h: print short usage string\n\n");
}

/*
* Parse user input using getopt function and check its validity. For example user
* cannot give 0 input output file. User can not give wrong flags. In those case
* the usage is printed. If the validation succeed then it calls xconcat syscall.
* If the return value is error then print appropriate message otherwise print the
* number returned by the syscall.
*/
struct input *user_input(int argc, char *argv[])
{
    int option, len, i, count;
	int operation;
	struct input *data;
    char *buf = NULL;
    opterr = 0;

    while((option = getopt(argc, argv, "czxedNSLh")) != -1)
    {
        switch(option)
        {
            case 'c':                
                operation = CHECKSUM;
                break;

            case 'z':                
                operation = COMPRESSION;
                break;
                
            case 'x':                
                operation = DECOMPRESSION;
                break;
                
            case 'e':                
                operation = ENCRYPTION;
                break;
                
            case 'd':                    
                operation = DECRYPTION;
                break;
                    
            case 'N':                    
                operation = NUMBER_OF_JOBS;
                break;
                    
            case 'S':                    
                operation = STATUS_OF_JOB;
                break;
                    
            case 'L':                    
                operation = LOG_OF_JOBS;                            
                break;
                    
            case 'h':                    
                print_usage();
                exit(0);
                break;
                    
            default:       
                operation = DEFAULT;
                break;
        }
    }

	data = (struct input *) malloc(sizeof(struct input));
	if(data == NULL)
	{
		printf("memory allocation failed.");
		exit(0);
	}
	
    
	data->user_id = getuid();
	data->process_id = getpid();
	data->operation = operation;
	printf("option = %d, data->operation = %d\n", operation, data->operation);
	
    if(operation == NUMBER_OF_JOBS ||		
		operation == LOG_OF_JOBS)
	{
		data->out_file = NULL;
		data->in_file = NULL;
		return data;
	}
    
    if(operation == STATUS_OF_JOB)
	{
		data->out_file = NULL;
		data->in_file = NULL;
		
        buf = argv[optind];
		if(buf == NULL) 
		{
			free(data);
            printf("job_id is not provided\n");
			exit(0);			
		}

		len = strlen(buf);
		while(i < len && buf[i]=='0') 
            i++; //skip the starting zero's

        while(i < len){ 
            if(buf[i] >='0' && buf[i]<='9') 
                count++;
            else {
                free(data);
                printf("job_id is bad formed\n");
                exit(0);
            }
            i++;
        }                
        data->job_id = strtol(buf, NULL, 10);
		if(data->job_id < 0) 
		{
			free(data);
            printf("job_id is bad formed\n");
			exit(0);			
		}
		return data;
	}
	
	if(argc <= optind + 1){        
        printf("number of arguments is incorrect\n");
        print_usage();
		free(data);
        exit(0);
    }

    data->out_file = (char *) argv[optind];
    data->in_file = (char *) argv[optind + 1];
    if(data->out_file == NULL || data->in_file == NULL)
    {
        printf("given file is invalid\n");
        print_usage();
        free(data);
        exit(0);
    }
    
    if(operation == ENCRYPTION || operation == DECRYPTION){        
        if(argc <= optind + 2)
        {
            printf("encryption key required\n");
            print_usage();
            free(data);
            exit(0);
        }
        data->scheme = (char *) argv[optind + 2];
        if(data->scheme == NULL)
        {
            printf("given scheme is invalid\n");
            print_usage();
            free(data);
            exit(0);
        }
    }

    
    if(DEBUG_MODE == 1) printf("Output File: %s\n", data->out_file);
	if(DEBUG_MODE == 1) printf("Input File: %s\n", data->in_file);
    if(DEBUG_MODE == 1) printf("Scheme : %s\n", data->scheme);

	return data;
}

//sem_t mutex;
int finish = 0;

void receiveData(int n, siginfo_t *info, void *unused) {
	printf("received a signal %i\n", info->si_int);
    finish = 1;
    //sem_post(&mutex);
}

/*
void wait_for_signal(){
     sem_init(&mutex, 0, 1);  
     printf("waiting for signal\n");
     sem_wait(&mutex);
     return;
}
*/

int main(int argc, char *argv[])
{
	int rc;
	struct input *data;
	
    //signal implementation
    struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);
    
    //data processing
	data = user_input(argc, argv);	
    printf("Login details: %d\n", data->user_id);
	printf("\noption = %d\n", data->operation);
	printf("processid = %d\n", data->process_id);
	
    rc = syscall(__NR_xjob, data, rc);	
    
	if(rc < 0)
	{
		printf("returned error (%d) in system-call \n", errno);
	}
	else
	{
		printf("system-call returned successfully %d\n", rc);
	}
	free(data);
    
    printf("waiting for signal...\n");
    //while(!finish);
    //wait_for_signal();    
	exit(rc);
}
