#!/bin/bash

# deposit tokens to wallet.sx
cleos transfer myaccount wallet.sx "1.0000 EOS"

# place limit order
cleos push action limit.sx place '["myaccount", \
    {"contract": "eosio.token", "quantity": "1.0000 EOS"}, \
    {"contract": "tethertether", "quantity": "3.0000 USDT"} ]' -p myaccount

# fill limit order
cleos push action limit.sx fill '["toaccount", 1]' -p toaccount

# cancel limit order
cleos push action limit.sx cancel '[1]' -p myaccount

# withdraw tokens from wallet.sx
cleos push action wallet.sx withdraw '["myaccount", "tethertether", "3.0000 USDT"]' -p myaccount