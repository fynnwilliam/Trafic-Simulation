#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

class Vehicle;

enum class TrafficLightPhase { red, green };

template <class T> class MessageQueue {
public:
  void send(T &&);
  T receive();

private:
  std::deque<T> _queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

class TrafficLight : public TrafficObject {
public:
  TrafficLightPhase getCurrentPhase() const noexcept { return _currentPhase; }
  void waitForGreen();
  void simulate();

private:
  void cycleThroughPhases();

  MessageQueue<TrafficLightPhase> _messages;
  std::condition_variable _condition;
  std::mutex _mutex;
  TrafficLightPhase _currentPhase = TrafficLightPhase::red;
};

#endif