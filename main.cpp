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

void do_stuff_in_background (AsyncManager* const manager) {
  C::counter++;
  std::this_thread::sleep_for(std::chrono::microseconds(10));
}

int main() {
  AsyncManager manager;

  UI ui (&manager);
  ui.wait_for_user_input();

  manager.add_persistent_function(do_stuff_in_background);
  //manager.add_persistent_function(generate_logs);
  manager.execute();
}


