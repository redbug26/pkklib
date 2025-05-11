#!/bin/bash

docker run --rm -v "${PWD}:/src/" -v "${PWD}/../pkklib:/src/pkklib" -it pico

