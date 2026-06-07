#include "database.h"
#include <stdexcept>
#include <iostream>

namespace alphapi {

std::string Database::get_next_id(const std::string& prefix) {
  return prefix + "_" + std::to_string(next_id_counter_++);
}

// User operations
void Database::save_user(const User& user) {
  users_[user.id] = std::make_shared<User>(user);
}

std::shared_ptr<User> Database::get_user(const std::string& user_id) {
  auto it = users_.find(user_id);
  if (it != users_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<User>> Database::get_all_users() {
  std::vector<std::shared_ptr<User>> result;
  for (auto& pair : users_) {
    result.push_back(pair.second);
  }
  return result;
}

// Account operations
void Database::save_account(const Account& account) {
  accounts_[account.id] = std::make_shared<Account>(account);
}

std::shared_ptr<Account> Database::get_account(const std::string& account_id) {
  auto it = accounts_.find(account_id);
  if (it != accounts_.end()) {
    return it->second;
  }
  return nullptr;
}

std::shared_ptr<Account> Database::get_account_by_user(
    const std::string& user_id) {
  for (auto& pair : accounts_) {
    if (pair.second->user_id == user_id) {
      return pair.second;
    }
  }
  return nullptr;
}

void Database::update_account_balance(const std::string& account_id,
                                      double new_balance) {
  auto it = accounts_.find(account_id);
  if (it != accounts_.end()) {
    it->second->cash_balance = new_balance;
  }
}

// Position operations
void Database::save_position(const Position& position) {
  positions_[position.id] = std::make_shared<Position>(position);
}

std::shared_ptr<Position> Database::get_position(const std::string& position_id) {
  auto it = positions_.find(position_id);
  if (it != positions_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<Position>> Database::get_positions_by_account(
    const std::string& account_id) {
  std::vector<std::shared_ptr<Position>> result;
  for (auto& pair : positions_) {
    if (pair.second->account_id == account_id) {
      result.push_back(pair.second);
    }
  }
  return result;
}

std::shared_ptr<Position> Database::get_position_by_account_symbol(
    const std::string& account_id, const std::string& symbol) {
  for (auto& pair : positions_) {
    if (pair.second->account_id == account_id &&
        pair.second->symbol == symbol) {
      return pair.second;
    }
  }
  return nullptr;
}

// Order operations
void Database::save_order(const Order& order) {
  orders_[order.id] = std::make_shared<Order>(order);
}

std::shared_ptr<Order> Database::get_order(const std::string& order_id) {
  auto it = orders_.find(order_id);
  if (it != orders_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<Order>> Database::get_orders_by_user(
    const std::string& user_id) {
  std::vector<std::shared_ptr<Order>> result;
  for (auto& pair : orders_) {
    if (pair.second->user_id == user_id) {
      result.push_back(pair.second);
    }
  }
  return result;
}

std::vector<std::shared_ptr<Order>> Database::get_pending_orders_by_symbol(
    const std::string& symbol) {
  std::vector<std::shared_ptr<Order>> result;
  for (auto& pair : orders_) {
    if (pair.second->symbol == symbol &&
        (pair.second->status == OrderStatus::PENDING ||
         pair.second->status == OrderStatus::PARTIALLY_FILLED)) {
      result.push_back(pair.second);
    }
  }
  return result;
}

void Database::update_order_status(const std::string& order_id,
                                   OrderStatus status, int filled_qty) {
  auto it = orders_.find(order_id);
  if (it != orders_.end()) {
    it->second->status = status;
    it->second->filled_quantity = filled_qty;
  }
}

// Trade operations
void Database::save_trade(const Trade& trade) {
  trades_[trade.id] = std::make_shared<Trade>(trade);
}

std::shared_ptr<Trade> Database::get_trade(const std::string& trade_id) {
  auto it = trades_.find(trade_id);
  if (it != trades_.end()) {
    return it->second;
  }
  return nullptr;
}

std::vector<std::shared_ptr<Trade>> Database::get_all_trades() {
  std::vector<std::shared_ptr<Trade>> result;
  for (auto& pair : trades_) {
    result.push_back(pair.second);
  }
  return result;
}

std::vector<std::shared_ptr<Trade>> Database::get_trades_by_user(
    const std::string& user_id) {
  std::vector<std::shared_ptr<Trade>> result;
  for (auto& pair : trades_) {
    if (pair.second->buyer_id == user_id ||
        pair.second->seller_id == user_id) {
      result.push_back(pair.second);
    }
  }
  return result;
}

void Database::load_from_disk(const std::string& data_dir) {
  // TODO: Implement JSON loading from data directory
}

void Database::save_to_disk(const std::string& data_dir) {
  // TODO: Implement JSON saving to data directory
}

} // namespace alphapi
