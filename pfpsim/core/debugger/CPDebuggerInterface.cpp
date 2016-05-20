/*
 * PFPSim: Library for the Programmable Forwarding Plane Simulation Framework
 *
 * Copyright (C) 2016 Concordia Univ., Montreal
 *     Samar Abdi
 *     Umair Aftab
 *     Gordon Bailey
 *     Faras Dewal
 *     Shafigh Parsazad
 *     Eric Tremblay
 *
 * Copyright (C) 2016 Ericsson
 *     Bochra Boughzala
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "CPDebuggerInterface.h"
#include <string>
#include <vector>
#include <map>

namespace pfp {
namespace core {
namespace db {

void CPDebuggerInterface::updateHandle(std::string table_name,
      std::string match_key, std::string action_name, uint64_t handle) {
  if (table_entries.find(table_name) != table_entries.end()) {
    if (table_entries[table_name].find(action_name)
          != table_entries[table_name].end()) {
      std::vector<TableEntry>& entries = table_entries[table_name][action_name];
      for (auto it = entries.begin(); it != entries.end(); it++) {
        if (it->match_key == match_key) {
          it->handle = handle;
          it->status = CPDebuggerInterface::TableEntryStatus::OK;
          break;
        }
      }
    }
  }
}

void CPDebuggerInterface::addTableEntry(std::string table_name,
      std::string match_key, std::string action_name,
      std::vector<std::string> action_data, uint64_t handle) {
  CPDebuggerInterface::TableEntry entry(table_name, match_key, action_name,
        action_data, handle, CPDebuggerInterface::TableEntryStatus::INSERTING);
  if (table_entries.find(table_name) == table_entries.end()) {
    std::vector<CPDebuggerInterface::TableEntry> entry_vector;
    entry_vector.push_back(entry);
    std::map<std::string,
          std::vector<CPDebuggerInterface::TableEntry>> action_map;
    action_map[action_name] = entry_vector;
    table_entries[table_name] = action_map;
  } else {
    std::map<std::string, std::vector<TableEntry>>& action_map_it
          = table_entries[table_name];
    if (action_map_it.find(action_name) == action_map_it.end()) {
      std::vector<CPDebuggerInterface::TableEntry> entry_vector;
      entry_vector.push_back(entry);
      action_map_it[action_name] = entry_vector;
    } else {
      std::vector<TableEntry>& entries_vector_it = action_map_it[action_name];
      entries_vector_it.push_back(entry);
    }
  }
}

void CPDebuggerInterface::printTableEntries() {
  std::cout << "Table Entries:" << std::endl;
  for (auto table = table_entries.begin();
        table != table_entries.end(); table++) {
    std::cout << table->first << ":" << std::endl;
    for (auto action = table->second.begin();
          action != table->second.end(); action++) {
      std::cout << "\t" << action->first << ":" << std::endl;
      for (auto entry = action->second.begin();
            entry != action->second.end(); entry++) {
        std::cout << "\t\t" << entry->match_key << " - "
              << entry->handle << std::endl;
        for (auto data = entry->action_data.begin();
              data != entry->action_data.end(); data++) {
          std::cout << "\t\t\t" << *data << std::endl;
        }
      }
    }
  }
}

void CPDebuggerInterface::updateTableEntry(std::string table_name,
        uint64_t handle, std::string action_name,
        std::vector<std::string> action_data) {
  std::vector<TableEntry>& entries = table_entries[table_name][action_name];
  for (auto it = entries.begin(); it != entries.end(); it++) {
    if (it->handle == handle) {
      it->action_data = action_data;
      it->status = CPDebuggerInterface::TableEntryStatus::MODIFYING;
      break;
    }
  }
}

void CPDebuggerInterface::deleteTableEntry(std::string table_name,
        uint64_t handle) {
  std::map<std::string, std::vector<TableEntry>>& action_map
        = table_entries[table_name];
  for (auto action = action_map.begin(); action != action_map.end(); action++) {
    for (auto entry = action->second.begin();
          entry != action->second.end(); entry++) {
      if (entry->handle == handle) {
        entry->status = CPDebuggerInterface::TableEntryStatus::DELETING;
        break;
      }
    }
  }
}

std::vector<CPDebuggerInterface::TableEntry>
CPDebuggerInterface::getAllTableEntries() {
  std::vector<TableEntry> entries;
  for (auto i = table_entries.begin(); i != table_entries.end(); i++) {
    for (auto j = i->second.begin(); j != i->second.end(); j++) {
      entries.insert(entries.end(), j->second.begin(), j->second.end());
    }
  }
  return entries;
}

void CPDebuggerInterface::confirmUpdateEntry(std::string table_name,
      uint64_t handle) {
  std::map<std::string, std::vector<TableEntry>>& action_map
        = table_entries[table_name];
  for (auto action = action_map.begin(); action != action_map.end(); action++) {
    for (auto entry = action->second.begin();
          entry != action->second.end(); entry++) {
      if (entry->handle == handle) {
        entry->status = CPDebuggerInterface::TableEntryStatus::OK;
        break;
      }
    }
  }
}

void CPDebuggerInterface::confirmDeleteEntry(std::string table_name,
      uint64_t handle) {
  std::map<std::string, std::vector<TableEntry>>& action_map
        = table_entries[table_name];
  for (auto action = action_map.begin(); action != action_map.end(); action++) {
    for (auto entry = action->second.begin();
          entry != action->second.end(); entry++) {
      if (entry->handle == handle) {
        action->second.erase(entry);
        if (action->second.size() == 0) {
          action_map.erase(action);
        }
        break;
      }
    }
  }
}

};  // namespace db
};  // namespace core
};  // namespace pfp
