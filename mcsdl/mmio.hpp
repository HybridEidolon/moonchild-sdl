#ifndef MCSDL_MMIO_HPP
#define MCSDL_MMIO_HPP

struct memmapped {
  void* data;
  unsigned long long size;
  bool valid;
#ifdef _WIN32
  void* hfile;
  void* hmap;
#else
  int fd;
#endif
};

memmapped map_file(const char* path);
void close_mapped_file(memmapped& m);

#endif // MCSDL_MMIO_HPP
