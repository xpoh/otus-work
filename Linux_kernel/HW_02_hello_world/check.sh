#!/bin/bash

str="hello world from script"

param_set() {
    local param="$1"
    local value="$2"
    local pstr="/sys/module/work_module/parameters/$param"

    echo "Setting $param to '$value'"

    sudo bash -c "echo -n '$value' > '$pstr'"

    if [ $? -ne 0 ]; then
        echo "WARNING: Failed to set $param"
    fi
}

param_get() {
    local param="$1"
    local pstr="/sys/module/work_module/parameters/$param"

    if [ ! -f "$pstr" ]; then
        echo "ERROR: $pstr not found"
        return 1
    fi

    cat "$pstr"
}

for ((i=0; i<${#str}; i++)); do
    val="${str:i:1}"
    idx="$i"

    param_set idx "$idx"
    echo "idx=$(param_get idx)"

    param_set ch_val "$val"
    echo "ch_val=$(param_get ch_val)"
done

echo ""

param_get my_str

echo ""

echo "try set my_str directly and wait error"
param_set my_str "hello"