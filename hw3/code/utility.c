/*
 * Copyright (c) 2006-2014 Mohammad Ruhul Amin
 * Copyright (c) 2006-2014 Stony Brook University
 * Copyright (c) 2006-2014 The Research Foundation of SUNY
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "utility.h"

int encryption(struct file *infile, struct file *outfile, char *encryption_scheme)
{    
    int err = 0;
    struct crypto_cipher *tfm;
    char *inbuf = 0;
    char *outbuf = 0;

    size_t read_bytes = 0;
    size_t write_bytes = 0;    
    bool eof = 0;

	char *pkey = encryption_scheme;

    tfm = crypto_alloc_cipher("aes", 4, CRYPTO_ALG_ASYNC);
    if(IS_ERR(tfm)){    
        printk(KERN_INFO "TFM is NULL = %ld?\n", PTR_ERR(tfm));    
        err = -ENOENT;
        goto out;
    }
    
    crypto_cipher_setkey(tfm, pkey, strlen(pkey));
    inbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);
    if (!inbuf) {
        printk(KERN_DEBUG "Error allocating inbuf in compress\n");
        err = -ENOMEM;
        goto out;
    }

    outbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);
    if (!outbuf) {
        printk(KERN_DEBUG "Error allocating outbuf in compress\n");
        err = -ENOMEM;
        goto out;
    }

    memset(inbuf, 0, LARGE_PAGE_SIZE);
    memset(outbuf, 0, LARGE_PAGE_SIZE);

    do {
        read_bytes = vfs_read(infile, inbuf, 16, &infile->f_pos);
        if (read_bytes < 0) {
            printk(KERN_INFO "Reading Error\n");
            err = read_bytes;
            goto out;
        }
        eof = (read_bytes == 0);
        if (!eof) {
            if(read_bytes < 16) {                
                inbuf[read_bytes] = '\0';
                read_bytes += 1;
            }
            crypto_cipher_encrypt_one(tfm, outbuf, inbuf);
        } else {
            break;
        }

        write_bytes = read_bytes;
        err = vfs_write(outfile, outbuf, 16, &outfile->f_pos);
        if (err < 0) {
            printk(KERN_INFO "Writing Error\n");
            goto out;
        }

    } while (!eof);

out:
    if (tfm) {
        crypto_free_cipher(tfm);
    }    
    kfree(inbuf);    
    kfree(outbuf);    
    return err;
}


/*
*/
int file_encryption(char *input, char *output, char *encryption_scheme)
{
    int err;
    struct file *f1 = NULL;
    struct file *f2 = NULL;
    mm_segment_t fs;

    fs = get_fs();
    set_fs(get_ds());

    f1 = filp_open(input, O_RDONLY, 600);
    f2 = filp_open(output, O_RDWR | O_CREAT | O_TRUNC, 666);

    if(IS_ERR(f1)) {
        err = PTR_ERR(f1);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

    if(IS_ERR(f2)) {
        err = PTR_ERR(f2);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }
	
    err = encryption(f1, f2, encryption_scheme);

out:
    set_fs(fs);
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);
    if(f2 != NULL && !IS_ERR(f2))
        filp_close(f2, NULL);

    return err;
}


int decryption(struct file *infile, struct file *outfile, char *decryption_scheme)
{
    int err = 0;
    struct crypto_cipher *tfm;
    char *inbuf = 0;
    char *outbuf = 0;

    size_t read_bytes = 0;
    size_t write_bytes = 0;        
    bool eof = 0;

	char *pkey = decryption_scheme;

    tfm = crypto_alloc_cipher("aes", 4, CRYPTO_ALG_ASYNC);
    if(IS_ERR(tfm))
    {
        printk(KERN_INFO "TFM is NULL = %ld?\n", PTR_ERR(tfm));    
        err = -ENOENT;
        goto out;
    }
    
    crypto_cipher_setkey(tfm, pkey, strlen(pkey));

    inbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);
    if (!inbuf) {
        printk(KERN_DEBUG "Error allocating inbuf in compress\n");
        err = -ENOMEM;
        goto out;
    }

    outbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);    
    if (!outbuf) {
        printk(KERN_DEBUG "Error allocating outbuf in compress\n");
        err = -ENOMEM;
        goto out;
    }

    memset(inbuf, 0, LARGE_PAGE_SIZE);
    memset(outbuf, 0, LARGE_PAGE_SIZE);

    do {
        read_bytes = vfs_read(infile, inbuf, 16, &infile->f_pos);
        if (read_bytes < 0) {
            err = read_bytes;
            goto out;
        }

        eof = (read_bytes == 0);
        if (!eof) {
            crypto_cipher_decrypt_one(tfm, outbuf, inbuf);
        } else {
            break;
        }

        write_bytes = read_bytes;
        if(write_bytes != strlen(outbuf)) {
            write_bytes = strlen(outbuf);            
        }

        err = vfs_write(outfile, outbuf, write_bytes, &outfile->f_pos);
        if (err < 0) {
            goto out;
        }
    } while (!eof);


