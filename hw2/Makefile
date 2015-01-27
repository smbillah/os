WRAPFS_VERSION="0.1"

EXTRA_CFLAGS += -DWRAPFS_VERSION=\"$(WRAPFS_VERSION)\"

obj-$(CONFIG_WRAP_FS) += wrapfs.o

wrapfs-y := subr.o dentry.o file.o inode.o main.o super.o \
	rdstate.o copyup.o dirhelper.o rename.o unlink.o \
	lookup.o commonfops.o dirfops.o sioq.o mmap.o whiteout.o

