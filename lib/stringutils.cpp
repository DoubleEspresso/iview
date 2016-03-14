#include "stringutils.h"

char * StringUtils::get_file_extension(char * fname)
{
  char * res = new char[std::strlen(fname)];  
  if (fname == 0) return fname;
  for (int c = std::strlen(fname); fname[c] != '\0'; --c)
    {
      if (fname[c] == '.' || fname[c] == ' ' || fname[c] == ',') break;
      res[c] = fname[c];
    }
  return res;
}
