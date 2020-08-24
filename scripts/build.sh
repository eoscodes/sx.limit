#!/bin/bash

eosio-cpp limit.sx.cpp -I include
cleos set contract limit.sx . limit.sx.wasm limit.sx.abi

# eosio-cpp examples/basic.cpp -I include
# cleos set contract basic . basic.wasm basic.abi
