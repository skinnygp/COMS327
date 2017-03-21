#ifndef STRING327_H
# define STRING327_H

# include <iostream>
# include <cstring>

using namespace std;

class string327 {
 private:
  char *str;
 public:
  string327();
  string327(const char *s);
  ~string327();
  inline int length() const
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strlen(str);
  }
  inline const char *c_str() const
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return str;
  }
  inline int operator<(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) < 0;
  }
  inline int operator<=(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) <= 0;
  }
  inline int operator>(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) > 0;
  }
  inline int operator>=(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) >= 0;
  }
  inline int operator==(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) == 0;
  }
  inline int operator!=(const string327 &s)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return strcmp(str, s.str) != 0;
  }
  string327 operator+(const string327 &s);
  string327 &operator+=(const string327 &s);
  const string327 &operator=(const char *s);
  const string327 &operator=(const string327 &s);
  inline char &operator[](const int i)
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return str[i];
  }
  inline operator const char *()
  {
    cout << __PRETTY_FUNCTION__ << endl;

    return str;
  }

  friend std::istream &operator>>(std::istream &i, const string327 &str);
};

std::ostream &operator<<(std::ostream &o, const string327 &str);

#endif
