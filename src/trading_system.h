#pragma once

#include "models.h"
#include "database.h"
#include "price_feed.h"
#include "order_manager.h"
#include "account_manager.h"
#include "matching_engine.h"
#include "executor.h"
#include <memory>
#include <vector>

namespace alphapi {

class TradingSystem {
 public:
  TradingSystem();
  ~TradingSystem() = default;

  // User management
  std::shared_ptr<User> create_user(const std::string& name,
                                    const std::string& email);
  std::shared_ptr<User> get_user(const std::string& user_id);

  // Account management
  std::shared_ptr<Account> create_account(const std::string& user_id,
                                          double initial_cash);
  std::shared_ptr<Account> get_account_by_user(const std::string& user_id);
  double get_balance(const std::string& user_id);

  // Price feed
  void set_quote(const Quote& quote);
  std::shared_ptr<Quote> get_market_data(const std::string& symbol);
  void load_prices_from_csv(const std::string& filename);

  // Order placement
  OrderConfirmation place_order(const std::string& user_id,
                                const std::string& symbol, OrderSide side,
                                OrderType type, double price, int quantity);

  // Order cancellation
  bool cancel_order(const std::string& user_id, const std::string& order_id);

  // Order queries
  std::shared_ptr<Order> get_order(const std::string& order_id);
  std::vector<std::shared_ptr<Order>> get_user_orders(
      const std::string& user_id);

  // Position queries
  std::vector<std::shared_ptr<Position>> get_positions(
      const std::string& user_id);

  // Trade queries
  std::vector<std::shared_ptr<Trade>> get_trades(const std::string& user_id);
  std::vector<std::shared_ptr<Trade>> get_all_trades();

  // Account summary
  struct AccountSummary {
    std::string user_id;
    double cash_balance;
    std::vector<std::shared_ptr<Position>> positions;
    double total_value;
  };

  AccountSummary get_account_summary(const std::string& user_id);

  // Order book
  std::shared_ptr<OrderBook> get_order_book(const std::string& symbol);

 private:
  std::shared_ptr<Database> db_;
  std::shared_ptr<PriceFeed> price_feed_;
  std::shared_ptr<OrderManager> order_manager_;
  std::shared_ptr<AccountManager> account_manager_;
  std::shared_ptr<MatchingEngine> matching_engine_;
  std::shared_ptr<TradeExecutor> executor_;

  int user_counter_ = 1;

  std::string get_next_user_id();
};

} // namespace alphapi
