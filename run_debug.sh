#!/bin/sh

make
sudo make install
sudo filebench -f workloads/test.f