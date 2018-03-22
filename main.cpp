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
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    });

    // Ejecutador de las funciones encoladas
    std::async([&](){
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
};

class asyncObject {
protected:
  AsyncManager* manager;
public:
  asyncObject (AsyncManager* const mngr) :
    manager (mngr)
  {}
  asyncObject() {};
  void connect (std::function<void (AsyncManager* const)> slot) {
    manager->add_function(slot);
  }

};

class myObject : public asyncObject {
public:
  myObject (AsyncManager* const mngr) : asyncObject(mngr) {}

// SLOTS -------------------------------------------------
  void slot (int i, int j) {
    std::cout << "He sido llamado con los números " << i << " " << j << std::endl;
  };

// SIGNALS ------------------------------------------------
  std::function<void (unsigned, unsigned)> signal;
};


void handle_response_0 (AsyncManager* const manager) {
  std::cout << "\t" << "El contador en segundo plano marca " << counter << "\n" << std::flush;
}

void handle_response_2 (AsyncManager* const manager) {
  std::cout << "\t" << "Comenzando B " << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "B finalizado " << "\n" << std::flush;
}

void handle_response_1 (AsyncManager* const manager) {
  std::cout << "\t" << "Comenzando A " << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "A finalizado " << "\n" << std::flush;
  handle_response_2(manager);
}

void handle_response_3 (AsyncManager* const manager) {
  std::cout << "Inicio del bloque paralelo" << std::endl;
  std::vector <std::future<void>> promises (10);
  for (auto& promise : promises)
    promise = std::async(handle_response_1, manager);

  for (auto& promise : promises)
    promise.get();

  std::cout << "Fin del bloque paralelo" << std::endl;
}

auto handle_response_4 (int m, int l) {
  return [&](AsyncManager* const manager){
    manager->end_process();
  };
}

void master_handler (AsyncManager* const manager, unsigned option) {
  switch (option) {
    case 0:
      manager->add_function(handle_response_0);
      break;
    case 1:
      manager->add_function(handle_response_1);
      break;
    case 2:
      manager->add_function(handle_response_2);
      break;
    case 3:
      manager->add_function(handle_response_3);
      break;
    case 4:
      manager->add_function(handle_response_4(1, 2));
      break;
  }
}

void wait_for_user_input (AsyncManager* const manager) {
  unsigned user_input;
  std::cout << "[0] Ver el estado de la operación en segundo plano" << "\n"
            << "[1] Realizar algoritmo 'costoso', A+B"<< "\n"
            << "[2] Realizar algoritmo 'costoso', solo B"<< "\n"
            << "[3] Realizar 10 instancias del algoritmo 'costoso' en paralelo"<< "\n"
            << "[4] Salir"<< "\n";
  std::cin >> user_input;
  master_handler (manager, user_input);
}

void do_stuff_in_background (AsyncManager* const manager) {
  counter++;
  std::this_thread::sleep_for(std::chrono::microseconds(10));
}

void generate_logs (AsyncManager* const manager) {
  std::ofstream outfile;
  outfile.open("test.txt", std::ios_base::app);
  outfile << "log : " << counter << "\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void aa (unsigned i, unsigned j) {
  std::cout << "a " << i << " " << j << std::endl;
}

int main() {
  AsyncManager manager;

  myObject a (&manager);
  myObject b (&manager);

  a.signal = [&](unsigned i, unsigned j) {
    a.connect(std::bind (&myObject::slot, b, i, j));
  };

  a.signal(40, 20);
  a.signal(40, 9);
  a.signal(90, 10);
  a.signal(4, 20);

  manager.add_persistent_function(wait_for_user_input);
  manager.add_persistent_function(do_stuff_in_background);
  manager.add_persistent_function(generate_logs);
  manager.execute();
}


