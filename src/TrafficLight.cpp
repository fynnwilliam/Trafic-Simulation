#include "TrafficLight.h"
#include <chrono>
#include <iostream>
#include <random>

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock lock{_mutex};
  _condition.wait(lock, [&]() { return _queue.size() > 0; });

  T message = std::move(_queue.front());
  _queue.clear();
  return message;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard lock{_mutex};
  _queue.emplace_back(std::move(msg));
  _condition.notify_one();
}

void TrafficLight::waitForGreen() {
  while (_messages.receive() == TrafficLightPhase::red ||
         _currentPhase == TrafficLightPhase::red)
    ; // continue until green...
}

void TrafficLight::simulate() {
  std::lock_guard lock{_mtx};
  threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
  using namespace std::chrono;

  std::mt19937 gen{std::random_device{}()};
  std::uniform_real_distribution<> dist{4.01, 6.01};

  while (auto real = dist(gen)) {
    const auto duration = milliseconds{static_cast<unsigned>(real * 1'000)};
    std::this_thread::sleep_for(duration);

    std::unique_lock lock{_mutex};
    _currentPhase = _currentPhase == TrafficLightPhase::red
                        ? TrafficLightPhase::green
                        : TrafficLightPhase::red;
    lock.unlock();
    _messages.send(TrafficLightPhase{_currentPhase});
  }
}