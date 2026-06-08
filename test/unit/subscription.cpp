#include "quantib/sync/ib.hpp"
#include <iostream>

int main() {
  auto ib = new IB();
  auto ok = ib->connect();
  ib->accountSummarySub();
  std::cout << "Value: " << ok.value() << std::endl;
  std::cin.get();
}
