#!/bin/bash

# unlock wallet
cleos wallet unlock --password $(cat ~/eosio-wallet/.pass)

# create accounts
cleos create account eosio limit.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio wallet.sx EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio tethertether EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio myaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio toaccount EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

# deploy
cleos set contract limit.sx . limit.sx.wasm limit.sx.abi
cleos set contract wallet.sx ../sx.wallet wallet.sx.wasm wallet.sx.abi
cleos set contract eosio.token ../eosio.token eosio.token.wasm eosio.token.abi
cleos set contract tethertether ../eosio.token eosio.token.wasm eosio.token.abi

# permission
cleos set account permission limit.sx active --add-code
cleos set account permission wallet.sx active limit.sx --add-code
cleos set account permission wallet.sx active --add-code

# create EOS token
cleos push action eosio.token create '["eosio", "100000000.0000 EOS"]' -p eosio.token
cleos push action eosio.token issue '["eosio", "5000000.0000 EOS", "init"]' -p eosio
cleos transfer eosio myaccount "50000.0000 EOS" "init"

# create USDT token
cleos push action tethertether create '["tethertether", "100000000.0000 USDT"]' -p tethertether
cleos push action tethertether issue '["tethertether", "5000000.0000 USDT", "init"]' -p tethertether
cleos transfer tethertether myaccount "50000.0000 USDT" "init" --contract tethertether
cleos transfer tethertether toaccount "50000.0000 USDT" "init" --contract tethertether
