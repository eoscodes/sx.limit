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

    // tables
    limitSx::orders _orders( get_self(), get_self().value );
    limitSx::settings _settings( get_self(), get_self().value );
    auto settings = _settings.get_or_create( get_self() );

    // create place order
    _orders.emplace( owner, [&]( auto& row ) {
        row.id          = settings.next_id;
        row.owner       = owner;
        row.timestamp   = current_time_point();
        row.bid         = bid;
        row.ask         = ask;
    });

    // bump next available order ID
    settings.next_id += 1;
    _settings.set( settings, get_self() );
}

[[eosio::action]]
void limitSx::fill( const name account, const uint64_t id )
{
    require_auth( account );

    limitSx::orders _orders( get_self(), get_self().value );
    const auto & itr = _orders.get( id, "order ID does not exist" );

    // static actions
    walletSx::move_action move( "wallet.sx"_n, { "wallet.sx"_n, "active"_n });
    token::transfer_action transfer_bid( itr.bid.contract, { get_self(), "active"_n });
    token::transfer_action transfer_ask( itr.ask.contract, {{ "wallet.sx"_n, "active"_n }, { get_self(), "active"_n }});
    // including `wallet.sx` as additional permission_level allows to bypass `open` balance RAM requirements

    // buyer - send ask tokens to limit
    move.send( account, get_self(), itr.ask.contract, itr.ask.quantity, "fill" );
    transfer_bid.send( get_self(), account, itr.bid.quantity, "fill" );

    // seller - return `ask` tokens back to owner account
    transfer_ask.send( get_self(), "wallet.sx"_n, itr.ask.quantity, itr.owner.to_string() );
    _orders.erase( itr );
}

[[eosio::action]]
void limitSx::cancel( const uint64_t id )
{
    limitSx::orders _orders( get_self(), get_self().value );
    const auto & itr = _orders.get( id, "order ID does not exist" );

    // static actions
    token::transfer_action transfer_bid( itr.bid.contract, { get_self(), "active"_n });

    // only owner (creator) can cancel active limit order
    require_auth( itr.owner );
    transfer_bid.send( get_self(), itr.owner, itr.bid.quantity, "cancel" );
    _orders.erase( itr );
}

[[eosio::action]]
void limitSx::setfee( const int64_t fee )
{
    require_auth( get_self() );

    limitSx::settings _settings( get_self(), get_self().value );
    limitSx::docs _docs( get_self(), get_self().value );

    // create docs
    _docs.get_or_create( get_self() );

    check( fee <= 300, "fee cannot be greater than 3%");
    check( fee >= 0, "fee must be positive");

    // set fee
    auto settings = _settings.get_or_create( get_self() );
    settings.fee = fee;
    _settings.set( settings, get_self() );
}