out:
    if (tfm) {
        crypto_free_cipher(tfm);
    }
    
    kfree(inbuf);
    kfree(outbuf);
    
    return err;
}

/*
*
*/
int file_decryption(char *input, char *output, char *decryption_scheme)
{
    int err;
    struct file *f1 = NULL;
    struct file *f2 = NULL;
    mm_segment_t fs;

    fs = get_fs();
    set_fs(get_ds());

    f1 = filp_open(input, O_RDONLY, 600);    
    if(IS_ERR(f1)) {
        err = PTR_ERR(f1);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }
    
    f2 = filp_open(output, O_RDWR | O_CREAT | O_TRUNC, 666);
    if(IS_ERR(f2)) {
        err = PTR_ERR(f2);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

    err = decryption(f1, f2, decryption_scheme);

out:
    set_fs(fs);
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);
    if(f2 != NULL && !IS_ERR(f2))
        filp_close(f2, NULL);

    return err;
}

/*
*/
int compression(struct file* infile, struct file* outfile, char *compression_scheme)
{
    int err;
    struct crypto_pcomp *tfm;
    struct comp_request req;

    char *inbuf = 0;
    char *outbuf = 0;

    size_t read_bytes = 0;
    size_t write_bytes = 0;
    loff_t inpos = 0;
    loff_t outpos = 0;

    bool eof = 0;
    UDBG;
    tfm = crypto_alloc_pcomp(compression_scheme, 0, 0);
    if (IS_ERR(tfm)) {
        tfm = 0;
        err = PTR_ERR(tfm);
        goto out;
    }
UDBG;
    err = crypto_compress_setup(tfm, 0, 0);
    if (err) {
        goto out;
    }
UDBG;
    err = crypto_compress_init(tfm);
    if (err) {
        goto out;
    }
    UDBG;
    inbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);    
    if (!inbuf) {
            printk(KERN_DEBUG "Error allocating inbuf in compress\n");
            err = -ENOMEM;
            goto out;
    }

    outbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);    
    if (!outbuf) {
        printk(KERN_DEBUG "Error allocating outbuf in compress\n");
        err = -ENOMEM;
        goto out;
    }

    memset(inbuf, 0, LARGE_PAGE_SIZE);
    memset(outbuf, 0, LARGE_PAGE_SIZE);
    UDBG;
    do {
        read_bytes = vfs_read(infile, inbuf, LARGE_PAGE_SIZE, &inpos);        
        if (read_bytes < 0) {
            printk(KERN_INFO "Compression File Reading Error\n");
            err = read_bytes;
            goto out;
        }
        infile->f_pos = inpos;
        req.avail_in = read_bytes;
        eof = (read_bytes == 0);
        req.next_in = inbuf;
        do {
            req.avail_out = LARGE_PAGE_SIZE;
            req.next_out = outbuf;
            if (!eof) {
                err = crypto_compress_update(tfm, &req);
                if (err < 0)
                    printk(KERN_INFO " Error in !EOF\n");
            } else {
                err = crypto_compress_final(tfm, &req);
                if (err < 0)
                    printk(KERN_INFO " Error in EOF\n");
            }

            if (err < 0) {
                printk(KERN_INFO "Crypto File Process Error\n");
                goto out;
            }

            write_bytes = LARGE_PAGE_SIZE - req.avail_out;            
            err = vfs_write(outfile, outbuf, write_bytes, &outpos);
            if(err > 0)
                printk(KERN_INFO "Total Written %d So Far with AVAIL = %d\n", err, req.avail_out);
            if (err < 0) {
                printk(KERN_INFO "Crypto File Writing Error = %d\n", write_bytes);
                goto out;
            }
            outfile->f_pos = outpos;
        } while (req.avail_out == 0);
    } while (!eof);
    UDBG;
