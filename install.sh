#!/bin/sh

echo "> Installing the 'stdlib':"
cd stdlib
make setup
make
sudo mkdir -p /usr/share/thalia
sudo cp -r lib/ /usr/share/thalia/stdlib/
cd ..

echo "> Installing the executable:"
lein test
lein uberjar
rm ./target/uberjar/thalia
cat stub.sh ./target/uberjar/thalia-0.0.0-standalone.jar > ./target/uberjar/thalia
chmod +x ./target/uberjar/thalia
sudo mv ./target/uberjar/thalia /bin/thalia

