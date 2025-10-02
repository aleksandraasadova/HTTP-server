#!/bin/bash

for i in {1..10}; do
  curl http://localhost:4221/echo/request_$i &
done
wait