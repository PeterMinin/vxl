// This is brl/bbas/baio/baio_windows.cxx
#include "baio.h"
//:
// \file
#include <vcl_iostream.h> //for vcl_cout
#include <vcl_cstdio.h>   //for vcl_perror
#include <vcl_cstring.h>  //for vcl_memset
//windows specific includes
#include <windows.h>

//: baio_info struct: wrapper for status variables
struct baio_info {
  //: insert status variable and data buffer
  baio_info() {
    vcl_memset(&o, 0, sizeof(OVERLAPPED));
    o.Offset = 0;
    o.OffsetHigh = 0;
    o.hEvent = 0;
    buffer   = 0;
    fhandle  = 0;
    status   = false;
  }
  OVERLAPPED o;
  char* buffer;
  HANDLE fhandle;
  bool status;
  long buffer_length_;
};

baio::baio()
{
  info_ = new baio_info();
}

baio::~baio()
{
  if (info_)
    delete info_;
}

//: Opens and reads file asynchronously
bool baio::read(vcl_string filename, char* buffer, long BUFSIZE)
{
  info_->fhandle = CreateFile(filename.c_str(),
                              FILE_READ_DATA,
                              FILE_SHARE_READ,
                              NULL, OPEN_EXISTING,
                              FILE_FLAG_NO_BUFFERING|FILE_FLAG_OVERLAPPED, NULL);
  if (info_->fhandle== INVALID_HANDLE_VALUE) {
    vcl_cerr<<"baio (Windows)::read could not open file "<<filename<<'\n';
    vcl_perror("open");
  }

  info_->buffer=buffer;
  // 3. Allocate a data buffer for the aiocb request
  if (!info_->buffer) {
    vcl_cerr<<"baio (Windows)::read could not allocate buffer of size "<<BUFSIZE<<'\n';
    vcl_perror("malloc");
  }
  info_->buffer_length_=BUFSIZE;
  //
  DWORD bytesRead = 0;

  vcl_cout << "reading..." << vcl_endl;
  info_->status = ReadFile(info_->fhandle, info_->buffer, BUFSIZE, &bytesRead, &(info_->o));
  return info_->status;
}

//: Opens and reads file asynchronously
bool baio::write(vcl_string filename, char* buffer, long BUFSIZE)
{
  info_->fhandle = CreateFile(filename.c_str(),
                              FILE_WRITE_DATA,
                              FILE_SHARE_WRITE,
                              NULL, CREATE_ALWAYS,
                              FILE_FLAG_WRITE_THROUGH|FILE_FLAG_OVERLAPPED, NULL);
  if (info_->fhandle== INVALID_HANDLE_VALUE) {
    vcl_cerr<<"baio (Windows)::read could not open file "<<filename<<'\n';
    vcl_perror("open");
  }

  info_->buffer=buffer;
  if (!info_->buffer) {
    vcl_cerr<<"baio (Windows)::the buffer does not exist, BUFSIZE = "<<BUFSIZE<<'\n';
  }
  //
  DWORD byteswritten = 0;

  vcl_cout << "reading..." << vcl_endl;
  info_->status = WriteFile(info_->fhandle, info_->buffer, BUFSIZE, &byteswritten, &(info_->o));
  return info_->status;
}

void baio::close_file()
{
    CloseHandle(info_->fhandle);
}

baio_status baio::status()
{
  int status = HasOverlappedIoCompleted(&(info_->o));
  if (!status) {
    return BAIO_IN_PROGRESS;
  }
  else {
    return BAIO_FINISHED;
  }
}

char* baio::buffer()
{
  return info_->buffer;
}

long baio::buffer_size()
{
  return (long) info_->buffer_length_;
}
