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
 * MTQueue.h
 *
 *  Created on: Oct 13, 2014
 *      Author: Kamil Saigol
 */
/**
 * @class MTQueue
 * A multiple-producer, multiple-consumer, thread-safe queue implemented using SystemC primitives
 * The MTQueue is used as the input buffer in the NPU modules
 */
#ifndef CORE_MTQUEUE_H_
#define CORE_MTQUEUE_H_

#include <queue>
#include "systemc.h"

template <typename T>
class MTQueue {
 public:
  /**
   * Construct a MTQueue
   */
  MTQueue() : /*sem_(1),*/ queue_(),
            mutex_(sc_gen_unique_name("mutex_")),
            cond_(sc_gen_unique_name("cond_")) {
  }
  /**
   * Pop the top element from the MTQueue and return a copy
   * @return  Copy of the top element
   */
  T pop() {
    while (queue_.empty()) {
      wait(cond_);
    }
    mutex_.lock();
    // sem_.wait();
    auto item = queue_.front();
    queue_.pop();
    mutex_.unlock();
    // sem_.post();
    return item;
  }

  /**
   * Pop the top element from the MTQueue and copy it into the output argument
   * @param  Output argument
   */
  void pop(T& item) {
    while (queue_.empty()) {
      wait(cond_);
    }
    mutex_.lock();
    // sem_.wait();
    item = queue_.front();
    queue_.pop();
    mutex_.unlock();
    // sem_.post();
  }

  /**
   * Push an item onto the MTQueue
   * @param  Item to push
   */
  void push(const T& item) {
    mutex_.lock();
    // sem_.wait();
    queue_.push(item);
    // sem_.post();
    mutex_.unlock();
    cond_.notify();
  }

  /**
   * Push an item onto the MTQueue (explicit move)
   * @param  Item to move
   */
  void push(T&& item) {
    mutex_.lock();
    // sem_.wait();
    queue_.push(std::move(item));
    // sem_.post();
    mutex_.unlock();
    cond_.notify();
  }
  /*
   * Return Size of Queue
   * @param none
   */
  void size(int& qsize) {
    mutex_.lock();
    qsize = queue_.size();
    mutex_.unlock();
  }

 private:
  std::queue<T> queue_;  /*!< Internal queue */
  sc_mutex mutex_;    /*!< Guard */
  sc_event cond_;      /*!< Event to notify when not empty */
  // sc_semaphore sem_;
};

#endif  // CORE_MTQUEUE_H_
