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
  asyncObject() {};
  void connect (std::function<void (AsyncManager* const)> slot) {
    manager->add_function(slot);
  }
};

class myObject : public asyncObject {
private:
  std::string name;
public:
  myObject (AsyncManager* const mngr, std::string nm = "default_name") :
    asyncObject(mngr),
    name (nm)
    {}

// SLOTS -------------------------------------------------
  void slot (int i, int j) {
    std::cout << "He sido llamado con los números " << i << " " << j << std::endl;
    std::cout << "Soy " << name << std::endl;
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

void master_handler (AsyncManager* const manager, myObject& a, myObject& b, unsigned option) {
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
      manager->add_function([&](AsyncManager* const manager){
        a.signal = [&](unsigned i, unsigned j) {
          a.connect(std::bind (&myObject::slot, b, i, j));
        };
      });
      break;
    case 5:
      manager->add_function([&](AsyncManager* const manager){
        a.signal = [&](unsigned i, unsigned j) {};
      });
      break;
    case 6:
      manager->add_function([&](AsyncManager* const manager){ a.signal(40, 20); });
      break;
    case 7:
      manager->add_function(std::bind(&AsyncManager::end_process, manager));
      break;

  }
}

void wait_for_user_input (AsyncManager* const manager) {
  myObject a (manager, "objeto a");
  myObject b (manager, "objeto b");

  unsigned user_input = 0;
    while (user_input != 7) {
    std::cout << "[0] Ver el estado de la operación en segundo plano" << "\n"
              << "[1] Realizar algoritmo 'costoso', A+B"<< "\n"
              << "[2] Realizar algoritmo 'costoso', solo B"<< "\n"
              << "[3] Realizar 10 instancias del algoritmo 'costoso' en paralelo"<< "\n"
              << "[4] Conectar con señales los objetos a y b"<< "\n"
              << "[5] Desconectar los objetos a y b"<< "\n"
              << "[6] Emitir la señal en a"<< "\n"
              << "[7] Salir"<< "\n";
    std::cin >> user_input;
    master_handler (manager, a, b, user_input);
  }
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

int main() {
  AsyncManager manager;

  manager.add_persistent_function(do_stuff_in_background);
  manager.add_persistent_function(generate_logs);
  manager.add_function(wait_for_user_input);
  manager.execute();
}


