#pragma once

#include "models.h"
#include "database.h"
#include "account_manager.h"
#include "order_manager.h"
#include <memory>
#include <vector>

namespace alphapi {

class TradeExecutor {
 public:
  TradeExecutor(std::shared_ptr<Database> db,
                std::shared_ptr<AccountManager> acc_mgr,
                std::shared_ptr<OrderManager> order_mgr)
      : db_(db), account_manager_(acc_mgr), order_manager_(order_mgr) {}
  ~TradeExecutor() = default;

  // Execute a single trade
  bool execute_trade(std::shared_ptr<Trade> trade);

  // Execute multiple trades from order matching
  bool execute_trades(const std::vector<std::shared_ptr<Trade>>& trades,
                      std::shared_ptr<Order> original_order);

  // Get trade details
  std::shared_ptr<Trade> get_trade(const std::string& trade_id);

  // Get all trades
  std::vector<std::shared_ptr<Trade>> get_all_trades();

 private:
  std::shared_ptr<Database> db_;
  std::shared_ptr<AccountManager> account_manager_;
  std::shared_ptr<OrderManager> order_manager_;

  // Helper methods
  bool update_buyer_account(const std::string& buyer_id, int quantity,
                            double price);
  bool update_seller_account(const std::string& seller_id,
                             const std::string& symbol, int quantity,
                             double price);
};

} // namespace alphapi
