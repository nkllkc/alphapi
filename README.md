# Alphapi - Stock Market Trading System

A high-performance C++ stock exchange trading system with order matching, price feeds, and account management. Built with the Blaze build system.

## Features

- **Order Matching Engine** - Efficient price-time priority matching for buy/sell orders
- **Limit & Market Orders** - Support for different order types
- **Order Management** - Create, modify, and cancel orders
- **Account Management** - Track cash balances and stock positions
- **Price Feed System** - Real-time quotes and historical price data
- **Trade Execution** - Automatic settlement and position updates
- **Order Book** - Full order book visibility by symbol

## Architecture

### Core Components

1. **Models** (`src/models.h`) - Core data structures
   - User, Account, Position, Order, Trade, Quote

2. **Database** (`src/database.*`) - In-memory data storage
   - Persistent storage interface for users, accounts, positions, orders, trades

3. **Price Feed** (`src/price_feed.*`) - Price data management
   - Load quotes from CSV, maintain price history
   - Support for real-time quote updates

4. **Order Manager** (`src/order_manager.*`) - Order CRUD operations
   - Create, retrieve, modify, and cancel orders
   - Track order status and fill quantities

5. **Account Manager** (`src/account_manager.*`) - User account management
   - Create and manage accounts
   - Track cash balances and stock positions
   - Validate orders before execution

6. **Matching Engine** (`src/matching_engine.*`) - Core trading logic
   - OrderBook data structure per symbol
   - Price-time priority matching algorithm
   - Buy order matching against sell orders and vice versa

7. **Trade Executor** (`src/executor.*`) - Trade settlement
   - Execute matched trades
   - Update account balances and positions
   - Record trade history

8. **Trading System** (`src/trading_system.*`) - Main orchestrator
   - High-level API for all trading operations
   - Coordinates all components
   - Provides account summaries and market data

## Building

### Prerequisites

- C++11 or later
- Bazel/Blaze build system

### Build Commands

```bash
# Build the entire project
blaze build //...

# Build just the trading system binary
blaze build //:trading_system

# Run the demo
blaze run //:trading_system
```

## Usage Example

```cpp
#include "src/trading_system.h"
using namespace alphapi;

// Initialize system
TradingSystem system;

// Create users and accounts
auto user = system.create_user("Alice", "alice@example.com");
auto account = system.create_account(user->id, 100000.00);

// Set prices
system.set_quote(Quote("AAPL", 150.00, 150.50));

// Place a buy order
auto confirmation = system.place_order(
    user->id, "AAPL", OrderSide::BUY, OrderType::LIMIT, 150.25, 100);

// Get account summary
auto summary = system.get_account_summary(user->id);
std::cout << "Balance: $" << summary.cash_balance << std::endl;
std::cout << "Total Value: $" << summary.total_value << std::endl;
```

## Order Matching Algorithm

The system uses **price-time priority** matching:

- **Buy Orders**: Matched against sell orders at the lowest price, earliest timestamp first
- **Sell Orders**: Matched against buy orders at the highest price, earliest timestamp first
- **Partial Fills**: Orders can be partially filled and remain on the book

### Matching Flow

1. New order arrives at matching engine
2. Opposite side order book is scanned in priority order
3. Matching orders are executed as trades
4. Remaining quantity stays on order book (if any)
5. All trades are settled immediately (T+0)

## Data Models

### Order

```cpp
struct Order {
  std::string id;
  std::string user_id;
  std::string symbol;
  OrderSide side;      // BUY or SELL
  OrderType type;      // LIMIT or MARKET
  double price;
  int quantity;
  int filled_quantity; // Number of shares matched
  OrderStatus status;  // PENDING, PARTIALLY_FILLED, FILLED, CANCELLED
  time_t created_at;
};
```

### Trade

```cpp
struct Trade {
  std::string id;
  std::string buyer_id;
  std::string seller_id;
  std::string symbol;
  int quantity;
  double price;         // Execution price
  time_t executed_at;
};
```

### Position

```cpp
struct Position {
  std::string id;
  std::string account_id;
  std::string symbol;
  int quantity;
  double avg_cost;      // Average cost basis
};
```

## File Structure

```
/alphapi/
├── BUILD                 # Root build file
├── WORKSPACE             # Bazel workspace configuration
├── main.cc              # Demo/entry point
├── README.md            # This file
├── data/
│   └── prices.csv       # Sample price data
└── src/
    ├── BUILD            # Source build configuration
    ├── models.h         # Core data structures
    ├── database.h/cc    # Data storage layer
    ├── price_feed.h/cc  # Price management
    ├── account_manager.h/cc
    ├── order_manager.h/cc
    ├── matching_engine.h/cc
    ├── executor.h/cc
    └── trading_system.h/cc
```

## Performance Characteristics

- **Order Submission**: O(log n) for matching, O(1) for adding to book
- **Order Cancellation**: O(1)
- **Account Operations**: O(1)
- **Memory**: All data structures fit in memory (in-memory database)

## Future Enhancements

- Persistent storage (SQLite/PostgreSQL)
- Historical candle data (OHLCV)
- Advanced order types (stop-loss, bracket orders)
- Portfolio analytics and reporting
- Real-time price feeds from external APIs
- Order book snapshots and market depth
- Commission and fee handling
- Multi-threaded concurrent order processing
- REST API for external client access

## Testing

Basic demo functionality is included in `main.cc`. To run:

```bash
blaze run //:trading_system
```

This demonstrates:
- Creating users and accounts
- Setting market prices
- Placing and executing trades
- Viewing order books
- Account summaries

## License

Apache License 2.0
