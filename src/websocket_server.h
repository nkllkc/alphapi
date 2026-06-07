#pragma once

#include "trading_system.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <mutex>
#include <thread>

namespace alphapi {

using json = nlohmann::json;

struct WebSocketClient {
  std::string client_id;
  std::unordered_set<std::string> subscribed_symbols;
  std::string user_id;  // Empty if not authenticated
  std::shared_ptr<void> connection;  // Opaque connection handle
};

class WebSocketServer {
 public:
  explicit WebSocketServer(std::shared_ptr<TradingSystem> system,
                           int port = 8081);
  ~WebSocketServer() = default;

  // Start the WebSocket server
  void start();

  // Stop the WebSocket server
  void stop();

  // Broadcast market data update
  void broadcast_quote(const Quote& quote);

  // Send order update to specific user
  void send_order_update(const std::string& user_id,
                         const OrderConfirmation& confirmation);

  // Send trade notification
  void send_trade_notification(const std::string& user_id,
                               const std::shared_ptr<Trade>& trade);

  // Check if server is running
  bool is_running() const;

 private:
  std::shared_ptr<TradingSystem> system_;
  int port_;
  bool running_ = false;
  std::mutex clients_mutex_;

  // Client management
  std::unordered_map<std::string, WebSocketClient> connected_clients_;

  // Symbol -> subscriber IDs mapping
  std::unordered_map<std::string, std::unordered_set<std::string>>
      symbol_subscribers_;

  // User -> client IDs mapping
  std::unordered_map<std::string, std::unordered_set<std::string>>
      user_subscriptions_;

  void setup_routes();
  void handle_client_message(const std::string& client_id,
                             const std::string& message);

  // Message handlers
  void handle_subscribe(const std::string& client_id, const json& msg);
  void handle_unsubscribe(const std::string& client_id, const json& msg);
  void handle_authenticate(const std::string& client_id, const json& msg);

  // Broadcast helpers
  void broadcast_to_symbol_subscribers(const std::string& symbol,
                                       const json& message);
  void send_to_user(const std::string& user_id, const json& message);
  void send_to_client(const std::string& client_id, const json& message);

  // Utilities
  std::string generate_client_id();
  int client_counter_ = 1;
};

}  // namespace alphapi
