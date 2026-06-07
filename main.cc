#include "src/trading_system.h"
#include <iostream>
#include <iomanip>

using namespace alphapi;

void print_order_confirmation(const OrderConfirmation& conf) {
  std::cout << "\n=== Order Confirmation ===" << std::endl;
  std::cout << "Order ID: " << conf.order_id << std::endl;
  std::cout << "Success: " << (conf.success ? "Yes" : "No") << std::endl;

  if (!conf.success) {
    std::cout << "Error: " << conf.error_message << std::endl;
    return;
  }

  std::cout << "Status: ";
  switch (conf.status) {
    case OrderStatus::PENDING:
      std::cout << "PENDING";
      break;
    case OrderStatus::PARTIALLY_FILLED:
      std::cout << "PARTIALLY_FILLED";
      break;
    case OrderStatus::FILLED:
      std::cout << "FILLED";
      break;
    case OrderStatus::CANCELLED:
      std::cout << "CANCELLED";
      break;
  }
  std::cout << std::endl;

  std::cout << "Filled Quantity: " << conf.filled_quantity << std::endl;
  std::cout << "Number of Trades: " << conf.trades.size() << std::endl;

  if (!conf.trades.empty()) {
    std::cout << "\nTrades Executed:" << std::endl;
    for (const auto& trade : conf.trades) {
      std::cout << "  - " << trade->quantity << " shares @ $"
                << std::fixed << std::setprecision(2) << trade->price
                << std::endl;
    }
  }

  std::cout << "Updated Balance: $" << std::fixed << std::setprecision(2)
            << conf.updated_balance << std::endl;
}

void print_account_summary(const TradingSystem::AccountSummary& summary) {
  std::cout << "\n=== Account Summary ===" << std::endl;
  std::cout << "User ID: " << summary.user_id << std::endl;
  std::cout << "Cash Balance: $" << std::fixed << std::setprecision(2)
            << summary.cash_balance << std::endl;

  if (!summary.positions.empty()) {
    std::cout << "\nPositions:" << std::endl;
    for (const auto& pos : summary.positions) {
      std::cout << "  " << pos->symbol << ": " << pos->quantity << " shares "
                << "@ avg cost $" << std::fixed << std::setprecision(2)
                << pos->avg_cost << std::endl;
    }
  }

  std::cout << "Total Portfolio Value: $" << std::fixed << std::setprecision(2)
            << summary.total_value << std::endl;
}

int main() {
  std::cout << "=== Alphapi Stock Exchange Trading System ===" << std::endl;

  // Initialize system
  TradingSystem system;

  // Create users
  std::cout << "\n--- Creating Users ---" << std::endl;
  auto alice = system.create_user("Alice Johnson", "alice@example.com");
  auto bob = system.create_user("Bob Smith", "bob@example.com");

  std::cout << "Created user: " << alice->name << " (" << alice->id << ")"
            << std::endl;
  std::cout << "Created user: " << bob->name << " (" << bob->id << ")"
            << std::endl;

  // Create accounts with initial cash
  std::cout << "\n--- Creating Accounts ---" << std::endl;
  auto alice_acc = system.create_account(alice->id, 100000.00);
  auto bob_acc = system.create_account(bob->id, 50000.00);

  std::cout << "Alice's Account: " << alice_acc->id << " ($"
            << std::fixed << std::setprecision(2) << alice_acc->cash_balance
            << ")" << std::endl;
  std::cout << "Bob's Account: " << bob_acc->id << " ($"
            << std::fixed << std::setprecision(2) << bob_acc->cash_balance
            << ")" << std::endl;

  // Set up price feed
  std::cout << "\n--- Setting Up Prices ---" << std::endl;
  system.set_quote(Quote("AAPL", 150.00, 150.50));
  system.set_quote(Quote("MSFT", 350.00, 350.75));
  system.set_quote(Quote("GOOGL", 140.00, 140.50));

  std::cout << "AAPL: Bid $150.00 | Ask $150.50" << std::endl;
  std::cout << "MSFT: Bid $350.00 | Ask $350.75" << std::endl;
  std::cout << "GOOGL: Bid $140.00 | Ask $140.50" << std::endl;

  // Alice places a BUY order for AAPL
  std::cout << "\n--- Alice BUY AAPL ---" << std::endl;
  auto conf1 = system.place_order(alice->id, "AAPL", OrderSide::BUY,
                                  OrderType::LIMIT, 150.25, 100);
  print_order_confirmation(conf1);

  // Bob places a SELL order for AAPL at same price
  std::cout << "\n--- Bob SELL AAPL ---" << std::endl;
  auto conf2 = system.place_order(bob->id, "AAPL", OrderSide::SELL,
                                  OrderType::LIMIT, 150.00, 50);
  print_order_confirmation(conf2);

  // View account summaries
  auto alice_summary = system.get_account_summary(alice->id);
  print_account_summary(alice_summary);

  auto bob_summary = system.get_account_summary(bob->id);
  print_account_summary(bob_summary);

  // Get all trades
  std::cout << "\n=== All Trades ===" << std::endl;
  auto all_trades = system.get_all_trades();
  std::cout << "Total trades executed: " << all_trades.size() << std::endl;

  for (const auto& trade : all_trades) {
    std::cout << "Trade " << trade->id << ": " << trade->quantity
              << " shares @ $" << std::fixed << std::setprecision(2)
              << trade->price << " (Buyer: " << trade->buyer_id
              << ", Seller: " << trade->seller_id << ")" << std::endl;
  }

  // Test order book
  std::cout << "\n=== Order Book for AAPL ===" << std::endl;
  auto book = system.get_order_book("AAPL");
  auto buy_orders = book->get_buy_orders();
  auto sell_orders = book->get_sell_orders();

  std::cout << "Buy Orders: " << buy_orders.size() << std::endl;
  for (const auto& order : buy_orders) {
    std::cout << "  $" << std::fixed << std::setprecision(2) << order->price
              << " x " << (order->quantity - order->filled_quantity)
              << std::endl;
  }

  std::cout << "Sell Orders: " << sell_orders.size() << std::endl;
  for (const auto& order : sell_orders) {
    std::cout << "  $" << std::fixed << std::setprecision(2) << order->price
              << " x " << (order->quantity - order->filled_quantity)
              << std::endl;
  }

  std::cout << "\n=== Trading System Demo Complete ===" << std::endl;

  return 0;
}