out:
    if (tfm) {
        crypto_free_pcomp(tfm);
    }
    
    kfree(inbuf);    
    kfree(outbuf);
    
    return err;
}

int file_compression(char *input, char *output, char *compression_scheme)
{
    int err;
    struct file *f1 = NULL;
    struct file *f2 = NULL;
    mm_segment_t fs;

    fs = get_fs();
    set_fs(get_ds());

    f1 = filp_open(input, O_RDONLY, 600);
    f2 = filp_open(output, O_RDWR | O_CREAT | O_TRUNC, 666);

    if(IS_ERR(f1)) {
        err = PTR_ERR(f1);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

    if(IS_ERR(f2)) {
        err = PTR_ERR(f2);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }
	
    err = compression(f1, f2, compression_scheme);

out:
    set_fs(fs);
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);
    if(f2 != NULL && !IS_ERR(f2))
        filp_close(f2, NULL);

    return err;
}


static int decompression(struct file* infile, struct file* outfile, char *decompression_scheme)
{
    int err;
    struct crypto_pcomp *tfm;
    struct comp_request req;

    char *inbuf = 0;
    char *outbuf = 0;

    size_t read_bytes = 0;
    size_t write_bytes = 0;
    loff_t inpos = 0;
    loff_t outpos = 0;

    bool eof = 0;

    tfm = crypto_alloc_pcomp(decompression_scheme, 0, 0);
    if (IS_ERR(tfm)) {
        tfm = 0;
        err = PTR_ERR(tfm);
        goto out;
    }

    err = crypto_decompress_setup(tfm, 0, 0);
    if (err) {
        goto out;
    }

    err = crypto_decompress_init(tfm);
    if (err) {
        goto out;
    }

    inbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);    
    if (!inbuf) {
        printk(KERN_DEBUG "Error allocating inbuf in decompress\n");
        err = -ENOMEM;
        goto out;
    }

    outbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);
    if (!outbuf) {
        printk(KERN_DEBUG "Error allocating outbuf in decompress\n");
        err = -ENOMEM;
        goto out;
    }

    memset(inbuf, 0, LARGE_PAGE_SIZE);
    memset(outbuf, 0, LARGE_PAGE_SIZE);

    do {
        read_bytes = vfs_read(infile, inbuf, LARGE_PAGE_SIZE,&inpos);
        if (read_bytes < 0) {
            err = read_bytes;
            goto out;
        }
        infile->f_pos = inpos;
        req.avail_in = read_bytes;
        eof = (read_bytes == 0);
        req.next_in = inbuf;
        do {
            req.avail_out = LARGE_PAGE_SIZE;
            req.next_out = outbuf;
            if (!eof) {
                err = crypto_decompress_update(tfm, &req);
            } else {
                err = crypto_decompress_final(tfm, &req);
            }

            if (err < 0) {
                goto out;
            }

            write_bytes = LARGE_PAGE_SIZE - req.avail_out;
            err = vfs_write(outfile, outbuf, write_bytes, &outpos);
            if (err < 0) {
                goto out;
            }
            outfile->f_pos = outpos;
        } while (req.avail_out == 0);
    } while (!eof);
out:
    if (tfm) {
        crypto_free_pcomp(tfm);
    }
    
    kfree(inbuf);    
    kfree(outbuf);

    return err;
}


