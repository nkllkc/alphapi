# REST API Documentation

The Alphapi trading system provides a comprehensive REST API for remote access to all trading operations.

## Base URL

```
http://localhost:8080/api
```

## Authentication

Currently no authentication. Future versions should implement API key or JWT authentication.

## Response Format

All responses are JSON with the following structure:

**Success Response:**
```json
{
  "success": true,
  "data": { ... }
}
```

**Error Response:**
```json
{
  "success": false,
  "error": "Error message"
}
```

## Endpoints

### Health Check

#### GET /api/health
Check if the server is running.

**Response:**
```json
{
  "status": "ok"
}
```

---

## User Endpoints

### Create User

#### POST /api/users
Create a new user.

**Request Body:**
```json
{
  "name": "Alice Johnson",
  "email": "alice@example.com"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "user_1",
    "name": "Alice Johnson",
    "email": "alice@example.com"
  }
}
```

### Get User

#### GET /api/users/:user_id
Get user details.

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "user_1",
    "name": "Alice Johnson",
    "email": "alice@example.com"
  }
}
```

---

## Account Endpoints

### Create Account

#### POST /api/accounts
Create a new account for a user.

**Request Body:**
```json
{
  "user_id": "user_1",
  "initial_cash": 100000.00
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "acc_1",
    "user_id": "user_1",
    "cash_balance": 100000.00
  }
}
```

### Get Account

#### GET /api/accounts/:user_id
Get account details for a user.

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "acc_1",
    "user_id": "user_1",
    "cash_balance": 100000.00
  }
}
```

### Get Balance

#### GET /api/accounts/:user_id/balance
Get current cash balance.

**Response:**
```json
{
  "success": true,
  "data": {
    "user_id": "user_1",
    "balance": 100000.00
  }
}
```

### Get Account Summary

#### GET /api/accounts/:user_id/summary
Get comprehensive account summary with positions and total value.

**Response:**
```json
{
  "success": true,
  "data": {
    "user_id": "user_1",
    "cash_balance": 75000.00,
    "positions": [
      {
        "symbol": "AAPL",
        "quantity": 100,
        "avg_cost": 150.25
      }
    ],
    "total_value": 90250.00
  }
}
```

---

## Order Endpoints

### Place Order

#### POST /api/orders
Place a new buy or sell order.

**Request Body:**
```json
{
  "user_id": "user_1",
  "symbol": "AAPL",
  "side": "BUY",
  "type": "LIMIT",
  "price": 150.25,
  "quantity": 100
}
```

**Parameters:**
- `side`: "BUY" or "SELL"
- `type`: "LIMIT" or "MARKET"
- `price`: Price per share (ignored for MARKET orders)
- `quantity`: Number of shares

**Response:**
```json
{
  "success": true,
  "data": {
    "order_id": "ord_1",
    "success": true,
    "filled_quantity": 50,
    "updated_balance": 92487.50,
    "trades": [
      {
        "id": "trade_1",
        "quantity": 50,
        "price": 150.00
      }
    ]
  }
}
```

### Get Order

#### GET /api/orders/:order_id
Get order details.

**Response:**
```json
{
  "success": true,
  "data": {
    "id": "ord_1",
    "user_id": "user_1",
    "symbol": "AAPL",
    "side": "BUY",
    "type": "LIMIT",
    "price": 150.25,
    "quantity": 100,
    "filled_quantity": 50
  }
}
```

### Get User Orders

