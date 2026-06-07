#include "src/trading_system.h"
#include "src/rest_api.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace alphapi;

int main(int argc, char* argv[]) {
  int port = 8080;

  if (argc > 1) {
    port = std::stoi(argv[1]);
  }

  std::cout << "=== Alphapi REST API Server ===" << std::endl;

  // Initialize trading system
  auto system = std::make_shared<TradingSystem>();

  // Initialize REST API
  auto rest_api = RestAPI(system, port);
  rest_api.start();

  // Seed some initial data for testing
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
  std::cout << "Endpoints:" << std::endl;
  std::cout << "  POST   /api/users - Create user" << std::endl;
  std::cout << "  GET    /api/users/:user_id - Get user" << std::endl;
  std::cout << "  POST   /api/accounts - Create account" << std::endl;
  std::cout << "  GET    /api/accounts/:user_id - Get account" << std::endl;
  std::cout << "  GET    /api/accounts/:user_id/summary - Account summary"
            << std::endl;
  std::cout << "  POST   /api/orders - Place order" << std::endl;
  std::cout << "  GET    /api/orders/:order_id - Get order" << std::endl;
  std::cout << "  DELETE /api/orders/:order_id - Cancel order" << std::endl;
  std::cout << "  POST   /api/quotes - Set quote" << std::endl;
  std::cout << "  GET    /api/quotes/:symbol - Get quote" << std::endl;
  std::cout << "  GET    /api/orderbook/:symbol - Get order book"
            << std::endl;
  std::cout << "  GET    /api/users/:user_id/positions - Get positions"
            << std::endl;
  std::cout << "  GET    /api/users/:user_id/trades - Get user trades"
            << std::endl;
  std::cout << "  GET    /api/trades - Get all trades" << std::endl;
  std::cout << "  GET    /api/health - Health check" << std::endl;

  std::cout << "\nServer is running at http://localhost:" << port << std::endl;
  std::cout << "Press Ctrl+C to stop" << std::endl;

  // Keep server running
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
