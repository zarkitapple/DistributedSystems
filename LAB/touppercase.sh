#!/bin/sh
/home/mario/Escritorio/Third/DistributedSystems/jdk8u252-b09/bin/javac src/webserviceserver/*.java -d bin/webServiceServer
/home/mario/Escritorio/Third/DistributedSystems/jdk8u252-b09/bin/java -cp bin/webServiceServer src.webserviceserver.ToUpperCasePublisher