#### GET /api/users/:user_id/orders
Get all orders for a user.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": "ord_1",
      "symbol": "AAPL",
      "side": "BUY",
      "quantity": 100,
      "filled_quantity": 50,
      "price": 150.25
    }
  ]
}
```

### Cancel Order

#### DELETE /api/orders/:order_id?user_id=user_1
Cancel a pending order.

**Query Parameters:**
- `user_id`: User ID (required for authorization)

**Response:**
```json
{
  "success": true,
  "data": {
    "order_id": "ord_1",
    "cancelled": true
  }
}
```

---

## Market Data Endpoints

### Set Quote

#### POST /api/quotes
Set or update a price quote for a symbol.

**Request Body:**
```json
{
  "symbol": "AAPL",
  "bid": 150.00,
  "ask": 150.50
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "symbol": "AAPL",
    "bid": 150.00,
    "ask": 150.50
  }
}
```

### Get Quote

#### GET /api/quotes/:symbol
Get current bid/ask prices for a symbol.

**Response:**
```json
{
  "success": true,
  "data": {
    "symbol": "AAPL",
    "bid": 150.00,
    "ask": 150.50
  }
}
```

### Get Order Book

#### GET /api/orderbook/:symbol
Get the full order book for a symbol (bids and asks).

**Response:**
```json
{
  "success": true,
  "data": {
    "symbol": "AAPL",
    "bids": [
      {
        "price": 150.25,
        "quantity": 50
      }
    ],
    "asks": [
      {
        "price": 150.50,
        "quantity": 100
      }
    ]
  }
}
```

---

## Position Endpoints

### Get Positions

#### GET /api/users/:user_id/positions
Get all positions (stock holdings) for a user.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "symbol": "AAPL",
      "quantity": 100,
      "avg_cost": 150.25
    },
    {
      "symbol": "MSFT",
      "quantity": 50,
      "avg_cost": 350.00
    }
  ]
}
```

---

## Trade Endpoints

### Get User Trades

#### GET /api/users/:user_id/trades
Get all trades executed for a user (as buyer or seller).

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": "trade_1",
      "symbol": "AAPL",
      "quantity": 50,
      "price": 150.00
    }
  ]
}
```

### Get All Trades

#### GET /api/trades
Get all trades executed on the exchange.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": "trade_1",
      "buyer_id": "user_1",
      "seller_id": "user_2",
      "symbol": "AAPL",
      "quantity": 50,
      "price": 150.00
    }
  ]
}
```

---

## Example Workflow

### 1. Create users
```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Alice","email":"alice@example.com"}'

curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Bob","email":"bob@example.com"}'
```

### 2. Create accounts
```bash
curl -X POST http://localhost:8080/api/accounts \
  -H "Content-Type: application/json" \
  -d '{"user_id":"user_1","initial_cash":100000}'

curl -X POST http://localhost:8080/api/accounts \
  -H "Content-Type: application/json" \
  -d '{"user_id":"user_2","initial_cash":50000}'
```

### 3. Set prices
```bash
curl -X POST http://localhost:8080/api/quotes \
  -H "Content-Type: application/json" \
  -d '{"symbol":"AAPL","bid":150.00,"ask":150.50}'
```

### 4. Place orders
```bash
curl -X POST http://localhost:8080/api/orders \
  -H "Content-Type: application/json" \
  -d '{"user_id":"user_1","symbol":"AAPL","side":"BUY","type":"LIMIT","price":150.25,"quantity":100}'

curl -X POST http://localhost:8080/api/orders \
  -H "Content-Type: application/json" \
  -d '{"user_id":"user_2","symbol":"AAPL","side":"SELL","type":"LIMIT","price":150.00,"quantity":50}'
```

### 5. Check account summary
```bash
curl http://localhost:8080/api/accounts/user_1/summary
```

### 6. View order book
```bash
curl http://localhost:8080/api/orderbook/AAPL
```

### 7. View trades
```bash
curl http://localhost:8080/api/trades
```

---

## Building and Running the REST Server

```bash
# Build the REST server
blaze build //:rest_server

# Run the server (default port 8080)
blaze run //:rest_server

# Run on custom port
blaze run //:rest_server -- 9000
```

The server will start and print available endpoints. Demo data will be automatically initialized.

---

## Error Handling

All errors return a 200 status with success=false and error message:

```json
{
  "success": false,
  "error": "Insufficient funds"
}
```

Common errors:
- "User not found"
- "Account not found"
- "Insufficient funds"
- "Insufficient shares"
- "Quote not found"
- "Order not found"

---

## Rate Limiting

Currently no rate limiting. Future versions should implement rate limiting per user/IP.

---

## Timeouts

Request timeout: 30 seconds (configurable)

---

## CORS

Currently CORS is not enabled. Enable by adding appropriate headers in rest_api.cc if needed.

---

## Future Enhancements

- API key authentication
- JWT token support
- WebSocket for real-time updates
- Rate limiting
- Request validation
- Comprehensive logging
- Metrics/monitoring endpoints
- Batch order operations
- Advanced order types
- Portfolio analytics endpoints