int file_decompression(char *input, char *output, char *decompression_scheme)
{
	int err;
    struct file *f1 = NULL;
    struct file *f2 = NULL;
    mm_segment_t fs;

    fs = get_fs();
    set_fs(get_ds());

    f1 = filp_open(input, O_RDONLY, 600);
    f2 = filp_open(output, O_RDWR | O_CREAT | O_TRUNC, 666);

    if(IS_ERR(f1)) {
    err = PTR_ERR(f1);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

    if(IS_ERR(f2)) {
    err = PTR_ERR(f2);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }
	
    err = decompression(f1, f2, decompression_scheme);

out:
    set_fs(fs);
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);
    if(f2 != NULL && !IS_ERR(f2))
        filp_close(f2, NULL);

    return err;
}


int checksum(struct file *infile, struct file *outfile, char *hashing_scheme) {
    int err = 0, i, l;
    int read_count = 0;
	int write_count = 0;
	unsigned char temp[SHA1_LENGTH * 2 + 1];
    struct scatterlist sg;
    struct crypto_hash *tfm = NULL;
    struct hash_desc desc;

    unsigned char *inbuf = NULL;
    unsigned char *outbuf = NULL;

    inbuf = kmalloc(LARGE_PAGE_SIZE, GFP_KERNEL);
    if (!inbuf) {
            printk(KERN_DEBUG "Error allocating inbuf in decompress\n");
            err = -ENOMEM;
            goto out;
    }

    outbuf = kmalloc(SHA1_LENGTH, GFP_KERNEL);    
    if (!outbuf) {
            printk(KERN_DEBUG "Error allocating outbuf in decompress\n");
            err = -ENOMEM;
            goto out;
    }

    memset(inbuf, 0, LARGE_PAGE_SIZE);
    memset(outbuf, 0, SHA1_LENGTH);

    //tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);
	tfm = crypto_alloc_hash(hashing_scheme, 0, CRYPTO_ALG_ASYNC);
    desc.tfm = tfm;
    desc.flags = 0;
    crypto_hash_init(&desc);

    do{
        read_count = vfs_read(infile, inbuf, LARGE_PAGE_SIZE, &infile->f_pos);                        
        if(read_count < 0)
        {
            err = read_count;
            printk(KERN_INFO "Error with File Reading\n");
            goto out;
        }

        if(read_count > 0) {
            sg_init_one(&sg, inbuf, read_count);
            crypto_hash_update(&desc, &sg, read_count);
        }
        else {
            crypto_hash_final(&desc, outbuf);
        }
    }while(read_count > 0);

	
	temp[0] = '\0';	
	for (i = 0; i < SHA1_LENGTH; i++)
    {        
		l = strlen(temp);
		sprintf(&temp[l], "%02x", outbuf[i]);		
	}
	printk(KERN_INFO "\n");

	write_count = vfs_write(outfile, temp, strlen(temp), &outfile->f_pos);
	if(write_count < strlen(temp))
	{
		err = EIO;
		printk(KERN_INFO "Hashing File Write Error %d\n", err);
        goto out;
	}

out:
    crypto_free_hash(tfm);
    if(inbuf != NULL)
        kfree(inbuf);
    if(outbuf != NULL)
        kfree(outbuf);
	if(DEBUG_MODE == 1) printk(KERN_INFO "Hashing Returns Value = %d\n", read_count);
    return err;
}

int file_checksum(char *input, char *output, char *hashing_scheme)
{
    int err;
    struct file *f1 = NULL;
	struct file *f2 = NULL;
    mm_segment_t fs;

    fs = get_fs();
    set_fs(get_ds());

    f1 = filp_open(input, O_RDONLY, 600);
	f2 = filp_open(output, O_RDWR | O_CREAT | O_TRUNC, 666);

    if(IS_ERR(f1)) {
    err = PTR_ERR(f1);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

	 if(IS_ERR(f2)) {
        err = PTR_ERR(f2);
        printk(KERN_INFO "File Error In The Beginning. (%d)\n", err);
        goto out;
    }

    err = checksum(f1, f2, hashing_scheme);

out:
    set_fs(fs);
    if(f1 != NULL && !IS_ERR(f1))
        filp_close(f1, NULL);
	if(f2 != NULL && !IS_ERR(f2))
        filp_close(f2, NULL);
    return err;
}
