#!/bin/bash

eosio-cpp limit.sx.cpp -I ..
cleos set contract limit.sx . limit.sx.wasm limit.sx.abi
