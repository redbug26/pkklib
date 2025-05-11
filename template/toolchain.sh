#!/bin/bash

docker run --rm -v "${PWD}:/src/" -v "${PWD}/../pkklib/src:/src/pkklib" -it pico

