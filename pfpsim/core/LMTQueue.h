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

/*
 * LMTQueue.h
 *
 *  Created on: Oct 13, 2014
 *      Author: Kamil Saigol
 */
/**
 * @class LMTQueue
 * A size-limited multiple-producer, multiple-consumer, thread-safe queue implemented using SystemC primitives
 * The LMTQueue is used as the input buffer in the NPU modules
 */
#ifndef CORE_LMTQUEUE_H_
#define CORE_LMTQUEUE_H_

#include <queue>
#include "systemc.h"

template <typename T, size_t N>
class LMTQueue {
 public:
  /**
   * Construct a LMTQueue
   */
  LMTQueue()
  : queue_(), mutex_(sc_gen_unique_name("mutex_")),
  cond_not_empty_(sc_gen_unique_name("cond_not_empty_")), slots_(0) {
  }
  /**
   * Pop the top element from the LMTQueue and return a copy
   * @return  Copy of the top element
   */
  T pop() {
    while (queue_.empty()) {
      wait(cond_not_empty_);
    }
    mutex_.lock();
    auto item = queue_.front();
    queue_.pop();
    slots_--;
    mutex_.unlock();
    cond_not_full_.notify();
    return item;
  }

  /**
   * Pop the top element from the LMTQueue and copy it into the output argument
   * @param  Output argument
   */
  void pop(T& item) {
    while (queue_.empty()) {
      wait(cond_not_empty_);
    }
    mutex_.lock();
    item = queue_.front();
    queue_.pop();
    slots_--;
    mutex_.unlock();
    cond_not_full_.notify();
  }

  /**
   * Push an item onto the LMTQueue
   * @param  Item to push
   */
  void push(const T& item) {
    while (this->full()) {
      wait(cond_not_full_);
    }
    mutex_.lock();
    queue_.push(item);
    slots_++;
    mutex_.unlock();
    cond_not_empty_.notify();
  }

  /**
   * Push an item onto the LMTQueue (explicit move)
   * @param  Item to move
   */
  void push(T&& item) {
    while (this->full()) {
      wait(cond_not_full_);
    }
    mutex_.lock();
    queue_.push(std::move(item));
    slots_++;
    mutex_.unlock();
    cond_not_empty_.notify();
  }

  size_t available() const {
    return N - slots_;
  }

  bool full() const {
    return (slots_ == N);
  }

  bool empty() const {
    return queue_.empty();
  }

 private:
  std::queue<T> queue_;            /*!< Internal queue */
  sc_mutex mutex_;              /*!< Guard */
  //! Events to notify when not empty and not full
  sc_event cond_not_empty_, cond_not_full_;
  size_t slots_;                /*!< Number of queue positions used */
};

#endif  // CORE_LMTQUEUE_H_
