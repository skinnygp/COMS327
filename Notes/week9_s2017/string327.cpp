#include <cstring>
#include <cstdlib>

#include "string327.h"

string327::string327()
{
    cout << __PRETTY_FUNCTION__ << endl;

  str = (char *) malloc(1);
  *str = '\0';
}

string327::string327(const char *s)
{
    cout << __PRETTY_FUNCTION__ << endl;

  str = strdup(s);
}

string327::~string327()
{
    cout << __PRETTY_FUNCTION__ << endl;

  free(str);
}

string327 string327::operator+(const string327 &s)
{
  string327 out;

    cout << __PRETTY_FUNCTION__ << endl;

  free(out.str);
  out.str = (char *) malloc(strlen(str) + strlen(s.str) + 1);
  strcpy(out.str, str);
  strcat(out.str, s.str);

  return out;
}

string327 &string327::operator+=(const string327 &s)
{
    cout << __PRETTY_FUNCTION__ << endl;

  str = (char *) realloc(str, length() + strlen(s.str) + 1); /* Not good practice */
  strcat(str, s.str);

  return *this;
}

const string327 &string327::operator=(const char *s)
{
    cout << __PRETTY_FUNCTION__ << endl;

  free(str);
  str = strdup(s);

  return *this;
}

const string327 &string327::operator=(const string327 &s)
{
    cout << __PRETTY_FUNCTION__ << endl;

  free(str);
  str = strdup(s.str);

  return *this;
}

std::istream &operator>>(std::istream &i, const string327 &str)
{
    cout << __PRETTY_FUNCTION__ << endl;

  i >> str.str; //Important bug here!
                //Fixing this bug is actually a good bit of work
                //We'll learn more about this after the break
  return i;
}

std::ostream &operator<<(std::ostream &o, const string327 &str)
{
    cout << __PRETTY_FUNCTION__ << endl;

  return o << str.c_str();
}
