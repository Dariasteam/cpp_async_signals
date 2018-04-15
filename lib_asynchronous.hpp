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

#include <functional>
#include <thread>
#include <atomic>
#include <condition_variable>

#define VERBOSE

class asyncManager {
private:
  std::atomic<bool> end_loop;
  long long n_threads;
  std::mutex m;
  std::condition_variable cv;

public:
  asyncManager() :
    n_threads(0)
    {
      end_loop.store(false);
    }

  ~asyncManager() {
    end_safe();
  }

  void start() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&]{return !is_alive();});
  }

  void add_function (std::function<void (void)> function) {
    auto ff = [&, function]() {
      function();
      --n_threads;
    };
    ++n_threads;
    std::thread t (ff);
    t.detach();
  }

  void add_persistent_function (std::function<bool (void)> function) {
    auto persistent = [&, function](void) {
      while (is_alive() && function ());
      --n_threads;
    };
    ++n_threads;
    std::thread t (persistent);
    t.detach();
  }

  bool end_safe () {
    if (!end_loop.load()) {
      end_loop.store(true);
      #ifdef VERBOSE
      std::cerr << "Waiting for threads to finish " << n_threads << std::endl;
      #endif
      while (n_threads > 1) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(3));
      }
      #ifdef VERBOSE
      std::cerr << "All threads finished" << std::endl;
      #endif
      cv.notify_one();
      return true;
    }
    return true;
  }

  void end_unsafe () {
    if (!end_loop.load()) {
      end_loop.store(true);
      cv.notify_one();
  }
  }

  bool is_alive () const { return !end_loop.load(); }
};

class asyncObject {
protected:
  asyncManager* manager;
public:
  asyncObject (asyncManager* const mngr) :
    manager (mngr)
    {}
  void send_signal (std::function<void (void)> slot) {
    manager->add_function(slot);
  }
};

#endif // LIB_ASYNC_H
