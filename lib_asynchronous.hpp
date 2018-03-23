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
#include <queue>

class AsyncManager {
private:
  std::queue<std::function<void (AsyncManager* const manager)>> asynchronous_functions;
  std::queue<std::future<void>> asynchronous_promises;
  bool end_program;
public:
  AsyncManager() : end_program(false) {}

  void execute () {
    // Recoge todas las funciones encoladas para ejecutar y alimenta al ejecutador
    std::async([&]() {
      while (!end_program) {
        while (!asynchronous_functions.empty()) {
          asynchronous_promises.push (std::async(asynchronous_functions.front(), this));
          asynchronous_functions.pop();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    });
    // Ejecutador de las funciones encoladas
    std::async([&]() {
      while (!end_program) {
        while (!asynchronous_promises.empty()) {
          asynchronous_promises.pop();
        }
      }
    });
  }

  void add_function (std::function<void (AsyncManager* const manager)> function) {
    asynchronous_functions.push(function);
  }

  void add_persistent_function (std::function<void (AsyncManager* const manager)> function) {
    auto persistent = [&](AsyncManager* const manager) {
      while (!AsyncManager::end_program) {
        function (manager);
      }
    };
    add_function(persistent);
  }

  void end_process () {
    end_program = true;
  }

  bool is_alive () const { return !end_program; }
};

class asyncObject {
protected:
  AsyncManager* manager;
public:
  asyncObject (AsyncManager* const mngr) :
    manager (mngr)
    {}
  void send_signal (std::function<void (AsyncManager* const)> slot) {
    manager->add_function(slot);
  }
};

#endif // LIB_ASYNC_H
