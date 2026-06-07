# WebSocket Streaming API Documentation

The Alphapi trading system provides real-time streaming of market data and order updates via WebSocket connections.

## Connection

### WebSocket URL
```
ws://localhost:8081/ws
```

### Connecting

**JavaScript Example:**
```javascript
const ws = new WebSocket("ws://localhost:8081/ws");

ws.onopen = (event) => {
  console.log("Connected to market data stream");
};

ws.onmessage = (event) => {
  const message = JSON.parse(event.data);
  console.log("Received:", message);
};

ws.onerror = (error) => {
  console.error("WebSocket error:", error);
};

ws.onclose = () => {
  console.log("Disconnected from stream");
};
```

## Message Format

All messages are JSON objects with a `type` field indicating the message category.

### Generic Message Structure
```json
{
  "type": "message_type",
  "data": { ... }
}
```

## Server Messages (Received)

### Welcome Message
Sent immediately upon connection.

```json
{
  "type": "welcome",
  "client_id": "client_1",
  "message": "Connected to Alphapi Market Data Stream"
}
```

### Quote Update
Real-time bid/ask price updates for subscribed symbols.

```json
{
  "type": "quote",
  "symbol": "AAPL",
  "bid": 150.25,
  "ask": 150.75,
  "timestamp": 1717824600
}
```

**Trigger:** When a price quote is updated on the exchange and you are subscribed to the symbol.

### Subscription Confirmation
Confirmation that you've subscribed to a symbol's updates.

```json
{
  "type": "subscribed",
  "symbol": "AAPL"
}
```

### Unsubscription Confirmation
Confirmation that you've unsubscribed from a symbol.

```json
{
  "type": "unsubscribed",
  "symbol": "AAPL"
}
```

### Order Update
Real-time notification of order status changes.

```json
{
  "type": "order_update",
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
```

**Trigger:** Sent to the user when their order is placed and matched.

### Trade Notification
Notification of executed trades involving the user.

```json
{
  "type": "trade",
  "trade_id": "trade_1",
  "symbol": "AAPL",
  "quantity": 50,
  "price": 150.00,
  "side": "BUY"
}
```

**Trigger:** When a trade is executed and you are the buyer or seller.

### Authentication Confirmation
Confirmation that you've been authenticated as a user.

```json
{
  "type": "authenticated",
  "user_id": "user_1"
}
```

### Error Message
Error response to invalid requests.

```json
{
  "type": "error",
  "error": "Invalid request format",
  "code": "INVALID_REQUEST"
}
```

## Client Messages (Sent)

### Subscribe to Symbol
Subscribe to price updates for a specific symbol.

```json
{
  "type": "subscribe",
  "symbol": "AAPL"
}
```

### Unsubscribe from Symbol
Stop receiving price updates for a symbol.

```json
{
  "type": "unsubscribe",
  "symbol": "AAPL"
}
```

### Authenticate User
Authenticate as a user to receive personal order/trade updates.

```json
{
  "type": "authenticate",
  "user_id": "user_1"
}
```

## REST Endpoints for WebSocket Testing

Since WebSocket implementation varies, these REST endpoints can be used to test the streaming functionality:

### Subscribe to Symbol

**POST /api/subscribe**
```bash
curl -X POST http://localhost:8081/api/subscribe \
  -H "Content-Type: application/json" \
  -d '{
    "client_id": "client_1",
    "symbol": "AAPL"
  }'
```

**Response:**
```json
{
  "success": true,
  "data": {
    "type": "subscribe_response",
    "status": "subscribed",
    "symbol": "AAPL"
  }
}
```

### Broadcast Quote Update

**POST /api/stream/quote**
Broadcast a price update to all subscribers of a symbol.

```bash
curl -X POST http://localhost:8081/api/stream/quote \
  -H "Content-Type: application/json" \
  -d '{
    "symbol": "AAPL",
    "bid": 150.25,
    "ask": 150.75
  }'
```

**Response:**
```json
{
  "success": true,
  "data": {
    "type": "quote",
    "symbol": "AAPL",
    "bid": 150.25,
    "ask": 150.75
  }
}
```

### List Connected Clients

**GET /api/stream/clients**
Get list of all connected WebSocket clients and their subscriptions.

```bash
curl http://localhost:8081/api/stream/clients
```

**Response:**
```json
{
  "connected_clients": 2,
  "clients": [
    {
      "client_id": "client_1",
      "user_id": "user_1",
      "subscribed_symbols": ["AAPL", "MSFT"]
    },
    {
      "client_id": "client_2",
      "user_id": "anonymous",
      "subscribed_symbols": ["GOOGL"]
    }
  ]
}
```

### WebSocket Health Check

**GET /ws/health**
Check if WebSocket server is running.

```bash
curl http://localhost:8081/ws/health
```

**Response:**
```json
{
  "status": "ok",
  "service": "websocket_server"
}
```

## Usage Examples

### Example 1: Subscribe to Stock Prices

```javascript
// Connect
const ws = new WebSocket("ws://localhost:8081/ws");

ws.onopen = () => {
  // Subscribe to AAPL price updates
  ws.send(JSON.stringify({
    type: "subscribe",
    symbol: "AAPL"
  }));
};

ws.onmessage = (event) => {
  const message = JSON.parse(event.data);
  
  if (message.type === "quote") {
    console.log(`${message.symbol} Bid: $${message.bid} Ask: $${message.ask}`);
  }
  
  if (message.type === "subscribed") {
    console.log(`Successfully subscribed to ${message.symbol}`);
  }
};
```

