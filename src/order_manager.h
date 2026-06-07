#pragma once

#include "models.h"
#include "database.h"
#include <memory>
#include <vector>

namespace alphapi {

class OrderManager {
 public:
  explicit OrderManager(std::shared_ptr<Database> db) : db_(db) {}
  ~OrderManager() = default;

  // Order CRUD
  std::shared_ptr<Order> create_order(const std::string& user_id,
                                      const std::string& symbol,
                                      OrderSide side, OrderType type,
                                      double price, int quantity);
  std::shared_ptr<Order> get_order(const std::string& order_id);
  std::vector<std::shared_ptr<Order>> get_orders_by_user(
      const std::string& user_id);
  std::vector<std::shared_ptr<Order>> get_pending_orders_by_symbol(
      const std::string& symbol);

  // Order modification
  void update_order(const std::string& order_id, double new_price,
                    int new_quantity);
  void cancel_order(const std::string& order_id);
  void fill_order(const std::string& order_id, int filled_quantity);
  void partially_fill_order(const std::string& order_id,
                            int additional_filled);

 private:
  std::shared_ptr<Database> db_;

  std::string get_next_id(const std::string& prefix);
  int id_counter_ = 1;
};

} // namespace alphapi
