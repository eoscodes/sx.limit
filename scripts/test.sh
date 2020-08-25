#!/bin/bash

cleos push action limit.sx setfee '[0]' -p limit.sx

# deposit tokens to wallet.sx
cleos transfer myaccount wallet.sx "3.0000 EOS"
cleos transfer toaccount wallet.sx "4.0000 USDT" --contract tethertether

# place limit order
cleos push action limit.sx place '["myaccount", {"contract": "eosio.token", "quantity": "1.0000 EOS"}, {"contract": "tethertether", "quantity": "4.0000 USDT"}]' -p myaccount
cleos push action limit.sx place '["myaccount", {"contract": "eosio.token", "quantity": "2.0000 EOS"}, {"contract": "tethertether", "quantity": "8.0000 USDT"}]' -p myaccount

# fill limit order
cleos push action limit.sx fill '["toaccount", 0]' -p toaccount

# cancel limit order
cleos push action limit.sx cancel '[1]' -p myaccount

# withdraw tokens from wallet.sx
cleos push action wallet.sx withdraw '["myaccount", "tethertether", "4.0000 USDT"]' -p myaccount
