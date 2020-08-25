// #pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("limit.sx")]] limitSx : public contract {

public:
    using contract::contract;

   /**
     * ## TABLE `settings`
     *
     * - `{int64_t} fee` - trading fee (pips 1/100 of 1%)
     * - `{uint64_t} next_id` - next order ID counter
     *
     * ### example
     *
     * ```json
     * {
     *   "fee": 0,
     *   "next_id": 0
     * }
     * ```
     */
    struct [[eosio::table("settings")]] params {
        int64_t             fee;
        uint64_t            next_id;
    };
    typedef eosio::singleton< "settings"_n, params > settings;

    /**
     * ## TABLE `docs`
     *
     * - `{string} url` - Documentation url
     *
     * ### example
     *
     * ```json
     * {
     *   "url": "https://github.com/stableex/sx.limit"
     * }
     * ```
     */
    struct [[eosio::table("docs")]] docs_row {
        string      url = "https://github.com/stableex/sx.limit";
    };
    typedef eosio::singleton< "docs"_n, docs_row > docs;

    /**
     * ## TABLE `orders`
     *
     * - ram_payer: `get_self()`
     *
     * ### multi-indexes
     *
     * | `param`        | `index_position` | `key_type` |
     * |----------------|------------------|------------|
     * | `byowner`      | 2                | i64        |
     * | `bytimestamp`  | 3                | i64        |
     *
     * ### params
     *
     * - `{uint64_t} id` - order id
     * - `{name} owner` - owner account name which placed order
     * - `{time_point_sec} timestamp` - placed order timestamp
     * - `{extended_asset} bid` - bid amount
     * - `{extended_asset} ask` - ask amount
     *
     * ### Example - cleos
     *
     * ```bash
     * $ cleos get table limit.sx limit.sx orders
     * ```
     *
     * ### Example - json
     *
     * ```json
     * {
     *     "id": 123,
     *     "owner": "myaccount",
     *     "timestamp": "2020-08-23T15:17:23",
     *     "bid": {"contract":"eosio.token", "quantity": "1.0000 EOS"},
     *     "ask": {"contract":"tethertether", "quantity": "3.0000 USDT"}
     * }
     * ```
     */
    struct [[eosio::table("orders")]] orders_row {
        uint64_t            id;
        name                owner;
        time_point_sec      timestamp;
        extended_asset      bid;
        extended_asset      ask;

        uint64_t primary_key() const { return id; }
        uint64_t by_owner() const { return owner.value; }
        uint64_t by_timestamp() const { return timestamp.sec_since_epoch(); }
    };
    typedef eosio::multi_index< "orders"_n, orders_row,
        indexed_by<"byowner"_n, const_mem_fun<orders_row, uint64_t, &orders_row::by_owner>>,
        indexed_by<"bytimestamp"_n, const_mem_fun<orders_row, uint64_t, &orders_row::by_timestamp>>
    > orders;

    /**
     * ## ACTION `place`
     *
     * Place limit order
     *
     * - **authority**: `owner`
     *
     * ### params
     *
     * - `{name} owner` - owner account to place limit order
     * - `{extended_asset} bid` - bid amount
     * - `{extended_asset} ask` - ask amount
     *
     * ### Example - cleos
     *
     * ```bash
     * cleos push action limit.sx place '["myaccount", \
     *   {"contract": "eosio.token", "quantity": "1.0000 EOS"}, \
     *   {"contract": "tethertether", "quantity": "3.0000 USDT"} ]' -p myaccount
     * ```
     *
     * ### Example - smart contract
     *
     * ```c++
     * const name owner = "myaccount"_n;
     * const extended_asset bid = extended_asset{asset{10000, symbol{"EOS", 4}}, "eosio.token"_n};
     * const extended_asset ask = extended_asset{asset{30000, symbol{"USDT", 4}}, "tethertether"_n};
     *
     * limitSx::place_action place( "limit.sx"_n, { owner, "active"_n });
     * place.send( owner, bid, ask );
     * ```
     */
    [[eosio::action]]
    void place( const name owner, const extended_asset bid, const extended_asset ask );

    /**
     * ## ACTION `cancel`
     *
     * Fill limit order
     *
     * - **authority**: `account`
     *
     * ### params
     *
     * - `{name} account` - account to fill limit order
     * - `{uint64_t} id` - limit order `id` to cancel
     *
     * ### Example - cleos
     *
     * ```bash
     * cleos push action limit.sx fill '["toaccount", 123]' -p toaccount
     * ```
     *
     * ### Example - smart contract
     *
     * ```c++
     * limitSx::fill_action fill( "limit.sx"_n, { "toaccount"_n, "active"_n });
     * fill.send( "toaccount"_n, 123 );
     * ```
     */
    [[eosio::action]]
    void fill( const name account, const uint64_t id );

    /**
     * ## ACTION `cancel`
     *
     * Cancel limit order
     *
     * - **authority**: `owner`
     *
     * ### params
     *
     * - `{uint64_t} id` - limit order `id` to cancel
     *
     * ### Example - cleos
     *
     * ```bash
     * cleos push action limit.sx cancel '[123]' -p myaccount
     * ```
     *
     * ### Example - smart contract
     *
     * ```c++
     * limitSx::cancel_action cancel( "limit.sx"_n, { "myaccount"_n, "active"_n });
     * cancel.send( 123 );
     * ```
     */
    [[eosio::action]]
    void cancel( const uint64_t id );

    [[eosio::action]]
    void setfee( const int64_t fee );

    // action wrappers
    using place_action = eosio::action_wrapper<"place"_n, &limitSx::place>;
    using fill_action = eosio::action_wrapper<"fill"_n, &limitSx::fill>;
    using cancel_action = eosio::action_wrapper<"cancel"_n, &limitSx::cancel>;

// private:

};
