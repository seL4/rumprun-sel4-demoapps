#define RUMPCMDLINE "lmbench fsync=1 if=/data2/file.txt of=/data2/file2.txt "

#define RUMPCONFIG "{,, \"blk\" :  {,,\"source\":\"dev\",,\"path\":\"/dev/wd0a\",,\"fstype\":\"blk\",,\"mountpoint\":\"/data2/\",,},,\"cmdline\": \""RUMPCMDLINE"\",,},,"
