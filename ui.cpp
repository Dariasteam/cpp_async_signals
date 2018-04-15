/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Daniel Darias Sánchez <dariasteam94@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui.h"

UI::UI (asyncManager* manager) :
  asyncObject(manager),
  a (manager),
  b (manager),
  lg (manager)
  {
  // CONNECTS ------------------------------------------------------------------
  signal_1 = [&]() {
    send_signal(std::bind(&UI::on_handle_response_1, this));
  };
  signal_2 = [&]() {
    send_signal(std::bind(&UI::on_handle_response_2, this));
  };
  signal_3 = [&](unsigned n) {
    send_signal(std::bind(&UI::on_handle_response_3, this, n));
  };
  signal_start_loggin = [&]() {
    send_signal(std::bind(&logger::on_start_logging, &lg));
  };

  // ENTRY POINT ---------------------------------------------------------------
  manager->add_persistent_function(std::bind(&UI::wait_for_user_input, this));
}

bool UI::wait_for_user_input() {
  unsigned user_input = 0;
  std::cout
  << "[0] Show this menu"<< "\n"
  << "[1] Execute algorithm (2)"<< "\n"
  << "[2] Execute algorithm (1) and (2) sequentially"<< "\n"
  << "[3] Execute 10 instances of (1) then (2) parallel"<< "\n"
  << "[4] Connect a & b objects (a ---> b)"<< "\n"
  << "[5] Disconnect a & b objects (a -/-> b)"<< "\n"
  << "[6] Emit signal (a --> )"<< "\n"
  << "[7] Start logging"<< "\n"
  << "[8] Stop logging"<< "\n"
  << "[9] Exit"<< "\n";
  std::cin >> user_input;
  return master_handler (user_input);
}

bool UI::master_handler(unsigned option) {
  switch (option) {
    case 1:
      signal_1();
      break;
    case 2:
      signal_2();
      break;
    case 3:
      signal_3(10);
      break;
    case 4:
      a.signal = [&](unsigned i, unsigned j) {
        a.send_signal(std::bind (&myObject::slot, b, i, j));
      };
      break;
    case 5:
      a.signal = [&](unsigned i, unsigned j) {};
      break;
    case 6:
      a.signal(40, 20);
      break;
    case 7:
      signal_start_loggin();
      break;
    case 8:
      signal_stop_loggin();
      break;
    case 9:
      manager->end_safe();
      return false;
      break;
  }
  return true;
}

void UI::on_handle_response_1() {
  std::cout <<  "\t(2) Starting\n" << std::flush;
  long long unsigned a = 2;
  while (a < 4500000000) a = std::pow(a, 3);
  a--;

  std::cout <<  "\t(2) Finished\n" << std::flush;
}

void UI::on_handle_response_2() {
  std::cout <<  "\t(1) Starting\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout <<  "\t(1) Finished\n" << std::flush;
  on_handle_response_1();
}

void UI::on_handle_response_3(unsigned n) {
  std::cout << "Starting parallel block\n" << std::flush;
  std::vector <std::future<void>> promises (n);
  for (auto& promise : promises)
    promise = std::async(&UI::on_handle_response_2, this);

  for (auto& promise : promises)
    promise.get();

  std::cout << "Parallel block finished" << std::endl;
}

void myObject::slot (int i, int j) {
  std::cout << "I've been called with values " << i << ", " << j << std::endl;
};
