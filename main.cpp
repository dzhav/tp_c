#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
  std::string place_name = argv[1];
  std::ifstream infile("/home/dzhavid/fld/mbdump/area");
  std::string line;
  int cnt = 0;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::string subs;
    iss >> subs;
    iss >> subs;
    iss >> subs;
    if (subs == place_name) cnt+=1;
  }
  std::cout<<cnt;
  infile.close();
  return 0;
}
