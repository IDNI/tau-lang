#!/bin/bash

./parser/gen parser/sbf.tgf --header-only false
./parser/gen parser/tau.tgf --header-only false
./parser/gen parser/bitvector.tgf --header-only false
./parser/gen parser/adt.tgf --header-only false
