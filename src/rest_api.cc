#include "rest_api.h"
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <thread>
#include <iostream>

using json = nlohmann::json;

namespace alphapi {

RestAPI::RestAPI(std::shared_ptr<TradingSystem> system, int port)
    : system_(system), port_(port) {}

void RestAPI::start() {
  running_ = true;

  auto server = std::make_shared<httplib::Server>();

  // User endpoints
  server->Post("/api/users", [this](const httplib::Request& req,
                                     httplib::Response& res) {
    auto response = handle_create_user(req.body);
    res.set_content(response, "application/json");
  });

  server->Get("/api/users/:user_id",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_user(user_id);
                res.set_content(response, "application/json");
              });

  // Account endpoints
  server->Post("/api/accounts", [this](const httplib::Request& req,
                                       httplib::Response& res) {
    auto response = handle_create_account(req.body);
    res.set_content(response, "application/json");
  });

  server->Get("/api/accounts/:user_id",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_account(user_id);
                res.set_content(response, "application/json");
              });

  server->Get("/api/accounts/:user_id/summary",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_account_summary(user_id);
                res.set_content(response, "application/json");
              });

  server->Get("/api/accounts/:user_id/balance",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_balance(user_id);
                res.set_content(response, "application/json");
              });

  // Order endpoints
  server->Post("/api/orders", [this](const httplib::Request& req,
                                     httplib::Response& res) {
    auto response = handle_place_order(req.body);
    res.set_content(response, "application/json");
  });

  server->Delete("/api/orders/:order_id",
                 [this](const httplib::Request& req, httplib::Response& res) {
                   auto order_id = req.path_params.at("order_id");
                   // Need user_id from query params
                   auto user_id = req.get_param_value("user_id");
                   auto response = handle_cancel_order(
                       json({{"order_id", order_id}, {"user_id", user_id}})
                           .dump());
                   res.set_content(response, "application/json");
                 });

  server->Get("/api/orders/:order_id",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto order_id = req.path_params.at("order_id");
                auto response = handle_get_order(order_id);
                res.set_content(response, "application/json");
              });

  server->Get("/api/users/:user_id/orders",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_user_orders(user_id);
                res.set_content(response, "application/json");
              });

  // Market data endpoints
  server->Post("/api/quotes", [this](const httplib::Request& req,
                                     httplib::Response& res) {
    auto response = handle_set_quote(req.body);
    res.set_content(response, "application/json");
  });

  server->Get("/api/quotes/:symbol",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto symbol = req.path_params.at("symbol");
                auto response = handle_get_quote(symbol);
                res.set_content(response, "application/json");
              });

  server->Get("/api/orderbook/:symbol",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto symbol = req.path_params.at("symbol");
                auto response = handle_get_order_book(symbol);
                res.set_content(response, "application/json");
              });

  // Position endpoints
  server->Get("/api/users/:user_id/positions",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_positions(user_id);
                res.set_content(response, "application/json");
              });

  // Trade endpoints
  server->Get("/api/users/:user_id/trades",
              [this](const httplib::Request& req, httplib::Response& res) {
                auto user_id = req.path_params.at("user_id");
                auto response = handle_get_trades(user_id);
                res.set_content(response, "application/json");
              });

  server->Get("/api/trades", [this](const httplib::Request& req,
                                    httplib::Response& res) {
    auto response = handle_get_all_trades();
    res.set_content(response, "application/json");
  });

  // Health check
  server->Get("/api/health", [](const httplib::Request& req,
                                httplib::Response& res) {
    res.set_content(json({{"status", "ok"}}).dump(), "application/json");
  });

  // Run server in background thread
  auto thread = std::thread([this, server]() {
    std::cout << "REST API Server listening on port " << port_ << std::endl;
    server->listen("localhost", port_);
  });

  thread.detach();
  std::cout << "REST API started on http://localhost:" << port_ << std::endl;
}

void RestAPI::stop() {
  running_ = false;
}

bool RestAPI::is_running() const {
  return running_;
}

std::string RestAPI::error_response(const std::string& message) {
  json response;
  response["success"] = false;
  response["error"] = message;
  return response.dump();
}

