json = require "json"

accounts = {
  faucet = {
    username = "faucet",
    -- note that the avash_call parser is pretty brain-dead, so we can't have spaces in any json strings
    password = "good-cub-book",
    -- Private key for the default, pre-funded X-Chain account on local test networks:
    wallet = {{
      address = "X-6Y3kysjF9jnHnYkdS9yGAuoHyae2eNmeV",
      privateKey = "ewoqjP7PxY4yr3iLTpLisriqt94hdyDFNgchSxGGztUrTXtNN",
    }},
  },

  test1 = {
    username = "test1",
    password = "good-cub-book",
    wallet = {{
      address = "X-3YDcRtaX9Voef9eD5wEDvBDUx9VsV6xhD",
      privateKey = "2MuCQHXZgxnMNgDShBCK2MJ7WcghPKJU6GERN18mG3inZFZoe4",
      initialFunds = 20000,
    }},
  },

  test2 = {
    username = "test2",
    password = "good-cub-book",
    wallet = {{
      address = "X-JwFJV5deBqxho3s8FEUxkCSApFL4XK1ha",
      privateKey = "VDPdTm6a77KD3ATnwm3hMbzrvK8mvo9fzkQDYWyJ7oaC5g8fC",
      initialFunds = 30000,
    }},
  },
}

function account_credentials (account)
  return {
    username = account.username,
    password = account.password,
  }
end

function create_keystore_user (node, account)
  avash_call("callrpc " .. node .. " ext/keystore keystore.createUser " .. json.encode(account_credentials(account)) .. " st nid")
  for index, addrKey in ipairs(account.wallet) do
    cred = account_credentials(account)
    cred.privateKey = addrKey.privateKey
    avash_call("callrpc " .. node .. " ext/bc/X avm.importKey " .. json.encode(cred) .. " st nid")
  end
end

function create_keystore_users (node, accounts)
  for index, account in pairs(accounts) do
    create_keystore_user(node, account)
  end
end

function fund_addresses (node, faucet, addresses)
  for index, address in ipairs(addresses) do
    if address.address ~= nil and address.initialFunds ~= nil then
      params = account_credentials(faucet)
      params.assetID = "AVA"
      params.amount = address.initialFunds
      params.to = address.address
      avash_call("callrpc " .. node .. " ext/bc/X avm.send " .. json.encode(params) .. " st nid")
    end
  end
end

function fund_accounts (node, faucet, accounts)
  for index, account in pairs(accounts) do
    fund_addresses (node, faucet, account.wallet)
  end
end

create_keystore_users ("n1", accounts)
fund_accounts ("n1", accounts.faucet, accounts)
