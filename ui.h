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

#ifndef UI_H
#define UI_H

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

struct C {
  static long long unsigned counter;
};

class myObject : public asyncObject {
private:
  std::string name;
public:
  myObject (AsyncManager* const mngr) :
    asyncObject(mngr)
    {}
  // SLOTS ---------------------------------------------------------------------
  void slot (int i, int j);
  // SIGNALS -------------------------------------------------------------------
  std::function<void (unsigned, unsigned)> signal;
};


/**
 * @todo write docs
 */
class UI : public asyncObject {
private:
  myObject a;
  myObject b;
public:
  UI (AsyncManager* manager);

  void wait_for_user_input ();
  void master_handler (unsigned option);

private:
  // SLOTS ---------------------------------------------------------------------
  void on_handle_response_0 ();
  void on_handle_response_1 ();
  void on_handle_response_2 ();
  void on_handle_response_3 (unsigned n);
  // SIGNALS -------------------------------------------------------------------
  std::function<void (void)> signal_0;
  std::function<void (void)> signal_1;
  std::function<void (void)> signal_2;
  std::function<void (unsigned)> signal_3;
};

#endif // UI_H
