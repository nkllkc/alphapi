#pragma once

#include "models.h"
#include "database.h"
#include <memory>

namespace alphapi {

class AccountManager {
 public:
  explicit AccountManager(std::shared_ptr<Database> db) : db_(db) {}
  ~AccountManager() = default;

  // Account operations
  std::shared_ptr<Account> create_account(const std::string& user_id,
                                          double initial_cash);
  std::shared_ptr<Account> get_account(const std::string& account_id);
  std::shared_ptr<Account> get_account_by_user(const std::string& user_id);

  // Position operations
  std::shared_ptr<Position> get_position(const std::string& account_id,
                                         const std::string& symbol);
  std::vector<std::shared_ptr<Position>> get_all_positions(
      const std::string& account_id);

  // Validation
  bool validate_buy_order(const std::string& user_id, double price,
                          int quantity);
  bool validate_sell_order(const std::string& user_id,
                           const std::string& symbol, int quantity);

  // Balance updates
  void debit_cash(const std::string& account_id, double amount);
  void credit_cash(const std::string& account_id, double amount);

  // Position updates
  void add_shares(const std::string& account_id, const std::string& symbol,
                  int quantity, double price);
  void remove_shares(const std::string& account_id, const std::string& symbol,
                     int quantity);

 private:
  std::shared_ptr<Database> db_;

  std::string get_next_id(const std::string& prefix);
  int id_counter_ = 1;
};

} // namespace alphapi
