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

#ifndef LIB_ASYNC_H
#define LIB_ASYNC_H

#include <future>
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

class AsyncManager {
private:
  bool end_loop;
public:
  AsyncManager() :
    end_loop(false)
    {}

  void add_function (std::function<void (void)> function) {
    std::thread t (function);
    t.detach();
  }

  void add_function (std::function<void (AsyncManager* const)> function) {
    std::thread t (function, this);
    t.detach();
  }

  void add_persistent_function (std::function<void (AsyncManager* const)> function) {
    auto persistent = [&](void) {
      while (!end_loop)
        function (this);
    };
    add_function(persistent);
  }

  void add_persistent_function (std::function<void (void)> function) {
    auto persistent = [&](void) {
      while (!end_loop)
        function ();
    };
    add_function(persistent);
  }

  void end_process () {
    end_loop = true;
  }

  bool is_alive () const { return !end_loop; }
};

class asyncObject {
protected:
  AsyncManager* manager;
public:
  asyncObject (AsyncManager* const mngr) :
    manager (mngr)
    {}
  void send_signal (std::function<void (void)> slot) {
    manager->add_function(slot);
  }
};

#endif // LIB_ASYNC_H
