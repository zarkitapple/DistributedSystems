#!/bin/sh
javac  source/*.java
java source/client -s localhost -p 6200
