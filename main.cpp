#include <iostream>
#include <future>
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <queue>
#include <fstream>

long long unsigned counter;

class AsyncManager {
private:
  std::queue<std::function<void (unsigned, AsyncManager* const manager)>> asynchronous_functions;
  unsigned id;
  std::queue<std::future<void>> asynchronous_promises;
public:
  bool end_program;
  AsyncManager() : end_program(false) {}

  void execute () {
    std::vector<std::future<void>> promises (2);

    // Recoge todas las funciones encoladas para ejecutar y alimenta al ejecutador
    promises[0] = std::async([&]() {
      while (!end_program) {
        while (!asynchronous_functions.empty()) {
          asynchronous_promises.push (std::async(asynchronous_functions.front(), id, this));
          asynchronous_functions.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });

    // Ejecutador de las funciones encoladas
    promises[1] = std::async([&](){
      while (!end_program) {
        while (!asynchronous_promises.empty()) {
          asynchronous_promises.front().wait();
          asynchronous_promises.pop();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });

    for (auto& p : promises) {
      p.wait();
    }
  }

  void add_function (std::function<void (unsigned, AsyncManager* const manager)> function) {
    id++;
    asynchronous_functions.push(function);
  }

  void add_persistent_function (std::function<void (unsigned, AsyncManager* const manager)> function) {
    auto persistent = [&](unsigned i, AsyncManager* const manager) {
      while (!AsyncManager::end_program) {
        function (i, manager);
      }
    };
    add_function(persistent);
  }

  void end_process () {
    end_program = true;
  }
};

void handle_response_0 (unsigned, AsyncManager* const manager) {
  std::cout << "\t" << "El contador en segundo plano marca " << counter << "\n" << std::flush;
}

void handle_response_2 (unsigned i, AsyncManager* const manager) {
  std::cout << "\t" << "Comenzando B " << i << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "B finalizado " << i << "\n" << std::flush;
}

void handle_response_1 (unsigned i, AsyncManager* const manager) {
  std::cout << "\t" << "Comenzando A " << i << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "A finalizado " << i << "\n" << std::flush;
  handle_response_2(i, manager);
}

void handle_response_3 (unsigned i, AsyncManager* const manager) {
  std::vector <std::future<void>> promises (10);
  for (auto& promise : promises)
    promise = std::async(handle_response_1, i, manager);

  for (auto& promise : promises)
    promise.get();
}

void handle_response_4 (AsyncManager* const manager) {
  manager->end_process();
}

void master_handler (unsigned option, AsyncManager* const manager) {
  switch (option) {
    case 0:
      manager->add_function(&handle_response_0);
      break;
    case 1:
      manager->add_function(&handle_response_1);
      break;
    case 2:
      manager->add_function(&handle_response_2);
      break;
    case 3:
      manager->add_function(&handle_response_3);
      break;
    case 4:
      handle_response_4(manager);
      break;
  }
}

void wait_for_user_input (unsigned i, AsyncManager* const manager) {
  unsigned user_input;
  std::cout << "[0] Ver el estado de la operación en segundo plano" << "\n"
            << "[1] Realizar algoritmo 'costoso', A+B"<< "\n"
            << "[2] Realizar algoritmo 'costoso', solo B"<< "\n"
            << "[3] Realizar 10 instancias del algoritmo 'costoso' en paralelo"<< "\n"
            << "[4] Salir"<< "\n";
  std::cin >> user_input;
  master_handler (user_input, manager);
}

void do_stuff_in_background (unsigned i, AsyncManager* const manager) {
  counter++;
}

void generate_logs (unsigned, AsyncManager* const manager) {
  std::ofstream outfile;
  outfile.open("test.txt", std::ios_base::app);
  outfile << "log : " << counter << "\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main() {
  AsyncManager manager;
  counter = 0;

  manager.add_persistent_function(&wait_for_user_input);
  manager.add_persistent_function(&do_stuff_in_background);
  manager.add_persistent_function(&generate_logs);
  manager.execute();
}


