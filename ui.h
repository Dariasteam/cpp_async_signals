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
#include <functional>
#include <future>
#include <vector>
#include <chrono>
#include <fstream>
#include <cmath>

#include "lib_asynchronous.hpp"
#include "logger.h"

class myObject : public asyncObject {
private:
  std::string name;
public:
  myObject (asyncManager* const mngr) :
    asyncObject(mngr)
    {}
  // SLOTS ---------------------------------------------------------------------
  void slot (int i, int j);
  // SIGNALS -------------------------------------------------------------------
  std::function<void (unsigned, unsigned)> signal = [](unsigned, unsigned){};
};

/**
 * @todo write docs
 */
class UI : public asyncObject {
private:
  myObject a;
  myObject b;
  logger lg;
public:
  UI (asyncManager* manager);

  bool wait_for_user_input (void);
  bool master_handler (unsigned option);

private:
  // SLOTS ---------------------------------------------------------------------
  void on_handle_response_1 ();
  void on_handle_response_2 ();
  void on_handle_response_3 (unsigned n);
  // SIGNALS -------------------------------------------------------------------
  std::function<void (void)> signal_1 = []{};
  std::function<void (void)> signal_2 = []{};
  std::function<void (unsigned)> signal_3 = [](unsigned){};
  std::function<void (void)> signal_start_loggin = []{};
  std::function<void (void)> signal_stop_loggin = [&] {
    send_signal(std::bind(&logger::on_stop_logging, &lg));
  };
};

#endif // UI_H
