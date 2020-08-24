#!/bin/bash

eosio-cpp limit.sx.cpp -I ../sx.wallet/include -I ../eosio.token/include
cleos set contract limit.sx . limit.sx.wasm limit.sx.abi
