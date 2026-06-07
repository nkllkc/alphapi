#include "price_feed.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

namespace alphapi {

void PriceFeed::set_quote(const Quote& quote) {
  current_quotes_[quote.symbol] = std::make_shared<Quote>(quote);

  // Add to history
  auto& history = price_history_[quote.symbol];
  history.push_back(std::make_shared<Quote>(quote));

  // Keep only last HISTORY_LIMIT quotes
  if (history.size() > HISTORY_LIMIT) {
    history.pop_front();
  }
}

std::shared_ptr<Quote> PriceFeed::get_latest_quote(const std::string& symbol) {
  auto it = current_quotes_.find(symbol);
  if (it != current_quotes_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<Quote>> PriceFeed::get_quote_history(
    const std::string& symbol) {
  auto it = price_history_.find(symbol);
  if (it != price_history_.end()) {
    std::vector<std::shared_ptr<Quote>> result(it->second.begin(),
                                                it->second.end());
    return result;
  }
  return std::vector<std::shared_ptr<Quote>>();
}

bool PriceFeed::has_symbol(const std::string& symbol) const {
  return current_quotes_.find(symbol) != current_quotes_.end();
}

std::vector<std::string> PriceFeed::get_symbols() const {
  std::vector<std::string> symbols;
  for (const auto& pair : current_quotes_) {
    symbols.push_back(pair.first);
  }
  return symbols;
}

void PriceFeed::simulate_price_update(const std::string& symbol,
                                      double change) {
  auto quote = get_latest_quote(symbol);
  if (quote) {
    double new_mid = (quote->bid + quote->ask) / 2.0 + change;
    double spread = quote->ask - quote->bid;
    Quote new_quote(symbol, new_mid - spread / 2, new_mid + spread / 2);
    set_quote(new_quote);
  }
}

void PriceFeed::load_from_csv(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return; // File not found, skip
  }

  std::string line;
  // Skip header if present
  std::getline(file, line);

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string symbol;
    double bid, ask;
    char comma;

    if (ss >> symbol >> comma >> bid >> comma >> ask) {
      set_quote(Quote(symbol, bid, ask));
    }
  }

  file.close();
}

} // namespace alphapi
