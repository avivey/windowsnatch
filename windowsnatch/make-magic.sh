#! /usr/bin/env bash

# usage: $0 '\011' '\036' '\037'

function set-title ()
{
    echo -ne "\e]2;$@\007"
}

while [[ -n $1 ]]
do
  set-title "target$1tegrat"
  read
  shift
done
