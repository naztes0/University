# SHA-256 Hash Function Implementation

## Overview
SHA-2 (Secure Hash Algorithm 2) is a family of cryptographic hash functions to ensure data integrity and security. 

SHA-256 is a specific algorithm within this family that processes input data of any size and produces a fixed-size 256-bit (32-byte) hash. It operates on 32-bit words and is used in modern cryptography, including digital signatures, secure communications (TLS/SSL), and blockchain technologies.

## Algorithm Workflow
The implementation of SHA-256 consists of the following primary stages:
1. Padding the original message.
2. Parsing the padded message into 512-bit blocks and 32-bit words.
3. Expanding the message schedule from 16 to 64 words.
4. Executing the compression function across 64 rounds.
5. Assembling the final hash value.

## Algorithm step-by-step

### Step 1: Message Padding
The algorithm requires the input data to be a perfect multiple of 512 bits (64 bytes). 
* A single `1` bit (represented as the byte `0x80`) is appended immediately after the original message.
* `0` bits are appended until the length of the message is exactly 64 bits short of a 512-bit multiple.
* The last 64 bits (8 bytes) are filled with the exact length of the original message in bits.

### Step 2: Parsing into Blocks and Words
The padded byte array is divided into manageable blocks.
* The data is processed in blocks of 512 bits.
* Each 512-bit block is further divided into an array of sixteen 32-bit integers (words).

### Step 3: Message Schedule Expansion
For each 512-bit block, the algorithm requires 64 words to complete its 64 rounds of hashing.
* The initial 16 words are copied directly from the parsed block.
* The remaining 48 words are generated dynamically using a combination of logical functions (s0 and s1), bitwise right rotations, right shifts, and modular addition of previous words.

### Step 4: The Compression Loop (64 Rounds)
The core mathematical engine of SHA-256. 
* Before the loop starts, 8 working variables (`a` through `h`) are initialized using the current global hash state values.
* The algorithm runs a strict 64-iteration loop. Inside this loop, the working variables are highly mutated using specific logical operations: `Choose` (Ch), `Majority` (Maj), and two `S` functions.
* During each round, the mutations are influenced by the current word from the expanded message schedule and a specific pre-calculated fractional constant (`K`).
* After the loop finishes, the mutated variables are added back to the global hash state variables, ensuring a cumulative cryptographic avalanche effect.

### Step 5: Final Hash 
Once all 512-bit blocks of the message have been processed through the compression loop, the final 8 state variables are concatenated. They are converted into an unbroken 64-character string, representing the final SHA-256 hash.
