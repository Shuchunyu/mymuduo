#include "AppendFile.h"
#include "Logging.h"

AppendFile::AppendFile(std::string filename)
  : fp_(::fopen(filename.c_str(), "ae")),  // 'e' for O_CLOEXEC
    writtenBytes_(0)
{
  ::setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
  ::fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
  size_t written = 0;

  while (written != len)
  {
    size_t remain = len - written;
    size_t n = write(logline + written, remain);
    if (n != remain)
    {
      int err = ferror(fp_);
      if (err)
      {
        fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
        break;
      }
    }
    written += n;
  }

  writtenBytes_ += written;
}

size_t AppendFile::write(const char* logline, size_t len)
{
  return ::fwrite_unlocked(logline, 1, len, fp_);
}

void AppendFile::flush()
{
  ::fflush(fp_);
}