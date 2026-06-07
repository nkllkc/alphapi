#include "src/trading_system.h"
#include "src/rest_api.h"
#include "src/websocket_server.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace alphapi;

int main(int argc, char* argv[]) {
  int rest_port = 8080;
  int ws_port = 8081;

  if (argc > 1) {
    rest_port = std::stoi(argv[1]);
  }
  if (argc > 2) {
    ws_port = std::stoi(argv[2]);
  }

  std::cout << "=== Alphapi Streaming Server ===" << std::endl;

  // Initialize trading system
  auto system = std::make_shared<TradingSystem>();

  // Initialize REST API
  auto rest_api = RestAPI(system, rest_port);
  rest_api.start();

  // Initialize WebSocket Server
  auto ws_server = WebSocketServer(system, ws_port);
  ws_server.start();

  // Seed initial data
  std::cout << "\n--- Initializing Demo Data ---" << std::endl;
  auto alice = system->create_user("Alice Johnson", "alice@example.com");
  auto bob = system->create_user("Bob Smith", "bob@example.com");

  system->create_account(alice->id, 100000.00);
  system->create_account(bob->id, 50000.00);

  system->set_quote(Quote("AAPL", 150.00, 150.50));
  system->set_quote(Quote("MSFT", 350.00, 350.75));
  system->set_quote(Quote("GOOGL", 140.00, 140.50));

  std::cout << "Created users: " << alice->name << ", " << bob->name
            << std::endl;
  std::cout << "Set initial quotes for AAPL, MSFT, GOOGL" << std::endl;

  std::cout << "\n=== REST API Available ===" << std::endl;
  std::cout << "Base URL: http://localhost:" << rest_port << "/api"
            << std::endl;
  std::cout << "Endpoints:" << std::endl;
  std::cout << "  POST   /api/users - Create user" << std::endl;
  std::cout << "  GET    /api/users/:user_id - Get user" << std::endl;
  std::cout << "  POST   /api/accounts - Create account" << std::endl;
  std::cout << "  GET    /api/accounts/:user_id - Get account" << std::endl;
  std::cout << "  POST   /api/orders - Place order" << std::endl;
  std::cout << "  GET    /api/quotes/:symbol - Get quote" << std::endl;
  std::cout << "  GET    /api/trades - Get all trades" << std::endl;

  std::cout << "\n=== WebSocket Streaming Available ===" << std::endl;
  std::cout << "WebSocket URL: ws://localhost:" << ws_port << "/ws"
            << std::endl;
  std::cout << "REST Endpoints:" << std::endl;
  std::cout << "  POST /api/subscribe - Subscribe to symbol" << std::endl;
  std::cout << "    {\"client_id\":\"client_1\",\"symbol\":\"AAPL\"}"
            << std::endl;
  std::cout << "  POST /api/stream/quote - Broadcast quote update"
            << std::endl;
  std::cout << "    {\"symbol\":\"AAPL\",\"bid\":150.25,\"ask\":150.75}"
            << std::endl;
  std::cout << "  GET  /api/stream/clients - List connected clients"
            << std::endl;
  std::cout << "  GET  /ws/health - WebSocket health check" << std::endl;

  std::cout << "\n=== Demo: Market Data Simulation ===" << std::endl;
  std::cout << "Price updates are simulated below..." << std::endl;

  // Simulate market data updates
  int simulation_iterations = 0;
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(5));

    if (ws_server.is_running() && simulation_iterations < 12) {
      // Simulate price movements
      double aapl_bid = 150.00 + (simulation_iterations % 5) * 0.25;
      double aapl_ask = aapl_bid + 0.50;

      Quote aapl_update("AAPL", aapl_bid, aapl_ask);
      system->set_quote(aapl_update);
      ws_server.broadcast_quote(aapl_update);

      std::cout << "[" << std::time(nullptr) << "] Broadcast AAPL: Bid $"
                << std::fixed << std::setprecision(2) << aapl_bid << " | Ask $"
                << aapl_ask << std::endl;

      simulation_iterations++;
    } else if (simulation_iterations >= 12) {
      std::cout << "\nSimulation complete. Servers continue running."
                << std::endl;
      break;
    }
  }

  // Keep servers running
  std::cout << "\nServers are running. Press Ctrl+C to stop" << std::endl;
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
