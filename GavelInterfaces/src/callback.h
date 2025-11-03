#ifndef GAVEL_CALLBACK_H
#define GAVEL_CALLBACK_H

#include <functional>
#include <iostream>
#include <vector>

class Callback {
public:
  // Add a free function or lambda
  void addCallback(std::function<void()> cb) { callbacks.push_back(std::move(cb)); }

  void trigger() const {
    for (auto& cb : callbacks) { cb(); }
  }

  void clearCallbacks() { callbacks.clear(); }
  size_t size() const { return callbacks.size(); }

private:
  std::vector<std::function<void()>> callbacks;
};

#endif