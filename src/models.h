#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <memory>

namespace alphapi {

enum class OrderSide { BUY, SELL };

enum class OrderType { LIMIT, MARKET };

enum class OrderStatus {
  PENDING,
  PARTIALLY_FILLED,
  FILLED,
  CANCELLED
};

struct User {
  std::string id;
  std::string name;
  std::string email;
  time_t created_at;

  User() = default;
  User(const std::string& id_, const std::string& name_,
       const std::string& email_)
      : id(id_), name(name_), email(email_), created_at(std::time(nullptr)) {}
};

struct Account {
  std::string id;
  std::string user_id;
  double cash_balance;
  time_t created_at;

  Account() : cash_balance(0), created_at(std::time(nullptr)) {}
  Account(const std::string& id_, const std::string& user_id_,
          double cash_)
      : id(id_), user_id(user_id_), cash_balance(cash_),
        created_at(std::time(nullptr)) {}
};

struct Position {
  std::string id;
  std::string account_id;
  std::string symbol;
  int quantity;
  double avg_cost;

  Position() : quantity(0), avg_cost(0) {}
  Position(const std::string& id_, const std::string& account_id_,
           const std::string& symbol_, int qty, double cost)
      : id(id_), account_id(account_id_), symbol(symbol_), quantity(qty),
        avg_cost(cost) {}
};

struct Order {
  std::string id;
  std::string user_id;
  std::string symbol;
  OrderSide side;
  OrderType type;
  double price;
  int quantity;
  int filled_quantity;
  OrderStatus status;
  time_t created_at;

  Order() : price(0), quantity(0), filled_quantity(0),
            created_at(std::time(nullptr)) {}

  Order(const std::string& id_, const std::string& user_id_,
        const std::string& symbol_, OrderSide side_, OrderType type_,
        double price_, int qty_)
      : id(id_), user_id(user_id_), symbol(symbol_), side(side_),
        type(type_), price(price_), quantity(qty_), filled_quantity(0),
        status(OrderStatus::PENDING), created_at(std::time(nullptr)) {}
};

struct Trade {
  std::string id;
  std::string buyer_id;
  std::string seller_id;
  std::string symbol;
  int quantity;
  double price;
  time_t executed_at;

  Trade() : quantity(0), price(0), executed_at(std::time(nullptr)) {}

  Trade(const std::string& id_, const std::string& buyer_id_,
        const std::string& seller_id_, const std::string& symbol_,
        int qty_, double price_)
      : id(id_), buyer_id(buyer_id_), seller_id(seller_id_),
        symbol(symbol_), quantity(qty_), price(price_),
        executed_at(std::time(nullptr)) {}
};

struct Quote {
  std::string symbol;
  double bid;
  double ask;
  time_t timestamp;

  Quote() : bid(0), ask(0), timestamp(std::time(nullptr)) {}

  Quote(const std::string& symbol_, double bid_, double ask_)
      : symbol(symbol_), bid(bid_), ask(ask_),
        timestamp(std::time(nullptr)) {}
};

struct OrderConfirmation {
  std::string order_id;
  OrderStatus status;
  int filled_quantity;
  std::vector<std::shared_ptr<Trade>> trades;
  double updated_balance;
  std::string error_message;
  bool success;

  OrderConfirmation() : filled_quantity(0), updated_balance(0),
                        success(false) {}
};

} // namespace alphapi
