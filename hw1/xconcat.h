#ifndef XCONCAT_H_
#define XCONCAT_H_

typedef struct golabalArgs_t
{
    const char *outfile; // name of output file
	const char **infiles; // array with names of input files
	unsigned int infile_count; // number of input files in infiles array
	int oflags; // Open flags to change behavior of syscall
	mode_t mode; // default permission mode for newly created outfile
	unsigned int flags; // special flags to change behavior of syscall
} golabalArgs_t; 

#endif /* XCONCAT_H_ */
