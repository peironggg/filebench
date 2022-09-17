#include "filebench.h"

struct MemoryStruct
{
  char *memory;
  size_t size;
};

int fb_ipfs_write(char *path, caddr_t iobuf, fbint_t iosize, off64_t offset);
int fb_ipfs_read(char *path, caddr_t iobuf, fbint_t iosize, off64_t fileoffset);
int fb_ipfs_generic_post(char *url);