std::string RestAPI::success_response(const std::string& data) {
  json response;
  response["success"] = true;
  response["data"] = json::parse(data);
  return response.dump();
}

// User endpoints
std::string RestAPI::handle_create_user(const std::string& request_body) {
  try {
    auto req = json::parse(request_body);
    std::string name = req["name"];
    std::string email = req["email"];

    auto user = system_->create_user(name, email);

    json response;
    response["id"] = user->id;
    response["name"] = user->name;
    response["email"] = user->email;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_user(const std::string& user_id) {
  try {
    auto user = system_->get_user(user_id);
    if (!user) {
      return error_response("User not found");
    }

    json response;
    response["id"] = user->id;
    response["name"] = user->name;
    response["email"] = user->email;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

// Account endpoints
std::string RestAPI::handle_create_account(const std::string& request_body) {
  try {
    auto req = json::parse(request_body);
    std::string user_id = req["user_id"];
    double initial_cash = req["initial_cash"];

    auto account = system_->create_account(user_id, initial_cash);

    json response;
    response["id"] = account->id;
    response["user_id"] = account->user_id;
    response["cash_balance"] = account->cash_balance;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_account(const std::string& user_id) {
  try {
    auto account = system_->get_account_by_user(user_id);
    if (!account) {
      return error_response("Account not found");
    }

    json response;
    response["id"] = account->id;
    response["user_id"] = account->user_id;
    response["cash_balance"] = account->cash_balance;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_balance(const std::string& user_id) {
  try {
    double balance = system_->get_balance(user_id);
    json response;
    response["user_id"] = user_id;
    response["balance"] = balance;
    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_account_summary(const std::string& user_id) {
  try {
    auto summary = system_->get_account_summary(user_id);

    json positions_json = json::array();
    for (const auto& pos : summary.positions) {
      json pos_json;
      pos_json["symbol"] = pos->symbol;
      pos_json["quantity"] = pos->quantity;
      pos_json["avg_cost"] = pos->avg_cost;
      positions_json.push_back(pos_json);
    }

    json response;
    response["user_id"] = summary.user_id;
    response["cash_balance"] = summary.cash_balance;
    response["positions"] = positions_json;
    response["total_value"] = summary.total_value;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

// Order endpoints
std::string RestAPI::handle_place_order(const std::string& request_body) {
  try {
    auto req = json::parse(request_body);
    std::string user_id = req["user_id"];
    std::string symbol = req["symbol"];
    std::string side_str = req["side"];
    std::string type_str = req["type"];
    double price = req["price"];
    int quantity = req["quantity"];

    OrderSide side = (side_str == "BUY") ? OrderSide::BUY : OrderSide::SELL;
    OrderType type =
        (type_str == "LIMIT") ? OrderType::LIMIT : OrderType::MARKET;

    auto confirmation = system_->place_order(user_id, symbol, side, type,
                                             price, quantity);

    json trades_json = json::array();
    for (const auto& trade : confirmation.trades) {
      json trade_json;
      trade_json["id"] = trade->id;
      trade_json["quantity"] = trade->quantity;
      trade_json["price"] = trade->price;
      trades_json.push_back(trade_json);
    }

    json response;
    response["order_id"] = confirmation.order_id;
    response["success"] = confirmation.success;
    response["filled_quantity"] = confirmation.filled_quantity;
    response["updated_balance"] = confirmation.updated_balance;
    response["trades"] = trades_json;

    if (!confirmation.success) {
      response["error"] = confirmation.error_message;
    }

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_cancel_order(const std::string& request_body) {
  try {
    auto req = json::parse(request_body);
    std::string user_id = req["user_id"];
    std::string order_id = req["order_id"];

    bool success = system_->cancel_order(user_id, order_id);

    json response;
    response["order_id"] = order_id;
    response["cancelled"] = success;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_order(const std::string& order_id) {
  try {
    auto order = system_->get_order(order_id);
    if (!order) {
      return error_response("Order not found");
    }

    json response;
    response["id"] = order->id;
    response["user_id"] = order->user_id;
    response["symbol"] = order->symbol;
    response["side"] = (order->side == OrderSide::BUY) ? "BUY" : "SELL";
    response["type"] = (order->type == OrderType::LIMIT) ? "LIMIT" : "MARKET";
    response["price"] = order->price;
    response["quantity"] = order->quantity;
    response["filled_quantity"] = order->filled_quantity;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_user_orders(const std::string& user_id) {
  try {
    auto orders = system_->get_user_orders(user_id);

    json orders_json = json::array();
    for (const auto& order : orders) {
      json order_json;
      order_json["id"] = order->id;
      order_json["symbol"] = order->symbol;
      order_json["side"] = (order->side == OrderSide::BUY) ? "BUY" : "SELL";
      order_json["quantity"] = order->quantity;
      order_json["filled_quantity"] = order->filled_quantity;
      order_json["price"] = order->price;
      orders_json.push_back(order_json);
    }

    return success_response(orders_json.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

// Market data endpoints
std::string RestAPI::handle_set_quote(const std::string& request_body) {
  try {
    auto req = json::parse(request_body);
    std::string symbol = req["symbol"];
    double bid = req["bid"];
    double ask = req["ask"];

    system_->set_quote(Quote(symbol, bid, ask));

    json response;
    response["symbol"] = symbol;
    response["bid"] = bid;
    response["ask"] = ask;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_quote(const std::string& symbol) {
  try {
    auto quote = system_->get_market_data(symbol);
    if (!quote) {
      return error_response("Quote not found");
    }

    json response;
    response["symbol"] = quote->symbol;
    response["bid"] = quote->bid;
    response["ask"] = quote->ask;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_order_book(const std::string& symbol) {
  try {
    auto book = system_->get_order_book(symbol);

    auto buy_orders = book->get_buy_orders();
    auto sell_orders = book->get_sell_orders();

    json buy_json = json::array();
    for (const auto& order : buy_orders) {
      json order_json;
      order_json["price"] = order->price;
      order_json["quantity"] = order->quantity - order->filled_quantity;
      buy_json.push_back(order_json);
    }

    json sell_json = json::array();
    for (const auto& order : sell_orders) {
      json order_json;
      order_json["price"] = order->price;
      order_json["quantity"] = order->quantity - order->filled_quantity;
      sell_json.push_back(order_json);
    }

    json response;
    response["symbol"] = symbol;
    response["bids"] = buy_json;
    response["asks"] = sell_json;

    return success_response(response.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

// Position endpoints
std::string RestAPI::handle_get_positions(const std::string& user_id) {
  try {
    auto positions = system_->get_positions(user_id);

    json positions_json = json::array();
    for (const auto& pos : positions) {
      json pos_json;
      pos_json["symbol"] = pos->symbol;
      pos_json["quantity"] = pos->quantity;
      pos_json["avg_cost"] = pos->avg_cost;
      positions_json.push_back(pos_json);
    }

    return success_response(positions_json.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

// Trade endpoints
std::string RestAPI::handle_get_trades(const std::string& user_id) {
  try {
    auto trades = system_->get_trades(user_id);

    json trades_json = json::array();
    for (const auto& trade : trades) {
      json trade_json;
      trade_json["id"] = trade->id;
      trade_json["symbol"] = trade->symbol;
      trade_json["quantity"] = trade->quantity;
      trade_json["price"] = trade->price;
      trades_json.push_back(trade_json);
    }

    return success_response(trades_json.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

std::string RestAPI::handle_get_all_trades() {
  try {
    auto trades = system_->get_all_trades();

    json trades_json = json::array();
    for (const auto& trade : trades) {
      json trade_json;
      trade_json["id"] = trade->id;
      trade_json["buyer_id"] = trade->buyer_id;
      trade_json["seller_id"] = trade->seller_id;
      trade_json["symbol"] = trade->symbol;
      trade_json["quantity"] = trade->quantity;
      trade_json["price"] = trade->price;
      trades_json.push_back(trade_json);
    }

    return success_response(trades_json.dump());
  } catch (const std::exception& e) {
    return error_response(e.what());
  }
}

} // namespace alphapi
