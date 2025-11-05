# Sui Transaction Builder Server

Backend server để build fresh Sui transactions cho ESP32 wallet.

## Tại sao cần server này?

Arduino/ESP32 **KHÔNG thể** tự build Sui transactions vì:
- BCS encoding quá phức tạp
- Thiếu memory và CPU power
- MicroSui library chỉ support prebuilt transaction bytes

→ **Giải pháp**: Server build transactions, ESP32 fetch qua HTTP!

## Setup

### 1. Install Dependencies (đã làm rồi)
```bash
cd tx-server
npm install
```

### 2. Chạy Server
```bash
node server.mjs
```

Server sẽ chạy trên: `http://0.0.0.0:3000`

### 3. Cấu hình ESP32

Trong file `ESP32S3_Squareline_UI.ino`, update:
```cpp
const char* TX_SERVER_URL = "http://YOUR_COMPUTER_IP:3000";
```

**Lấy IP của máy tính**:
```bash
# Linux/Mac
hostname -I

# hoặc
ifconfig | grep "inet " | grep -v 127.0.0.1
```

Ví dụ: `http://192.168.1.7:3000`

## API Endpoints

### GET /build-transaction

Build simple transfer transaction (split và transfer to self)

**Parameters**:
- `amount` (optional) - Số MIST (1 SUI = 1,000,000,000 MIST), default: 1,000,000
- `recipient` (optional) - Địa chỉ nhận, default: sender address (self)

**Example**:
```bash
# Transfer 0.001 SUI to self
curl http://localhost:3000/build-transaction?amount=1000000

# Transfer 0.005 SUI to another address
curl "http://localhost:3000/build-transaction?amount=5000000&recipient=0xRECIPIENT"
```

**Response**:
```json
{
  "success": true,
  "txBytes": "000002000840420f...",
  "details": {
    "sender": "0xb0bd...",
    "recipient": "0xb0bd...",
    "amount": 1000000,
    "amountSUI": "0.001000000",
    "network": "testnet",
    "timestamp": "2025-11-02T..."
  }
}
```

### POST /build-custom-transaction

Build custom transaction với JSON body

**Body**:
```json
{
  "recipient": "0xRECIPIENT_ADDRESS",
  "amount": 5000000,
  "memo": "Test transfer"
}
```

## Workflow với ESP32

### Button 1 - Offline Sign
- Dùng hardcoded `TRANSACTION_BYTES`
- Chỉ generate signature
- Không cần server

### Button 2 - Dynamic Sign & Execute
1. **Fetch** fresh transaction từ server
2. **Sign** offline (generate signature)
3. **Execute** online (send to blockchain)

→ Mỗi lần click = Fresh transaction = Luôn thành công! ✨

## Customize Transactions

Edit `server.mjs` để thay đổi transaction type:

```javascript
// Example: Different transfer amounts
const [coin] = tx.splitCoins(tx.gas, [5_000_000]); // 0.005 SUI

// Example: Call smart contract
tx.moveCall({
  target: '0xPACKAGE::module::function',
  arguments: [...]
});
```

## Troubleshooting

**ESP32 không connect được server?**
- Check IP address của máy tính
- Server có đang chạy không? (`node server.mjs`)
- ESP32 và máy tính cùng WiFi network?
- Firewall có block port 3000?

**Transaction failed?**
- Check balance của sender address
- Verify sender address trong server.mjs match với ESP32

## Production Notes

⚠️ **Server này chỉ dùng cho development/testing!**

Để production:
- Add authentication/API key
- Deploy lên cloud (Heroku, Railway, Vercel...)
- Use HTTPS
- Add rate limiting
- Không hardcode private key trong ESP32!

## Logs

Server sẽ log mỗi request:
```
📦 Building new transaction...
  Amount: 1000000 MIST
  Recipient: 0xb0bd...
✓ Transaction built successfully!
  Bytes length: 292 bytes
```
