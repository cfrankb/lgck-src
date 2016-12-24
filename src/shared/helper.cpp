#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <list>
#include <FileWrap.h>

const char *toUpper(char *s)
{
    for (unsigned int i=0; i < strlen(s);++i){
        if (isalpha(s[i])) {
            s[i] = toupper(s[i]);
        }
    }
    return s;
}
 
char *getUUID()
{
    char *uuid = new char[128];
    sprintf(uuid, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
            rand(),
            rand() & 0xffff,
            rand() & 0xffff,
            rand() & 0xffff,
            rand(),
            rand() & 0xffff
            );
    return uuid;
}

bool copyFile(const std::string in, const std::string out, std::string errMsg)
{
    bool result = true;
    CFileWrap sfile;
    CFileWrap tfile;
    if (sfile.open(in.c_str())) {
        int size = sfile.getSize();
        char *buf = new char[size];
        sfile.read(buf, size);
        sfile.close();
        if (tfile.open(out.c_str(), "wb")) {
            tfile.write(buf, size);
            tfile.close();
        } else {
            char tmp[out.length() + 128];
            sprintf(tmp, "couldn't write: %s", out.c_str());
            result = false;
        }
        delete [] buf;
    } else {
        char tmp[in.length() + 128];
        sprintf(tmp, "couldn't write: %s", in.c_str());
        errMsg = tmp;
        result = false;
    }
    return result;
}

bool concat(const std::list<std::string> files, std::string out, std::string msg)
{
    CFileWrap tfile;
    bool result = true;
    if (tfile.open(out.c_str(), "wb")) {
        for (std::list<std::string>::const_iterator iterator = files.begin(), end = files.end(); iterator != end; ++iterator) {
            CFileWrap sfile;
            std::string in = *iterator;
            if (sfile.open(in.c_str())) {
                int size = sfile.getSize();
                char *buf = new char[size];
                sfile.read(buf, size);
                sfile.close();
                tfile.write(buf, size);
                delete [] buf;
            } else {
                char tmp[in.length() + 128];
                sprintf(tmp, "couldn't read: %s", in.c_str());
                msg = tmp;
                result = false;
                break;
            }
        }
        tfile.close();
    } else {
        char tmp[out.length() + 128];
        sprintf(tmp, "couldn't write: %s", out.c_str());
        msg = tmp;
        result = false;
    }
    return result;
}

#ifdef _WIN32
#include <windows.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

char *realpath(const char *path, char resolved_path[PATH_MAX])
{
  char *return_path = 0;

  if (path) //Else EINVAL
  {
    if (resolved_path)
    {
      return_path = resolved_path;
    }
    else
    {
      //Non standard extension that glibc uses
      return_path = (char*)malloc(PATH_MAX);
    }

    if (return_path) //Else EINVAL
    {
      //This is a Win32 API function similar to what realpath() is supposed to do
      size_t size = GetFullPathNameA(path, PATH_MAX, return_path, 0);

      //GetFullPathNameA() returns a size larger than buffer if buffer is too small
      if (size > PATH_MAX)
      {
        if (return_path != resolved_path) //Malloc'd buffer - Unstandard extension retry
        {
          size_t new_size;

          free(return_path);
          return_path = (char*)malloc(size);

          if (return_path)
          {
            new_size = GetFullPathNameA(path, size, return_path, 0); //Try again

            if (new_size > size) //If it's still too large, we have a problem, don't try again
            {
              free(return_path);
              return_path = 0;
              errno = ENAMETOOLONG;
            }
            else
            {
              size = new_size;
            }
          }
          else
          {
            //I wasn't sure what to return here, but the standard does say to return EINVAL
            //if resolved_path is null, and in this case we couldn't malloc large enough buffer
            errno = EINVAL;
          }
        }
        else //resolved_path buffer isn't big enough
        {
          return_path = 0;
          errno = ENAMETOOLONG;
        }
      }

      //GetFullPathNameA() returns 0 if some path resolve problem occured
      if (!size)
      {
        if (return_path != resolved_path) //Malloc'd buffer
        {
          free(return_path);
        }

        return_path = 0;

        //Convert MS errors into standard errors
        switch (GetLastError())
        {
          case ERROR_FILE_NOT_FOUND:
            errno = ENOENT;
            break;

          case ERROR_PATH_NOT_FOUND: case ERROR_INVALID_DRIVE:
            errno = ENOTDIR;
            break;

          case ERROR_ACCESS_DENIED:
            errno = EACCES;
            break;

          default: //Unknown Error
            errno = EIO;
            break;
        }
      }

      //If we get to here with a valid return_path, we're still doing good
      if (return_path)
      {
        struct stat stat_buffer;

        //Make sure path exists, stat() returns 0 on success
        if (stat(return_path, &stat_buffer))
        {
          if (return_path != resolved_path)
          {
            free(return_path);
          }

          return_path = 0;
          //stat() will set the correct errno for us
        }
        //else we succeeded!
      }
    }
    else
    {
      errno = EINVAL;
    }
  }
  else
  {
    errno = EINVAL;
  }

  return return_path;
}
#else
#endif
