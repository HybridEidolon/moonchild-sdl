#include "mmio.hpp"

#ifdef _WIN32
#  include <Windows.h>
#else
#  ifndef _LARGEFILE64_SOURCE
#    define _LARGEFILE64_SOURCE
#  endif
#  include <sys/mman.h>
#  include <sys/types.h>
#  include <fcntl.h>
#  include <unistd.h>
#endif


memmapped map_file(const char* path) {
  memmapped m {};

#ifdef _WIN32
  m.hfile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (m.hfile == INVALID_HANDLE_VALUE) {
    return m;
  }
  LARGE_INTEGER filesize;
  GetFileSizeEx(m.hfile, &filesize);
  m.size = filesize.QuadPart;
  m.hmap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (m.hmap == INVALID_HANDLE_VALUE) {
    CloseHandle(h.hfile);
    h.hfile = INVALID_HANDLE_VALUE;
    return m;
  }
  m.data = MapViewOfFile((*mmapped)->hMap, FILE_MAP_READ, 0, 0, 0);
  m.valid = true;

#else
  int fd;
  if ((fd = open(path, O_RDONLY)) == -1) {
    return m;
  }
  off_t filesize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  m.data = mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
  m.fd = fd;
  m.size = filesize;
  m.valid = true;

#endif

  return m;
}

void close_mapped_file(memmapped& m) {
#ifdef _WIN32
  UnmapViewOfFile(m.data);
  CloseHandle(m.hmap);
  CloseHandle(m.hfile);
  m = {};
#else
  munmap(m.data, m.size);
  close(m.fd);
  m = {};
#endif
}
