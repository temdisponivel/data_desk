#!/bin/bash
clang -Wall -g -fPIC -shared -I ../../source/ ./features_demo.c -o features_demo.so
../../build/data_desk --custom ./features_demo.so ./data.ds

