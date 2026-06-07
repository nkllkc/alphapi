#include "order_manager.h"

namespace alphapi {

std::string OrderManager::get_next_id(const std::string& prefix) {
  return prefix + "_" + std::to_string(id_counter_++);
}

std::shared_ptr<Order> OrderManager::create_order(
    const std::string& user_id, const std::string& symbol, OrderSide side,
    OrderType type, double price, int quantity) {
  auto order = std::make_shared<Order>(
      get_next_id("ord"), user_id, symbol, side, type, price, quantity);
  db_->save_order(*order);
  return order;
}

std::shared_ptr<Order> OrderManager::get_order(const std::string& order_id) {
  return db_->get_order(order_id);
}

std::vector<std::shared_ptr<Order>> OrderManager::get_orders_by_user(
    const std::string& user_id) {
  return db_->get_orders_by_user(user_id);
}

std::vector<std::shared_ptr<Order>> OrderManager::get_pending_orders_by_symbol(
    const std::string& symbol) {
  return db_->get_pending_orders_by_symbol(symbol);
}

void OrderManager::update_order(const std::string& order_id, double new_price,
                                 int new_quantity) {
  auto order = get_order(order_id);
  if (order && order->status == OrderStatus::PENDING) {
    order->price = new_price;
    order->quantity = new_quantity;
    db_->save_order(*order);
  }
}

void OrderManager::cancel_order(const std::string& order_id) {
  auto order = get_order(order_id);
  if (order && order->status != OrderStatus::FILLED) {
    db_->update_order_status(order_id, OrderStatus::CANCELLED,
                             order->filled_quantity);
  }
}

void OrderManager::fill_order(const std::string& order_id,
                               int filled_quantity) {
  auto order = get_order(order_id);
  if (order) {
    db_->update_order_status(order_id, OrderStatus::FILLED, filled_quantity);
  }
}

void OrderManager::partially_fill_order(const std::string& order_id,
                                         int additional_filled) {
  auto order = get_order(order_id);
  if (order) {
    int new_filled = order->filled_quantity + additional_filled;
    OrderStatus status =
        (new_filled == order->quantity) ? OrderStatus::FILLED
                                         : OrderStatus::PARTIALLY_FILLED;
    db_->update_order_status(order_id, status, new_filled);
  }
}

} // namespace alphapi
