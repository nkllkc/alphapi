#include "trading_system.h"
#include <iostream>

namespace alphapi {

TradingSystem::TradingSystem() {
  db_ = std::make_shared<Database>();
  price_feed_ = std::make_shared<PriceFeed>();
  order_manager_ = std::make_shared<OrderManager>(db_);
  account_manager_ = std::make_shared<AccountManager>(db_);
  matching_engine_ = std::make_shared<MatchingEngine>();
  executor_ =
      std::make_shared<TradeExecutor>(db_, account_manager_, order_manager_);
}

std::string TradingSystem::get_next_user_id() {
  return "user_" + std::to_string(user_counter_++);
}

std::shared_ptr<User> TradingSystem::create_user(const std::string& name,
                                                 const std::string& email) {
  auto user = std::make_shared<User>(get_next_user_id(), name, email);
  db_->save_user(*user);
  return user;
}

std::shared_ptr<User> TradingSystem::get_user(const std::string& user_id) {
  return db_->get_user(user_id);
}

std::shared_ptr<Account> TradingSystem::create_account(
    const std::string& user_id, double initial_cash) {
  return account_manager_->create_account(user_id, initial_cash);
}

std::shared_ptr<Account> TradingSystem::get_account_by_user(
    const std::string& user_id) {
  return account_manager_->get_account_by_user(user_id);
}

double TradingSystem::get_balance(const std::string& user_id) {
  auto account = get_account_by_user(user_id);
  if (account) {
    return account->cash_balance;
  }
  return 0;
}

void TradingSystem::set_quote(const Quote& quote) {
  price_feed_->set_quote(quote);
}

std::shared_ptr<Quote> TradingSystem::get_market_data(
    const std::string& symbol) {
  return price_feed_->get_latest_quote(symbol);
}

void TradingSystem::load_prices_from_csv(const std::string& filename) {
  price_feed_->load_from_csv(filename);
}

OrderConfirmation TradingSystem::place_order(const std::string& user_id,
                                             const std::string& symbol,
                                             OrderSide side, OrderType type,
                                             double price, int quantity) {
  OrderConfirmation confirmation;

  // Validate user exists
  auto user = get_user(user_id);
  if (!user) {
    confirmation.success = false;
    confirmation.error_message = "User not found";
    return confirmation;
  }

  // Validate account exists
  auto account = get_account_by_user(user_id);
  if (!account) {
    confirmation.success = false;
    confirmation.error_message = "Account not found";
    return confirmation;
  }

  // Validate sufficient funds/shares
  if (side == OrderSide::BUY) {
    if (!account_manager_->validate_buy_order(user_id, price, quantity)) {
      confirmation.success = false;
      confirmation.error_message = "Insufficient funds";
      return confirmation;
    }
  } else {
    if (!account_manager_->validate_sell_order(user_id, symbol, quantity)) {
      confirmation.success = false;
      confirmation.error_message = "Insufficient shares";
      return confirmation;
    }
  }

  // Create order
  auto order = order_manager_->create_order(user_id, symbol, side, type,
                                             price, quantity);

  // Submit to matching engine
  auto execution = matching_engine_->submit_order(order);

  if (!execution.success) {
    confirmation.success = false;
    confirmation.error_message = execution.error_message;
    return confirmation;
  }

  // Execute trades
  executor_->execute_trades(execution.trades, order);

  // Prepare confirmation
  confirmation.order_id = order->id;
  confirmation.status = order->status;
  confirmation.filled_quantity = order->filled_quantity;
  confirmation.trades = execution.trades;
  confirmation.updated_balance = account->cash_balance;
  confirmation.success = true;

  return confirmation;
}

bool TradingSystem::cancel_order(const std::string& user_id,
                                  const std::string& order_id) {
  auto order = get_order(order_id);
  if (!order || order->user_id != user_id) {
    return false;
  }

  if (order->status == OrderStatus::FILLED) {
    return false;
  }

  matching_engine_->cancel_order(order->symbol, order_id);
  order_manager_->cancel_order(order_id);
  return true;
}

std::shared_ptr<Order> TradingSystem::get_order(const std::string& order_id) {
  return order_manager_->get_order(order_id);
}

std::vector<std::shared_ptr<Order>> TradingSystem::get_user_orders(
    const std::string& user_id) {
  return order_manager_->get_orders_by_user(user_id);
}

std::vector<std::shared_ptr<Position>> TradingSystem::get_positions(
    const std::string& user_id) {
  auto account = get_account_by_user(user_id);
  if (!account) {
    return std::vector<std::shared_ptr<Position>>();
  }
  return account_manager_->get_all_positions(account->id);
}

std::vector<std::shared_ptr<Trade>> TradingSystem::get_trades(
    const std::string& user_id) {
  return db_->get_trades_by_user(user_id);
}

std::vector<std::shared_ptr<Trade>> TradingSystem::get_all_trades() {
  return executor_->get_all_trades();
}

TradingSystem::AccountSummary TradingSystem::get_account_summary(
    const std::string& user_id) {
  AccountSummary summary;
  summary.user_id = user_id;
  summary.total_value = 0;

  auto account = get_account_by_user(user_id);
  if (account) {
    summary.cash_balance = account->cash_balance;
    summary.total_value += summary.cash_balance;
  }

  auto positions = get_positions(user_id);
  for (const auto& pos : positions) {
    summary.positions.push_back(pos);
    auto quote = get_market_data(pos->symbol);
    if (quote) {
      double mid_price = (quote->bid + quote->ask) / 2.0;
      summary.total_value += pos->quantity * mid_price;
    }
  }

  return summary;
}

std::shared_ptr<OrderBook> TradingSystem::get_order_book(
    const std::string& symbol) {
  return matching_engine_->get_order_book(symbol);
}

} // namespace alphapi
