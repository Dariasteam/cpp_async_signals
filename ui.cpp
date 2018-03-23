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

long long unsigned C::counter = 0;

UI::UI (AsyncManager* manager) :
  asyncObject(manager),
  a (manager),
  b (manager)
  {
  // CONNECTS --------------------------------------------------------------
  signal_0 = [&]() {
    send_signal(std::bind(&UI::on_handle_response_0, this));
  };
  signal_1 = [&]() {
    send_signal(std::bind(&UI::on_handle_response_1, this));
  };
  signal_2 = [&]() {
    send_signal(std::bind(&UI::on_handle_response_2, this));
  };
  signal_3 = [&](unsigned n) {
    send_signal(std::bind(&UI::on_handle_response_3, this, n));
  };
}

void UI::wait_for_user_input() {
  manager->add_function([&](AsyncManager const *){
    unsigned user_input = 0;
    while (user_input != 7) {
      std::cout << "[0] Ver el estado de la operación en segundo plano" << "\n"
      << "[1] Realizar algoritmo 'costoso' Alg2"<< "\n"
      << "[2] Realizar algoritmo 'costoso', Alg1 + Alg2"<< "\n"
      << "[3] Realizar 10 instancias del algoritmo 'costoso' en paralelo"<< "\n"
      << "[4] Conectar con señales los objetos a y b"<< "\n"
      << "[5] Desconectar los objetos a y b"<< "\n"
      << "[6] Emitir la señal en a"<< "\n"
      << "[7] Salir"<< "\n";
      std::cin >> user_input;
      master_handler (user_input);
    }
  });
}

void UI::master_handler(unsigned option) {
  switch (option) {
    case 0:
      signal_0();
      break;
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
      manager->add_function([&](AsyncManager* const manager) { a.signal(40, 20); });
      break;
    case 7:
      manager->add_function(std::bind(&AsyncManager::end_process, manager));
      break;
  }
}

void UI::on_handle_response_0() {
  std::cout << "\t" << "El contador en segundo plano marca " << C::counter << "\n" << std::flush;
}

void UI::on_handle_response_1() {
  std::cout << "\t" << "Comenzando B " << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "\t" << "B finalizado " << "\n" << std::flush;
}

void UI::on_handle_response_2() {
  std::cout << "\t" << "Comenzando A " << "\n" << std::flush;
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "\t" << "A finalizado " << "\n" << std::flush;
  on_handle_response_1();
}

void UI::on_handle_response_3(unsigned n) {
  std::cout << "Inicio del bloque paralelo" << std::endl;
  std::vector <std::future<void>> promises (n);
  for (auto& promise : promises)
    promise = std::async(&UI::on_handle_response_1, this);

  for (auto& promise : promises)
    promise.get();

  std::cout << "Fin del bloque paralelo" << std::endl;
}

void myObject::slot (int i, int j) {
  std::cout << "He sido llamado con los números " << i << " " << j << std::endl;
};
