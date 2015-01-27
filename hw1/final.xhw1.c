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
    printf("./xhw1 [-cateANP] [-m ARG] outfile infile1 infile2 ...\n");
}


/* displays the current values returned by getopt() */
void display_arguments(golabalArgs_t globalArgs){
    printf("outFile = %s\n", globalArgs.outfile);	
	printf("infile_count = %d\n", globalArgs.infile_count);
        
    int i=0;
    for(i=0;i<globalArgs.infile_count;i++)
        printf("\t%s\n", globalArgs.infiles[i]);
    
	printf("oflags = %d\n", globalArgs.oflags);
	printf("mode = %o\n", globalArgs.mode);
	printf("flags = %d\n", globalArgs.flags);  
}


/* the main function */
int main(int argc, char *argv[])
{
	int rc;
	int i=0, count=0;
	/* Initialize globalArgs before we get to work. */
    golabalArgs_t globalArgs;
	globalArgs.outfile = NULL;	
	globalArgs.infiles = NULL;
	globalArgs.infile_count = 0;
	globalArgs.oflags = 0x00;
	globalArgs.mode = 0644;
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
                i=0; count=0;
                if(!optarg) goto out;                               
                while(i < strlen(optarg) && optarg[i]=='0') i++; //skip the starting zero's
                while(i < strlen(optarg)){                                    
                    if(optarg[i] >='0' && optarg[i]<'8') count++; // count permission digits
                    else goto out; 
                    i++;
                }                
                if (count > 3) goto out; 
                globalArgs.mode = strtol(optarg, NULL, 8);
                
                break;
                out:
                    printf("xhw1: invalid/missing -m ARG\n" );                    
                    exit(-1);			
            case 'h':			
				display_usage();
				break;
				
			default:	
                printf("xhw1: invalid flags are provided\n" );                    
                exit(-1);			
				break;
		}		
		opt = getopt( argc, argv, optString );
	}
	
    //validate flags
    if(!(globalArgs.flags & 0x04)){
        if( (globalArgs.flags & 0x02) && (globalArgs.flags & 0x01)){
            printf("xhw1: incompatible options -P -N\n");
            exit(-1);
        }
    }
    
	//read the file name(s)
    count = argc - optind -1;            
    if(count < 1 ){
        printf("xhw1: input and/or output files are not specified\n");
        exit(-1);
    }
    globalArgs.infile_count = count;
    
    //reading the output file's name
    globalArgs.outfile = argv[optind++];    
    
    //reading the input file's names
    globalArgs.infiles = (const char** )&argv[optind];
	
    //debug function
    //display_arguments(globalArgs);
    
    
    rc = syscall(__NR_xconcat, (void*)&globalArgs, sizeof(globalArgs));
	if (rc >= 0){
		printf("syscall returned %d\n", rc);
    }
	else {
		printf("syscall returned %d \n[%s(%d)]\n", rc, strerror(errno), errno);
    }
    
	exit(rc);
}
