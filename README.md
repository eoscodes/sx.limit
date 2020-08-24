# SX `Limit` 📊 - EOSIO Smart Contract

High frequency single entry limit orderbook exchange

## Quickstart

```bash
# deposit tokens to wallet.sx
cleos transfer myaccount wallet.sx "1.0000 EOS"

# place limit order
cleos push action limit.sx place '["myaccount", \
    {"contract": "eosio.token", "quantity": "1.0000 EOS"}, \
    {"contract": "tethertether", "quantity": "3.0000 USDT"} ]' -p myaccount

# fill limit order
cleos push action limit.sx fill '["toaccount", 123]' -p toaccount

# cancel limit order
cleos push action limit.sx cancel '[123]' -p myaccount

# withdraw tokens from wallet.sx
cleos push action wallet.sx withdraw '["myaccount", "tethertether", "3.0000 USDT"]' -p myaccount
```

## Table of Content

- [TABLE `orders`](#table-orders)
- [ACTION `place`](#action-place)
- [ACTION `fill`](#action-fill)
- [ACTION `cancel`](#action-cancel)

## TABLE `orders`

- ram_payer: `get_self()`

### multi-indexes

| `param`        | `index_position` | `key_type` |
|----------------|------------------|------------|
| `byowner`      | 2                | i64        |
| `bytimestamp`  | 3                | i64        |

### params

- `{uint64_t} id` - order id
- `{name} owner` - owner account name which placed order
- `{time_point_sec} timestamp` - placed order timestamp
- `{extended_asset} bid` - bid amount
- `{extended_asset} ask` - ask amount

### Example - cleos

```bash
$ cleos get table limit.sx limit.sx orders
```

### Example - json

```json
{
    "id": 123,
    "owner": "myaccount",
    "timestamp": "2020-08-23T15:17:23",
    "bid": {"contract":"eosio.token", "quantity": "1.0000 EOS"},
    "ask": {"contract":"tethertether", "quantity": "3.0000 USDT"}
}
```

## ACTION `place`

Place limit order

- **authority**: `owner`

### params

- `{name} owner` - owner account to place limit order
- `{extended_asset} bid` - bid amount
- `{extended_asset} ask` - ask amount

### Example - cleos

```bash
cleos push action limit.sx place '["myaccount", \
{"contract": "eosio.token", "quantity": "1.0000 EOS"}, \
{"contract": "tethertether", "quantity": "3.0000 USDT"} ]' -p myaccount
```

### Example - smart contract

```c++
const name owner = "myaccount"_n;
const extended_asset bid = extended_asset{asset{10000, symbol{"EOS", 4}}, "eosio.token"_n};
const extended_asset ask = extended_asset{asset{30000, symbol{"USDT", 4}}, "tethertether"_n};

limitSx::place_action place( "limit.sx"_n, { owner, "active"_n });
place.send( owner, bid, ask );
```

## ACTION `cancel`

Fill limit order

- **authority**: `account`

### params

- `{name} account` - account to fill limit order
- `{uint64_t} id` - limit order `id` to cancel

### Example - cleos

```bash
cleos push action limit.sx fill '["toaccount", 123]' -p toaccount
```

### Example - smart contract

```c++
limitSx::fill_action fill( "limit.sx"_n, { "toaccount"_n, "active"_n });
fill.send( "toaccount"_n, 123 );
```

## ACTION `cancel`

Cancel limit order

- **authority**: `owner`

### params

- `{uint64_t} id` - limit order `id` to cancel

### Example - cleos

```bash
cleos push action limit.sx cancel '[123]' -p myaccount
```

### Example - smart contract

```c++
limitSx::cancel_action cancel( "limit.sx"_n, { "myaccount"_n, "active"_n });
cancel.send( 123 );
```