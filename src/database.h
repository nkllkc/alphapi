#pragma once

#include "models.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

namespace alphapi {

class Database {
 public:
  Database() = default;
  ~Database() = default;

  // User operations
  void save_user(const User& user);
  std::shared_ptr<User> get_user(const std::string& user_id);
  std::vector<std::shared_ptr<User>> get_all_users();

  // Account operations
  void save_account(const Account& account);
  std::shared_ptr<Account> get_account(const std::string& account_id);
  std::shared_ptr<Account> get_account_by_user(const std::string& user_id);
  void update_account_balance(const std::string& account_id,
                              double new_balance);

  // Position operations
  void save_position(const Position& position);
  std::shared_ptr<Position> get_position(const std::string& position_id);
  std::vector<std::shared_ptr<Position>> get_positions_by_account(
      const std::string& account_id);
  std::shared_ptr<Position> get_position_by_account_symbol(
      const std::string& account_id, const std::string& symbol);

  // Order operations
  void save_order(const Order& order);
  std::shared_ptr<Order> get_order(const std::string& order_id);
  std::vector<std::shared_ptr<Order>> get_orders_by_user(
      const std::string& user_id);
  std::vector<std::shared_ptr<Order>> get_pending_orders_by_symbol(
      const std::string& symbol);
  void update_order_status(const std::string& order_id, OrderStatus status,
                           int filled_qty);

  // Trade operations
  void save_trade(const Trade& trade);
  std::shared_ptr<Trade> get_trade(const std::string& trade_id);
  std::vector<std::shared_ptr<Trade>> get_all_trades();
  std::vector<std::shared_ptr<Trade>> get_trades_by_user(
      const std::string& user_id);

  // Persistence
  void load_from_disk(const std::string& data_dir);
  void save_to_disk(const std::string& data_dir);

 private:
  // In-memory storage
  std::unordered_map<std::string, std::shared_ptr<User>> users_;
  std::unordered_map<std::string, std::shared_ptr<Account>> accounts_;
  std::unordered_map<std::string, std::shared_ptr<Position>> positions_;
  std::unordered_map<std::string, std::shared_ptr<Order>> orders_;
  std::unordered_map<std::string, std::shared_ptr<Trade>> trades_;

  // Helper methods
  std::string get_next_id(const std::string& prefix);
  int next_id_counter_ = 1;
};

} // namespace alphapi
