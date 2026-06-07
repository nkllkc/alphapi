#pragma once

#include "trading_system.h"
#include <memory>
#include <string>

namespace alphapi {

class RestAPI {
 public:
  explicit RestAPI(std::shared_ptr<TradingSystem> system, int port = 8080);
  ~RestAPI() = default;

  // Start the HTTP server
  void start();

  // Stop the HTTP server
  void stop();

  // Check if server is running
  bool is_running() const;

 private:
  std::shared_ptr<TradingSystem> system_;
  int port_;
  bool running_ = false;

  // Handler methods (return JSON responses)
  void setup_routes();

  // User endpoints
  std::string handle_create_user(const std::string& request_body);
  std::string handle_get_user(const std::string& user_id);

  // Account endpoints
  std::string handle_create_account(const std::string& request_body);
  std::string handle_get_account(const std::string& user_id);
  std::string handle_get_balance(const std::string& user_id);
  std::string handle_get_account_summary(const std::string& user_id);

  // Order endpoints
  std::string handle_place_order(const std::string& request_body);
  std::string handle_cancel_order(const std::string& request_body);
  std::string handle_get_order(const std::string& order_id);
  std::string handle_get_user_orders(const std::string& user_id);

  // Market data endpoints
  std::string handle_set_quote(const std::string& request_body);
  std::string handle_get_quote(const std::string& symbol);
  std::string handle_get_order_book(const std::string& symbol);

  // Position endpoints
  std::string handle_get_positions(const std::string& user_id);

  // Trade endpoints
  std::string handle_get_trades(const std::string& user_id);
  std::string handle_get_all_trades();

  // Helper methods
  std::string error_response(const std::string& message);
  std::string success_response(const std::string& data);
};

} // namespace alphapi
