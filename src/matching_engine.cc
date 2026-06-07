#include "matching_engine.h"
#include <algorithm>
#include <cmath>

namespace alphapi {

// OrderBook implementation

void OrderBook::add_order(std::shared_ptr<Order> order) {
  if (order->side == OrderSide::BUY) {
    buy_orders_[order->id] = order;
    buy_levels_[order->price].push_back(order);
  } else {
    sell_orders_[order->id] = order;
    sell_levels_[order->price].push_back(order);
  }
}

void OrderBook::remove_order(const std::string& order_id) {
  // Try to remove from buy orders
  auto buy_it = buy_orders_.find(order_id);
  if (buy_it != buy_orders_.end()) {
    auto price = buy_it->second->price;
    buy_orders_.erase(buy_it);
    auto& level = buy_levels_[price];
    auto pos = std::find_if(level.begin(), level.end(),
                            [&order_id](const auto& o) {
                              return o->id == order_id;
                            });
    if (pos != level.end()) {
      level.erase(pos);
    }
    return;
  }

  // Try to remove from sell orders
  auto sell_it = sell_orders_.find(order_id);
  if (sell_it != sell_orders_.end()) {
    auto price = sell_it->second->price;
    sell_orders_.erase(sell_it);
    auto& level = sell_levels_[price];
    auto pos = std::find_if(level.begin(), level.end(),
                            [&order_id](const auto& o) {
                              return o->id == order_id;
                            });
    if (pos != level.end()) {
      level.erase(pos);
    }
  }
}

double OrderBook::get_best_bid() const {
  if (buy_orders_.empty()) {
    return 0;
  }
  double best = 0;
  for (const auto& pair : buy_orders_) {
    if (pair.second->price > best) {
      best = pair.second->price;
    }
  }
  return best;
}

double OrderBook::get_best_ask() const {
  if (sell_orders_.empty()) {
    return 0;
  }
  double best = std::numeric_limits<double>::max();
  for (const auto& pair : sell_orders_) {
    if (pair.second->price < best) {
      best = pair.second->price;
    }
  }
  return best == std::numeric_limits<double>::max() ? 0 : best;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_orders_at_price(
    double price, OrderSide side) {
  if (side == OrderSide::BUY) {
    auto it = buy_levels_.find(price);
    if (it != buy_levels_.end()) {
      return std::vector<std::shared_ptr<Order>>(it->second.begin(),
                                                  it->second.end());
    }
  } else {
    auto it = sell_levels_.find(price);
    if (it != sell_levels_.end()) {
      return std::vector<std::shared_ptr<Order>>(it->second.begin(),
                                                  it->second.end());
    }
  }
  return std::vector<std::shared_ptr<Order>>();
}

std::vector<std::shared_ptr<Order>> OrderBook::get_buy_orders() {
  std::vector<std::shared_ptr<Order>> result;
  for (const auto& pair : buy_orders_) {
    result.push_back(pair.second);
  }
  return result;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_sell_orders() {
  std::vector<std::shared_ptr<Order>> result;
  for (const auto& pair : sell_orders_) {
    result.push_back(pair.second);
  }
  return result;
}

std::shared_ptr<Order> OrderBook::get_order(const std::string& order_id) {
  auto buy_it = buy_orders_.find(order_id);
  if (buy_it != buy_orders_.end()) {
    return buy_it->second;
  }
  auto sell_it = sell_orders_.find(order_id);
  if (sell_it != sell_orders_.end()) {
    return sell_it->second;
  }
  return nullptr;
}

// MatchingEngine implementation

std::string MatchingEngine::get_next_trade_id() {
  return "trade_" + std::to_string(trade_counter_++);
}

std::shared_ptr<OrderBook> MatchingEngine::get_order_book(
    const std::string& symbol) {
  auto it = books_.find(symbol);
  if (it == books_.end()) {
    books_[symbol] = std::make_shared<OrderBook>();
  }
  return books_[symbol];
}

std::unordered_map<std::string, std::shared_ptr<OrderBook>>
MatchingEngine::get_all_order_books() {
  return books_;
}

void MatchingEngine::cancel_order(const std::string& symbol,
                                   const std::string& order_id) {
  auto book = get_order_book(symbol);
  book->remove_order(order_id);
}

TradeExecution MatchingEngine::match_buy_order(
    std::shared_ptr<Order> order, const std::string& symbol) {
  TradeExecution execution;
  execution.success = true;

  auto book = get_order_book(symbol);
  int remaining = order->quantity;

  // Get all sell orders sorted by price (lowest first), then by time
  auto sell_orders = book->get_sell_orders();
  std::sort(sell_orders.begin(), sell_orders.end(),
            [](const auto& a, const auto& b) {
              if (std::abs(a->price - b->price) > 1e-9) {
                return a->price < b->price;
              }
              return a->created_at < b->created_at;
            });

  for (auto& sell_order : sell_orders) {
    if (remaining == 0)
      break;

    // Can match if buy price >= sell price
    if (order->price >= sell_order->price) {
      int match_qty = std::min(remaining, sell_order->quantity -
                                              sell_order->filled_quantity);

      if (match_qty > 0) {
        auto trade = std::make_shared<Trade>(
            get_next_trade_id(), order->user_id, sell_order->user_id, symbol,
            match_qty, sell_order->price);

        execution.trades.push_back(trade);

        remaining -= match_qty;
        order->filled_quantity += match_qty;
        sell_order->filled_quantity += match_qty;
      }
    }
  }

  // Add remaining to order book
  if (remaining > 0) {
    book->add_order(order);
  }

  return execution;
}

TradeExecution MatchingEngine::match_sell_order(
    std::shared_ptr<Order> order, const std::string& symbol) {
  TradeExecution execution;
  execution.success = true;

  auto book = get_order_book(symbol);
  int remaining = order->quantity;

  // Get all buy orders sorted by price (highest first), then by time
  auto buy_orders = book->get_buy_orders();
  std::sort(buy_orders.begin(), buy_orders.end(),
            [](const auto& a, const auto& b) {
              if (std::abs(a->price - b->price) > 1e-9) {
                return a->price > b->price;
              }
              return a->created_at < b->created_at;
            });

  for (auto& buy_order : buy_orders) {
    if (remaining == 0)
      break;

    // Can match if sell price <= buy price
    if (order->price <= buy_order->price) {
      int match_qty = std::min(remaining, buy_order->quantity -
                                              buy_order->filled_quantity);

      if (match_qty > 0) {
        auto trade = std::make_shared<Trade>(
            get_next_trade_id(), buy_order->user_id, order->user_id, symbol,
            match_qty, buy_order->price);

        execution.trades.push_back(trade);

        remaining -= match_qty;
        order->filled_quantity += match_qty;
        buy_order->filled_quantity += match_qty;
      }
    }
  }

  // Add remaining to order book
  if (remaining > 0) {
    book->add_order(order);
  }

  return execution;
}

TradeExecution MatchingEngine::submit_order(std::shared_ptr<Order> order) {
  if (order->side == OrderSide::BUY) {
    return match_buy_order(order, order->symbol);
  } else {
    return match_sell_order(order, order->symbol);
  }
}

} // namespace alphapi
