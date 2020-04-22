#!/bin/sh
/home/mario/Escritorio/Third/DistributedSystems/jdk8u252-b09/bin/javac src/clientjava/*.java -d bin/clientJava
/home/mario/Escritorio/Third/DistributedSystems/jdk8u252-b09/bin/java -cp .:bin/clientJava src.clientjava.client -s localhost -p 5000
