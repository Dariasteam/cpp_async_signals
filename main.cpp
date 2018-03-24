#include <iostream>
#include <future>
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <queue>
#include <fstream>

#include "lib_asynchronous.hpp"
#include "ui.h"

void do_stuff_in_background () {
  C::counter++;
}

int main() {
  AsyncManager manager;

  //manager.add_persistent_function(do_stuff_in_background);

  UI ui (&manager);
  ui.wait_for_user_input();

  std::cout << "END_PROGRAM" << std::endl;
}


