#include <iostream>
#include <future>
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <queue>


using namespace std;

bool end_program;
long long unsigned counter;
std::queue<std::function<void (unsigned)>> asynchronous_functions;
unsigned id;
std::queue<future<void>> promises;

// Encargado de recoger las funciones que deben realizarse y alimentar al dispatcher
void asynchronous_handler () {
  while (!end_program) {
    while (!asynchronous_functions.empty()) {
      promises.push (std::async(asynchronous_functions.front(), id));
      asynchronous_functions.pop();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

// Encargado de realizar las funciones de forma asíncrona
void asynchronous_dispatcher () {
  unsigned last_index = 0;
  while (!end_program) {
    while (!promises.empty()) {
      promises.front().get();
      promises.pop();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void handle_response_0 (unsigned) {
  std::cout << "\t" << "El contador en segundo plano marca " << counter << "\n" << std::flush;
}

void handle_response_2 (unsigned i) {
  std::cout << "\t" << "Comenzando B " << i << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "B finalizado " << i << "\n" << std::flush;
}

void handle_response_1 (unsigned i) {
  std::cout << "\t" << "Comenzando A " << i << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cout << "\t" << "A finalizado " << i << "\n" << std::flush;
  handle_response_2(i);
}

void handle_response_3 (unsigned i) {
  std::vector <std::future<void>> promises (10);
  for (auto& promise : promises)
    promise = std::async(handle_response_1, i);

  for (auto& promise : promises)
    promise.get();
}

void handle_response_4 (unsigned) {
  end_program = true;
}

void master_handler (unsigned option) {
  switch (option) {
    case 0:
      asynchronous_functions.push(&handle_response_0);
      break;
    case 1:
      asynchronous_functions.push(&handle_response_1);
      break;
    case 2:
      asynchronous_functions.push(&handle_response_2);
      break;
    case 3:
      asynchronous_functions.push(&handle_response_3);
      break;
    case 4:
      asynchronous_functions.push(&handle_response_4);
      break;
  }
}

void wait_for_user_input () {
  unsigned user_input;
  while (!end_program) {
    std::cout << "[0] Ver el estado de la operación en segundo plano" << "\n"
              << "[1] Realizar algoritmo 'costoso', A+B"<< "\n"
              << "[2] Realizar algoritmo 'costoso', solo B"<< "\n"
              << "[3] Realizar 10 instancias del algoritmo 'costoso' en paralelo"<< "\n"
              << "[4] Salir"<< "\n";
    std::cin >> user_input;
    master_handler (user_input);
    id++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  std::cout << "La entrada de usuario ha terminado\n" << std::flush;
}

void do_stuff_in_background () {
  auto f = [&](unsigned i) {
    while (!end_program) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      counter++;
    }
    std::cout << "La operación en segundo plano ha terminado\n" << std::flush;
  };
  asynchronous_functions.push(f);
}

int main() {
  end_program = false;
  counter = 0;
  id = 0;

  std::vector<std::future<void>> promises (3);

  promises[0] = std::async(wait_for_user_input);
  promises[1] = std::async(asynchronous_dispatcher);
  promises[2] = std::async(asynchronous_handler);

  do_stuff_in_background();

  for (auto& p : promises) {
    p.wait();
  }

  std::cout << "Programa finalizado" << std::endl;
}


