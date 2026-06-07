#pragma once

#include "models.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <deque>

namespace alphapi {

class PriceFeed {
 public:
  PriceFeed() = default;
  ~PriceFeed() = default;

  // Add or update a quote
  void set_quote(const Quote& quote);

  // Get current quote
  std::shared_ptr<Quote> get_latest_quote(const std::string& symbol);

  // Get all quotes for a symbol
  std::vector<std::shared_ptr<Quote>> get_quote_history(
      const std::string& symbol);

  // Check if symbol has quotes
  bool has_symbol(const std::string& symbol) const;

  // Get all available symbols
  std::vector<std::string> get_symbols() const;

  // Simulate price movement (for testing)
  void simulate_price_update(const std::string& symbol, double change);

  // Load prices from CSV file
  void load_from_csv(const std::string& filename);

 private:
  // Current quotes
  std::unordered_map<std::string, std::shared_ptr<Quote>> current_quotes_;

  // Historical quotes per symbol
  std::unordered_map<std::string, std::deque<std::shared_ptr<Quote>>>
      price_history_;

  static const int HISTORY_LIMIT = 1000; // Keep last 1000 quotes per symbol
};

} // namespace alphapi
