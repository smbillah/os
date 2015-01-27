/*
 * Copyright (c) 2006-2014 Mohammad Ruhul Amin
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _UTILITY_H
#define _UTILITY_H

#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/audit.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/namei.h>
#include <linux/fs_stack.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <linux/err.h>
#include <linux/scatterlist.h>
#include <linux/fs.h>
#include <linux/zlib.h>
#include <linux/crypto.h>
#include <linux/netlink.h>
#include <crypto/compress.h>

#include "xjob.h"

/*Maximum number of input files this program support for concatenation.*/
#define MAX_NO_INFILE_SUPPORTED 32
#define DEBUG_MODE 1
#define DEBUG_ATOMIC 0
#define SHA1_LENGTH 20
#define BUF_LENGTH 40
#define LARGE_PAGE_SIZE 16384


extern int file_encryption(char *input, char *output, char *encryption_scheme);
extern int file_decryption(char *input, char *output, char *decryption_scheme);
extern int file_compression(char *input, char *output, char *compression_scheme);
extern int file_decompression(char *input, char *output, char *decompression_scheme);
extern int file_checksum(char *input, char *output, char *hashing_scheme);

#endif
