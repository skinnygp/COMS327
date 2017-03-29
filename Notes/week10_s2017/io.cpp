#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
  ifstream f("input_file.txt");
  int i;
  string str;
  char s[80];

  i = f.get();

  cout << (char) i << endl;
  cout << (char) f.get() << endl;
  cout << (char) f.get() << endl;
  cout << (char) f.get() << endl;
  cout << (char) f.get() << endl;
  cout << (char) f.get() << endl;
  cout << (char) f.get() << endl;

  cout << (char) f.peek() << endl;
  cout << (char) f.peek() << endl;
  cout << (char) f.peek() << endl;
  cout << (char) f.peek() << endl;
  cout << (char) f.peek() << endl;
  cout << (char) f.peek() << endl;

  f.getline(s, 80);

  cout << s << endl;

  getline(f, str, 'e');

  cout << str << 'e';

  getline(f, str);

  cout << str << endl;

  f >> str;
  cout << str << endl;

  f >> str;
  cout << str << endl;

  f.get(); // newline at the end of line 4
  f.get(); // blank line 

  getline(f, str);

  cout << str << endl;

  stringstream ss;

  ss << "This is going into my string." << endl;
  ss << "So is this!" << endl;

  cout << ss.str();

  str = "This is a string.";

  cout << str << endl;
  
  str += "  This is more on my string.";

  cout << str << endl;

  str.push_back('\n');

  cout << str;

  f.close(); // We're done with our file

  vector<int> v; //More to come next week

  return 0;
}
