#include "websocket_server.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace alphapi {

WebSocketServer::WebSocketServer(std::shared_ptr<TradingSystem> system,
                                 int port)
    : system_(system), port_(port) {}

void WebSocketServer::start() {
  running_ = true;

  auto server = std::make_shared<httplib::Server>();

  // WebSocket endpoints
  server->Get("/ws", [this](const httplib::Request& req,
                            httplib::Response& res) {
    // Check for WebSocket upgrade
    if (req.has_header("Upgrade") && req.has_header("Connection")) {
      std::string upgrade = req.get_header_value("Upgrade");
      std::string connection = req.get_header_value("Connection");

      if (upgrade == "websocket" && connection.find("Upgrade") != std::string::npos) {
        std::string client_id = generate_client_id();

        // Create client entry
        {
          std::lock_guard<std::mutex> lock(clients_mutex_);
          WebSocketClient client;
          client.client_id = client_id;
          client.user_id = "";
          connected_clients_[client_id] = client;
        }

        std::cout << "WebSocket client connected: " << client_id << std::endl;

        // Send welcome message
        json welcome;
        welcome["type"] = "welcome";
        welcome["client_id"] = client_id;
        welcome["message"] =
            "Connected to Alphapi Market Data Stream";
        send_to_client(client_id, welcome);

        // Keep connection alive (simplified - just return 200 for now)
        res.status = 200;
        return;
      }
    }

    res.status = 400;
    res.set_content("WebSocket connection required", "text/plain");
  });

  // REST endpoint for testing subscription
  server->Post("/api/subscribe", [this](const httplib::Request& req,
                                        httplib::Response& res) {
    try {
      auto msg = json::parse(req.body);
      std::string symbol = msg["symbol"];
      std::string client_id = msg["client_id"];

      handle_subscribe(client_id, msg);

      json response;
      response["type"] = "subscribe_response";
      response["status"] = "subscribed";
      response["symbol"] = symbol;

      res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
      json error;
      error["error"] = e.what();
      res.set_content(error.dump(), "application/json");
    }
  });

  // Broadcast quote endpoint for testing
  server->Post("/api/stream/quote", [this](const httplib::Request& req,
                                           httplib::Response& res) {
    try {
      auto quote_data = json::parse(req.body);
      Quote quote(quote_data["symbol"], quote_data["bid"], quote_data["ask"]);

      system_->set_quote(quote);
      broadcast_quote(quote);

      json response;
      response["type"] = "quote";
      response["symbol"] = quote.symbol;
      response["bid"] = quote.bid;
      response["ask"] = quote.ask;

      res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
      json error;
      error["error"] = e.what();
      res.set_content(error.dump(), "application/json");
    }
  });

  // Health check endpoint
  server->Get("/ws/health", [](const httplib::Request& req,
                               httplib::Response& res) {
    json response;
    response["status"] = "ok";
    response["service"] = "websocket_server";
    res.set_content(response.dump(), "application/json");
  });

  // List connected clients
  server->Get("/api/stream/clients", [this](const httplib::Request& req,
                                            httplib::Response& res) {
    std::lock_guard<std::mutex> lock(clients_mutex_);

    json clients_json = json::array();
    for (const auto& [client_id, client] : connected_clients_) {
      json client_json;
      client_json["client_id"] = client.client_id;
      client_json["user_id"] = client.user_id.empty() ? "anonymous"
                                                       : client.user_id;

      json symbols = json::array();
      for (const auto& symbol : client.subscribed_symbols) {
        symbols.push_back(symbol);
      }
      client_json["subscribed_symbols"] = symbols;

      clients_json.push_back(client_json);
    }

    json response;
    response["connected_clients"] = clients_json.size();
    response["clients"] = clients_json;

    res.set_content(response.dump(), "application/json");
  });

  // Run server in background thread
  auto thread = std::thread([this, server]() {
    std::cout << "WebSocket Server listening on port " << port_ << std::endl;
    server->listen("localhost", port_);
  });

  thread.detach();
  std::cout << "WebSocket Server started on ws://localhost:" << port_ << "/ws"
            << std::endl;
}

void WebSocketServer::stop() {
  running_ = false;
  std::lock_guard<std::mutex> lock(clients_mutex_);
  connected_clients_.clear();
}

bool WebSocketServer::is_running() const {
  return running_;
}

std::string WebSocketServer::generate_client_id() {
  return "client_" + std::to_string(client_counter_++);
}

void WebSocketServer::broadcast_quote(const Quote& quote) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  json message;
  message["type"] = "quote";
  message["symbol"] = quote.symbol;
  message["bid"] = quote.bid;
  message["ask"] = quote.ask;
  message["timestamp"] = quote.timestamp;

  // Send to all subscribers of this symbol
  broadcast_to_symbol_subscribers(quote.symbol, message);
}

