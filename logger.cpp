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

#include "logger.h"

logger::logger (AsyncManager* const manager) :
  asyncObject(manager),
  log (false)
  {}

void logger::on_start_logging() {
  if (!log) {
    log = true;
    do {
      std::ofstream outfile;
      outfile.open("test.txt", std::ios_base::app);
      outfile << "log : " << "log writen" << "\n";
      outfile.close();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (log);
  }
}

void logger::on_stop_logging() {
  if (log) {
    log = false;
    std::ofstream outfile;
    outfile.open("test.txt", std::ios_base::app);
    outfile << "> stop log" << "\n";
    outfile.close();
  }
}

