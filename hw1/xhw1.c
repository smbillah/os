#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "xconcat.h"


#define __NR_xconcat	349	/* our private syscall number */

/* xhw1 supports the following command-line arguments:
 * 
 * -a: append mode (O_APPEND)
 * -c: O_CREATE
 * -t: O_TRUNC
 * -e: O_EXCL
 * -A: Atomic concat mode (extra credit only)
 * -N: return num-files instead of num-bytes written
 * -P: return percentage of data written out
 * -m ARG: set default mode to ARG (e.g., octal 755, see chmod(2) and umask(1))
 * -h: print short usage string
 */


/* displays the help functionalities */
void display_usage(){    
    printf("How to use this function... TBD\n");
}


/* displays the current values returned by getopt() */
void display_arguments(golabalArgs_t globalArgs){
    printf("outFile = %s\n", globalArgs.outfile);	
	printf("infile_count = %d\n", globalArgs.infile_count);
        
    int i=0;
    for(i=0;i<globalArgs.infile_count;i++)
        printf("\t%s\n", globalArgs.infiles[i]);
    
	printf("oflags = %d\n", globalArgs.oflags);
	printf("mode = %d\n", globalArgs.mode);
	printf("flags = %d\n", globalArgs.flags);  
}


/* the main function */
int main(int argc, char *argv[])
{
	int rc;
	
	/* Initialize globalArgs before we get to work. */
    golabalArgs_t globalArgs;
	globalArgs.outfile = NULL;	
	globalArgs.infiles = NULL;
	globalArgs.infile_count = 0;
	globalArgs.oflags = 0x0;
	globalArgs.mode = 644;
	globalArgs.flags = 0x00;
	
	/* Process the arguments with getopt(), then populate globalArgs. 
	 * ./xhw1 [flags] outfile infile1 infile2 ...
     */
     
    int opt = 0;
    const char* optString ="acteANPm:h";
	opt = getopt( argc, argv, optString);
	while( opt != -1 ) {
		switch( opt ) {
			case 'a':				
                globalArgs.oflags |= O_APPEND;
				break;            
            case 'c':
                globalArgs.oflags |= O_CREAT;				
				break;
            case 't':
				globalArgs.oflags |= O_TRUNC;
				break;
            case 'e':
				globalArgs.oflags |= O_EXCL;
				break;
            
            case 'A':
				globalArgs.flags |= 0x04;
				break;
            case 'N':
				globalArgs.flags |= 0x01;
				break;
            case 'P':
				globalArgs.flags |= 0x02;
				break;
            
			case 'm':				
				globalArgs.mode = atoi(optarg);//TODO: octal check
				break;				
			case 'h':			
				display_usage();
				break;
				
			default:				
				break;
		}		
		opt = getopt( argc, argv, optString );
	}
	
	//read the file name(s)
    globalArgs.infile_count = argc - optind -1;    
    if(globalArgs.infile_count < 1 ){
        printf("Error: input/output files are not specified\n");
        exit(-1);
    }
    
    //reading the output file's name
    globalArgs.outfile = argv[optind++];    
    
    //reading the input file's names
    globalArgs.infiles = (const char** )&argv[optind];
	
    //debug function
    display_arguments(globalArgs);
    
    
    rc = syscall(__NR_xconcat, (void*)&globalArgs, sizeof(globalArgs));
	if (rc == 0){
		printf("syscall returned %d\n", rc);
    }
	else {
		printf("syscall returned %d (errno=%d)\n", rc, errno);
    }
    
	exit(rc);
}
