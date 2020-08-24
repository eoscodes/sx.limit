#include <eosio.token/eosio.token.hpp>
#include <wallet.sx/wallet.sx.hpp>

#include "limit.sx.hpp"

[[eosio::action]]
void limitSx::place( const name owner, const extended_asset bid, const extended_asset ask )
{
    require_auth( owner );

    // owner must send funds from wallet.sx => limit.sx
    walletSx::transfer_action transfer( "wallet.sx"_n, { get_self(), "active"_n });
    transfer.send( owner, get_self(), bid.contract, bid.quantity, "place" );

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
    walletSx::transfer_action transfer( "wallet.sx"_n, { get_self(), "active"_n });
    transfer.send( account, get_self(), itr.ask.contract, itr.ask.quantity, "fill" );
    transfer.send( get_self(), "wallet.sx"_n, itr.bid.contract, itr.bid.quantity, account.to_string() );

    // seller - return `ask` tokens back to owner account
    transfer.send( get_self(), "wallet.sx"_n, itr.ask.contract, itr.ask.quantity, itr.owner.to_string() );
    _orders.erase( itr );
}

[[eosio::action]]
void limitSx::cancel( const uint64_t id )
{
    limitSx::orders _orders( get_self(), get_self().value );
    const auto & itr = _orders.get( id, "order ID does not exist" );

    // only owner (creator) can cancel active limit order
    require_auth( itr.owner );
    walletSx::transfer_action transfer( "wallet.sx"_n, { get_self(), "active"_n });
    transfer.send( get_self(), "wallet.sx"_n, itr.bid.contract, itr.bid.quantity, itr.owner.to_string() );
    _orders.erase( itr );
}