### Example 2: Track Personal Orders

```javascript
const ws = new WebSocket("ws://localhost:8081/ws");

ws.onopen = () => {
  // Authenticate as user
  ws.send(JSON.stringify({
    type: "authenticate",
    user_id: "user_1"
  }));
};

ws.onmessage = (event) => {
  const message = JSON.parse(event.data);
  
  if (message.type === "order_update") {
    console.log(`Order ${message.order_id} filled: ${message.filled_quantity} shares`);
    console.log(`New balance: $${message.updated_balance}`);
  }
  
  if (message.type === "trade") {
    console.log(`Trade executed: ${message.side} ${message.quantity} ${message.symbol} @ $${message.price}`);
  }
};
```

### Example 3: Multi-Symbol Monitoring

```javascript
const ws = new WebSocket("ws://localhost:8081/ws");
const prices = {};

ws.onopen = () => {
  const symbols = ["AAPL", "MSFT", "GOOGL", "TSLA"];
  
  symbols.forEach(symbol => {
    ws.send(JSON.stringify({
      type: "subscribe",
      symbol: symbol
    }));
  });
};

ws.onmessage = (event) => {
  const message = JSON.parse(event.data);
  
  if (message.type === "quote") {
    prices[message.symbol] = {
      bid: message.bid,
      ask: message.ask,
      spread: message.ask - message.bid
    };
    
    console.log("Current Market:", prices);
  }
};
```

### Example 4: Combined REST + WebSocket

```javascript
// Use REST to place order
async function placeOrder() {
  const response = await fetch("http://localhost:8080/api/orders", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      user_id: "user_1",
      symbol: "AAPL",
      side: "BUY",
      type: "LIMIT",
      price: 150.25,
      quantity: 100
    })
  });
  
  const result = await response.json();
  console.log("Order placed:", result.data.order_id);
}

// Use WebSocket to track updates
const ws = new WebSocket("ws://localhost:8081/ws");

ws.onopen = () => {
  ws.send(JSON.stringify({
    type: "authenticate",
    user_id: "user_1"
  }));
  
  placeOrder();
};

ws.onmessage = (event) => {
  const message = JSON.parse(event.data);
  
  if (message.type === "order_update") {
    console.log("Order update received:", message);
  }
};
```

## Subscription Patterns

### Market Data Subscriptions
- **Subscribe to multiple symbols** - Get price updates for basket of stocks
- **Symbol-specific feeds** - High-frequency updates for actively traded symbols
- **Market-wide feeds** - All symbols (future enhancement)

### Personal Subscriptions
- **User authentication** - Receive your order updates
- **Trade notifications** - Real-time trade confirmations
- **Portfolio updates** - Position and balance changes

## Message Flow Diagram

```
Client                                WebSocket Server

  |                                        |
  |---------- CONNECT (WS) ------------->  |
  |                                        |
  |<------- WELCOME MESSAGE -----------   |
  |                                        |
  |------- SUBSCRIBE (AAPL) ----------->   |
  |                                        |
  |<----- SUBSCRIBED (AAPL) -----------    |
  |                                        |
  |                                   (quote updates happen)
  |                                        |
  |<------- QUOTE UPDATE (AAPL) -------    |
  |  {bid: 150.25, ask: 150.75}            |
  |                                        |
  |<------- QUOTE UPDATE (AAPL) -------    |
  |  {bid: 150.50, ask: 151.00}            |
  |                                        |
  |------ AUTHENTICATE (user_1) ----->     |
  |                                        |
  |<----- AUTHENTICATED (user_1) ------    |
  |                                        |
  |                                   (order placed via REST)
  |                                        |
  |<------- ORDER UPDATE ---------------   |
  |  {order_id: ord_1, filled: 50}         |
  |                                        |
  |<------- TRADE NOTIFICATION ---------   |
  |  {side: BUY, qty: 50, price: 150}      |
```

## Performance Notes

- **Quote updates:** Broadcast to all symbol subscribers
- **Order updates:** Sent only to authenticated user
- **Trade notifications:** Sent to buyer and seller
- **Message rate:** Depends on order activity and price updates

## Limitations & Future Enhancements

**Current Limitations:**
- Simplified WebSocket implementation (uses REST polling underneath)
- No authentication/authorization
- No message compression
- No order book depth streaming

**Future Enhancements:**
- Full WebSocket support with persistent connections
- JWT-based authentication
- Order book snapshots (top N levels)
- Candle data streaming (OHLCV)
- Portfolio performance updates
- News feed integration
- Message compression (msgpack, protobuf)
- Connection fallback (HTTP long-polling)
- Client library (TypeScript/JavaScript)

## Troubleshooting

### Connection Issues
- Ensure WebSocket server is running: `GET http://localhost:8081/ws/health`
- Check firewall allows port 8081
- Verify WebSocket URL format: `ws://` (not `http://`)

### Not Receiving Updates
- Confirm subscription was acknowledged: check for `subscribed` message
- Verify price updates are being broadcast: `GET /api/stream/clients`
- Check authentication for personal updates: send `authenticate` message

### Lost Connection
- Implement reconnection logic with exponential backoff
- Resubscribe to symbols after reconnecting
- Re-authenticate if needed
