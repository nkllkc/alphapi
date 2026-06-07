#include "account_manager.h"
#include <cmath>

namespace alphapi {

std::string AccountManager::get_next_id(const std::string& prefix) {
  return prefix + "_" + std::to_string(id_counter_++);
}

std::shared_ptr<Account> AccountManager::create_account(
    const std::string& user_id, double initial_cash) {
  auto account = std::make_shared<Account>(
      get_next_id("acc"), user_id, initial_cash);
  db_->save_account(*account);
  return account;
}

std::shared_ptr<Account> AccountManager::get_account(
    const std::string& account_id) {
  return db_->get_account(account_id);
}

std::shared_ptr<Account> AccountManager::get_account_by_user(
    const std::string& user_id) {
  return db_->get_account_by_user(user_id);
}

std::shared_ptr<Position> AccountManager::get_position(
    const std::string& account_id, const std::string& symbol) {
  return db_->get_position_by_account_symbol(account_id, symbol);
}

std::vector<std::shared_ptr<Position>> AccountManager::get_all_positions(
    const std::string& account_id) {
  return db_->get_positions_by_account(account_id);
}

bool AccountManager::validate_buy_order(const std::string& user_id,
                                        double price, int quantity) {
  auto account = get_account_by_user(user_id);
  if (!account) {
    return false;
  }

  double required_cash = price * quantity;
  return account->cash_balance >= required_cash;
}

bool AccountManager::validate_sell_order(const std::string& user_id,
                                         const std::string& symbol,
                                         int quantity) {
  auto account = get_account_by_user(user_id);
  if (!account) {
    return false;
  }

  auto position = get_position(account->id, symbol);
  if (!position) {
    return false;
  }

  return position->quantity >= quantity;
}

void AccountManager::debit_cash(const std::string& account_id,
                                 double amount) {
  auto account = get_account(account_id);
  if (account) {
    account->cash_balance -= amount;
    db_->update_account_balance(account_id, account->cash_balance);
  }
}

void AccountManager::credit_cash(const std::string& account_id,
                                  double amount) {
  auto account = get_account(account_id);
  if (account) {
    account->cash_balance += amount;
    db_->update_account_balance(account_id, account->cash_balance);
  }
}

void AccountManager::add_shares(const std::string& account_id,
                                const std::string& symbol, int quantity,
                                double price) {
  auto position = get_position(account_id, symbol);

  if (position) {
    // Update average cost
    int total_qty = position->quantity + quantity;
    double total_cost =
        position->quantity * position->avg_cost + quantity * price;
    position->avg_cost = total_cost / total_qty;
    position->quantity = total_qty;
  } else {
    // Create new position
    auto new_pos = std::make_shared<Position>(
        "pos_" + std::to_string(id_counter_++), account_id, symbol, quantity,
        price);
    db_->save_position(*new_pos);
  }
}

void AccountManager::remove_shares(const std::string& account_id,
                                   const std::string& symbol,
                                   int quantity) {
  auto position = get_position(account_id, symbol);
  if (position) {
    position->quantity -= quantity;
    if (position->quantity == 0) {
      // Position can be deleted (left as-is for simplicity)
    }
  }
}

} // namespace alphapi
