#!/bin/bash

export SONAR_SCANNER_VERSION=4.4.0.2170
export SONAR_SCANNER_HOME=$HOME/.sonar/sonar-scanner-$SONAR_SCANNER_VERSION-linux
curl --create-dirs -sSLo $HOME/.sonar/sonar-scanner.zip https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-$SONAR_SCANNER_VERSION-linux.zip
unzip -o $HOME/.sonar/sonar-scanner.zip -d $HOME/.sonar/
export PATH=$SONAR_SCANNER_HOME/bin:$PATH
export SONAR_SCANNER_OPTS="-server"

curl --create-dirs -sSLo $HOME/.sonar/build-wrapper-linux-x86.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
unzip -o $HOME/.sonar/build-wrapper-linux-x86.zip -d $HOME/.sonar/
export PATH=$HOME/.sonar/build-wrapper-linux-x86:$PATH

# Setup the build system
  - mkdir build
  - cd build
  - cmake ..
  - cd ..

# Build inside the build-wrapper

build-wrapper-linux-x86-64 --out-dir bw-output cmake --build build/

# Run sonar scanner (here, arguments are passed through the command line but most of them can be written in the sonar-project.properties file)
sonar-scanner \
  -Dsonar.organization=proofofconcept \
  -Dsonar.projectKey=AkumalexTheExalted_proof_of_concept \
  -Dsonar.sources=. \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io
