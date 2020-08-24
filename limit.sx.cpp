#include <eosio.token/eosio.token.hpp>
#include <sx.wallet/wallet.sx.hpp>

#include "limit.sx.hpp"

[[eosio::action]]
void limitSx::place( const name owner, const extended_asset bid, const extended_asset ask )
{
    require_auth( owner );

    // owner must send funds from wallet.sx => limit.sx
    walletSx::move_action move( "wallet.sx"_n, { "wallet.sx"_n, "active"_n });
    move.send( owner, get_self(), bid.contract, bid.quantity, "place" );

    // create order
    limitSx::orders _orders( get_self(), get_self().value );
    _orders.emplace( owner, [&]( auto& row ) {
        row.id          = _orders.available_primary_key();
        row.owner       = owner;
        row.timestamp   = current_time_point();
        row.bid         = bid;
        row.ask         = ask;
    });
}

[[eosio::action]]
void limitSx::fill( const name account, const uint64_t id )
{
    require_auth( account );

    limitSx::orders _orders( get_self(), get_self().value );
    const auto & itr = _orders.get( id, "order ID does not exist" );

    // buyer - send ask tokens to limit
    walletSx::move_action move( "wallet.sx"_n, { "wallet.sx"_n, "active"_n });
    move.send( account, get_self(), itr.ask.contract, itr.ask.quantity, "fill" );
    move.send( get_self(), "wallet.sx"_n, itr.bid.contract, itr.bid.quantity, account.to_string() );

    // seller - return `ask` tokens back to owner account
    move.send( get_self(), "wallet.sx"_n, itr.ask.contract, itr.ask.quantity, itr.owner.to_string() );
    _orders.erase( itr );
}

[[eosio::action]]
void limitSx::cancel( const uint64_t id )
{
    limitSx::orders _orders( get_self(), get_self().value );
    const auto & itr = _orders.get( id, "order ID does not exist" );

    // only owner (creator) can cancel active limit order
    require_auth( itr.owner );
    walletSx::move_action move( "wallet.sx"_n, { "wallet.sx"_n, "active"_n });
    move.send( get_self(), "wallet.sx"_n, itr.bid.contract, itr.bid.quantity, itr.owner.to_string() );
    _orders.erase( itr );
}
