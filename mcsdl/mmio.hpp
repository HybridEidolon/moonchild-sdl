#ifndef MCSDL_MMIO_HPP
#define MCSDL_MMIO_HPP

#ifdef _WIN32
#  include <cstdint>
#endif

struct memmapped {
  void* data;
  unsigned long long size;
  bool valid;
#ifdef _WIN32
  uintptr_t hfile;
  uintptr_t hmap;
#else
  int fd;
#endif
};

memmapped map_file(const char* path);
void close_mapped_file(memmapped& m);

#endif // MCSDL_MMIO_HPP
