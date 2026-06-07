#include "executor.h"

namespace alphapi {

bool TradeExecutor::execute_trade(std::shared_ptr<Trade> trade) {
  // Save trade to database
  db_->save_trade(*trade);

  // Update buyer account (add shares, debit cash)
  if (!update_buyer_account(trade->buyer_id, trade->quantity,
                            trade->price)) {
    return false;
  }

  // Update seller account (remove shares, credit cash)
  if (!update_seller_account(trade->seller_id, trade->symbol,
                             trade->quantity, trade->price)) {
    return false;
  }

  return true;
}

bool TradeExecutor::execute_trades(
    const std::vector<std::shared_ptr<Trade>>& trades,
    std::shared_ptr<Order> original_order) {
  bool all_success = true;

  for (auto& trade : trades) {
    if (!execute_trade(trade)) {
      all_success = false;
    }
  }

  // Update order status
  if (!trades.empty()) {
    int total_filled = 0;
    for (const auto& trade : trades) {
      total_filled += trade->quantity;
    }

    if (total_filled == original_order->quantity) {
      order_manager_->fill_order(original_order->id, total_filled);
    } else if (total_filled > 0) {
      order_manager_->partially_fill_order(original_order->id, total_filled);
    }
  }

  return all_success;
}

std::shared_ptr<Trade> TradeExecutor::get_trade(const std::string& trade_id) {
  return db_->get_trade(trade_id);
}

std::vector<std::shared_ptr<Trade>> TradeExecutor::get_all_trades() {
  return db_->get_all_trades();
}

bool TradeExecutor::update_buyer_account(const std::string& buyer_id,
                                         int quantity, double price) {
  auto buyer = db_->get_user(buyer_id);
  if (!buyer) {
    return false;
  }

  auto account = account_manager_->get_account_by_user(buyer_id);
  if (!account) {
    return false;
  }

  // Debit cash (amount already reserved during validation)
  double cash_spent = quantity * price;
  account_manager_->debit_cash(account->id, cash_spent);

  // Add shares to position
  account_manager_->add_shares(account->id, "UNKNOWN", quantity, price);

  return true;
}

bool TradeExecutor::update_seller_account(const std::string& seller_id,
                                          const std::string& symbol,
                                          int quantity, double price) {
  auto seller = db_->get_user(seller_id);
  if (!seller) {
    return false;
  }

  auto account = account_manager_->get_account_by_user(seller_id);
  if (!account) {
    return false;
  }

  // Remove shares from position
  account_manager_->remove_shares(account->id, symbol, quantity);

  // Credit cash
  double cash_received = quantity * price;
  account_manager_->credit_cash(account->id, cash_received);

  return true;
}

} // namespace alphapi
