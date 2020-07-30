GECKO_HOST='localhost:9652'

echo 'Getting Balance for "faucet"'
curl -X POST --data '{
  "jsonrpc":"2.0",
  "id"     : 1,
  "method" :"avm.getBalance",
  "params" :{
    "address":"X-6Y3kysjF9jnHnYkdS9yGAuoHyae2eNmeV",
    "assetID":"AVA"
  }
}' -H 'content-type:application/json;' $GECKO_HOST/ext/bc/X

echo 'Getting Balance for "test1"'
curl -X POST --data '{
  "jsonrpc":"2.0",
  "id"     : 1,
  "method" :"avm.getBalance",
  "params" :{
    "address":"X-3YDcRtaX9Voef9eD5wEDvBDUx9VsV6xhD",
    "assetID":"AVA"
  }
}' -H 'content-type:application/json;' $GECKO_HOST/ext/bc/X

echo 'Getting Balance for "test2"'
curl -X POST --data '{
  "jsonrpc":"2.0",
  "id"     : 1,
  "method" :"avm.getBalance",
  "params" :{
    "address":"X-JwFJV5deBqxho3s8FEUxkCSApFL4XK1ha",
    "assetID":"AVA"
  }
}' -H 'content-type:application/json;' $GECKO_HOST/ext/bc/X
