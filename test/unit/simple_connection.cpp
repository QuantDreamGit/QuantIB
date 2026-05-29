#include <cassert>
#include <ostream>

#include "quantib/core/ib.hpp"

int main() {
  auto ib = new IB();

  auto isConnect = ib->connect();

  std::cout << isConnect.value() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  return 0;
}
