#pragma once

#include "models.h"
#include <memory>
#include <vector>
#include <deque>
#include <unordered_map>

namespace alphapi {

struct TradeExecution {
  bool success;
  std::vector<std::shared_ptr<Trade>> trades;
  std::string error_message;
};

class OrderBook {
 public:
  OrderBook() = default;
  ~OrderBook() = default;

  // Add order to book
  void add_order(std::shared_ptr<Order> order);

  // Remove order from book
  void remove_order(const std::string& order_id);

  // Get best bid/ask prices
  double get_best_bid() const;
  double get_best_ask() const;

  // Get all orders at specific price level
  std::vector<std::shared_ptr<Order>> get_orders_at_price(double price,
                                                          OrderSide side);

  // Get all pending buy/sell orders
  std::vector<std::shared_ptr<Order>> get_buy_orders();
  std::vector<std::shared_ptr<Order>> get_sell_orders();

  // Get order by ID
  std::shared_ptr<Order> get_order(const std::string& order_id);

 private:
  // Buy orders: ordered by price (highest first), then by time (earliest first)
  std::unordered_map<std::string, std::shared_ptr<Order>> buy_orders_;
  std::unordered_map<std::string, std::shared_ptr<Order>> sell_orders_;

  // Price levels for quick lookup
  std::unordered_map<double, std::deque<std::shared_ptr<Order>>> buy_levels_;
  std::unordered_map<double, std::deque<std::shared_ptr<Order>>> sell_levels_;
};

class MatchingEngine {
 public:
  MatchingEngine() = default;
  ~MatchingEngine() = default;

  // Submit order for matching
  TradeExecution submit_order(std::shared_ptr<Order> order);

  // Cancel order from book
  void cancel_order(const std::string& symbol, const std::string& order_id);

  // Get order book for symbol
  std::shared_ptr<OrderBook> get_order_book(const std::string& symbol);

  // Get all order books
  std::unordered_map<std::string, std::shared_ptr<OrderBook>>
  get_all_order_books();

 private:
  // Order books per symbol
  std::unordered_map<std::string, std::shared_ptr<OrderBook>> books_;

  int trade_counter_ = 1;

  // Matching logic
  TradeExecution match_buy_order(std::shared_ptr<Order> order,
                                  const std::string& symbol);
  TradeExecution match_sell_order(std::shared_ptr<Order> order,
                                   const std::string& symbol);

  std::string get_next_trade_id();
};

} // namespace alphapi