void WebSocketServer::send_order_update(const std::string& user_id,
                                         const OrderConfirmation& confirmation) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  json message;
  message["type"] = "order_update";
  message["order_id"] = confirmation.order_id;
  message["success"] = confirmation.success;
  message["filled_quantity"] = confirmation.filled_quantity;
  message["updated_balance"] = confirmation.updated_balance;

  json trades = json::array();
  for (const auto& trade : confirmation.trades) {
    json trade_json;
    trade_json["id"] = trade->id;
    trade_json["quantity"] = trade->quantity;
    trade_json["price"] = trade->price;
    trades.push_back(trade_json);
  }
  message["trades"] = trades;

  if (!confirmation.success) {
    message["error"] = confirmation.error_message;
  }

  send_to_user(user_id, message);
}

void WebSocketServer::send_trade_notification(const std::string& user_id,
                                               const std::shared_ptr<Trade>& trade) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  json message;
  message["type"] = "trade";
  message["trade_id"] = trade->id;
  message["symbol"] = trade->symbol;
  message["quantity"] = trade->quantity;
  message["price"] = trade->price;

  // Check if user is buyer or seller
  if (trade->buyer_id == user_id) {
    message["side"] = "BUY";
  } else if (trade->seller_id == user_id) {
    message["side"] = "SELL";
  }

  send_to_user(user_id, message);
}

void WebSocketServer::broadcast_to_symbol_subscribers(
    const std::string& symbol, const json& message) {
  auto it = symbol_subscribers_.find(symbol);
  if (it != symbol_subscribers_.end()) {
    for (const auto& client_id : it->second) {
      send_to_client(client_id, message);
    }
  }
}

void WebSocketServer::send_to_user(const std::string& user_id,
                                   const json& message) {
  auto it = user_subscriptions_.find(user_id);
  if (it != user_subscriptions_.end()) {
    for (const auto& client_id : it->second) {
      send_to_client(client_id, message);
    }
  }
}

void WebSocketServer::send_to_client(const std::string& client_id,
                                      const json& message) {
  // In a real implementation, this would send over the WebSocket
  // For now, we log it for demonstration
  std::cout << "Message to client " << client_id << ": " << message.dump()
            << std::endl;
}

void WebSocketServer::handle_client_message(const std::string& client_id,
                                             const std::string& message) {
  try {
    auto msg = json::parse(message);
    std::string type = msg["type"];

    if (type == "subscribe") {
      handle_subscribe(client_id, msg);
    } else if (type == "unsubscribe") {
      handle_unsubscribe(client_id, msg);
    } else if (type == "authenticate") {
      handle_authenticate(client_id, msg);
    }
  } catch (const std::exception& e) {
    std::cerr << "Error handling client message: " << e.what() << std::endl;
  }
}

void WebSocketServer::handle_subscribe(const std::string& client_id,
                                        const json& msg) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  if (msg.contains("symbol")) {
    std::string symbol = msg["symbol"];

    // Add client to symbol subscribers
    symbol_subscribers_[symbol].insert(client_id);

    // Update client's subscriptions
    connected_clients_[client_id].subscribed_symbols.insert(symbol);

    std::cout << "Client " << client_id << " subscribed to " << symbol
              << std::endl;

    // Send confirmation
    json confirmation;
    confirmation["type"] = "subscribed";
    confirmation["symbol"] = symbol;
    send_to_client(client_id, confirmation);
  }
}

void WebSocketServer::handle_unsubscribe(const std::string& client_id,
                                          const json& msg) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  if (msg.contains("symbol")) {
    std::string symbol = msg["symbol"];

    // Remove client from symbol subscribers
    auto it = symbol_subscribers_.find(symbol);
    if (it != symbol_subscribers_.end()) {
      it->second.erase(client_id);
    }

    // Update client's subscriptions
    connected_clients_[client_id].subscribed_symbols.erase(symbol);

    std::cout << "Client " << client_id << " unsubscribed from " << symbol
              << std::endl;
  }
}

void WebSocketServer::handle_authenticate(const std::string& client_id,
                                           const json& msg) {
  std::lock_guard<std::mutex> lock(clients_mutex_);

  if (msg.contains("user_id")) {
    std::string user_id = msg["user_id"];

    // Update client
    connected_clients_[client_id].user_id = user_id;

    // Add to user subscriptions
    user_subscriptions_[user_id].insert(client_id);

    std::cout << "Client " << client_id << " authenticated as " << user_id
              << std::endl;

    // Send confirmation
    json confirmation;
    confirmation["type"] = "authenticated";
    confirmation["user_id"] = user_id;
    send_to_client(client_id, confirmation);
  }
}

}  // namespace alphapi
