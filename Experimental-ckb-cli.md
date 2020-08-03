## Experimental CKB-CLI features
A few of the examples in the README.md reflect a version of ckb-cli that is not the one contained in `nix/dep/ckb-cli/git.json`. For
using the ledger with the version contained in this repo, see the following instructions.

Note that the experimental features on Obsidian's fork of the ckb-cli client should only be used for development and testing purposes.

### Message Signing

To sign the hash of a message with their ledger a user may do the following (Note the recoverable flag is optional):

```sh
CKB> util sign-message --message "hello world i love nervos" --from-account <my-ledger-account> --recoverable
message-hash: <blake2b hash of: magic_bytes + message>
recoverable: false
signature: <signature>
```
If a message is longer than 64 characters the ledger will display the first 61 chars, followed by an ellipsis (`...`)
The ckb-cli accepts utf8 strings in its `--utf8-string` argument, but the ledger can not display all chars. If the ledger comes accross a
character that it is unnable to display it will display an asterisk (`\*`) instead.

Prompts on the Ledger device are as follows:

|  Prompt 1 	|   Prompt 2  	|
|:---------:	|:-----------:	|
|   `Sign`  	|  `Message`  	|
| `Message` 	| `<message>` 	|

One can verfiy the signature as follows:

```bash
CKB> util verify-signature --message "hello world i love nervos" --from-account <my-ledger-account> --signature <signature from above>
pubkey: <pubkey of my ledger's account root>
recoverable: <signature type>
verify-ok: true
```

A user may also switch out the `--message` option with a `--binary-hex` option, and the ledger will display
the message as hex, instead of ascii characters
```
CKB> util sign-message --binary-hex '0x61' --from-account <my-ledger-account> 
```

### Signing a Hash ###
The ledger also allows for the signing of a pre-hashed message. Due to the potential security risks of this option, a user must "opt-in" to this
setting by going into the `Configuration` section of the app, and setting `Allow sign hash` to `On`.

Note that the current implementation of the `ledger-sign-hash` util has a verification system built-in, and, as such, only accepts
blake2b hashes that use the Nervos personalization. A user can use `util blake2b` to get a hash of this sort. The ledger itself does not have
this constraint.
```
CKB> util ledger-sign-hash --binary-hex <message hash> --from-account <my-ledger-account> --path <optional derivation path>
```
The ledger will display the following:

|    Prompt 1    	|   Prompt 2  	|
|:--------------:	|:-----------:	|
|     `Sign`     	|  `Message`  	|
| `Message Hash` 	| `<message>` 	|

Expected output from terminal:
```
VERIFY: true 
message-hash: <binary hex passed in>
signature: <signature>
```
It is not possible to verify the signature with `util verify signature` (this is why the verification is built in to `ledger-sign-hash`) because `verify-signature` assumes that
additional magic-bytes were prepended to the message passed in, and that the ledger signature is of the hash of the message passed in (with the magic-bytes), whereas
`ledger-sign-hash` simply sends the input to the ledger untouched (and the ledger does not hash the input before signing it).

If the length of the hash is greater than 64 bytes, or if the `Allow sign hash` is set to `Off`, the ledger will reject the message.
