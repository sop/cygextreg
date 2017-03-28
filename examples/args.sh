#!/bin/bash

argv=("$@")
argc=${#argv[@]}
for (( i=0; i<$argc; i++ )); do
    arg="${argv[$i]}"
    printf 'argument #%d: %s\n' "$i" "$arg"
    if [[ -e "$arg" ]]; then
        stat "$arg"
    fi
    echo
done

exit 1
