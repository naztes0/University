
# RC6 Block Cipher Implementation

## Overview
RC6 is a symmetric-key block cipher derived from RC5, designed by a group of people to meet the requirements of the Advanced Encryption Standard (AES) competition. 

The implemented version is RC6-32/20/b, which operates on 128-bit blocks (sixteen bytes), uses 32-bit words, and features 20 rounds of encryption/decryption. 

## Algorithm Workflow
The implementation of RC6 consists of the following primary stages:
1. Expanding the Key Schedule to generate 44 subkeys.
2. Padding the original message to fit 128-bit block boundaries.
3. Parsing the data into manageable 16-byte blocks and 32-bit words.
4. Executing the core encryption/decryption loop across 20 rounds.
5. Assembling the final ciphertext or plaintext.

## Algorithm step-by-step

### Step 1: Key Schedule Expansion
The algorithm requires $2r + 4$ subkeys (44 subkeys for 20 rounds) to ensure every step of the encryption uses a unique cryptographic state.
* The user's variable-length byte key is converted into an array of 32-bit words using Little-Endian byte ordering.
* An array of 44 subkeys is initialized using two mathematical constants ($P_{32}$ and $Q_{32}$)
* The user's key and the subkey array are mixed together over a loop. 

### Step 2: Message Padding
Because RC6 is a block cipher, the input data must be a perfect multiple of 128 bits (16 bytes). 
* The implementation uses the **PKCS#7** padding standard.
* If a block is incomplete, it is padded with bytes whose value equals the number of missing bytes (e.g., if 4 bytes are missing, four `0x04` bytes are appended).
* If the original message is exactly a multiple of 16 bytes, an entire new block of 16 bytes (each holding `0x10`) is added to ensure unambiguous unpadding during decryption.

### Step 3: Block Initialization and Pre-whitening
The padded byte array is divided into 16-byte chunks.
* Each 16-byte block is parsed into four 32-bit integers (working registers `A`, `B`, `C`, and `D`) using Little-Endian order.
* **Pre-whitening:** Before the main rounds begin, registers `B` and `D` are modified by adding the first two subkeys from the expanded key schedule.

### Step 4: The Core Rounds (20 Iterations)
This is the cryptographic engine of RC6, running identically for 20 rounds.
* During each round, registers `B` and `D` are passed through a quadratic function: $f(x) = x(2x + 1) \lll 5$.
* **Data-Dependent Rotations:** Registers `A` and `C` are XORed with the quadratic results, and then cyclically shifted (rotated). The exact number of bits they are rotated by is dictated by the current data inside registers `D` and `B`. This dynamic shifting is RC6's signature security feature.
* At the end of the round, the registers are cyclically swapped in parallel (`A` becomes `B`, `B` becomes `C`, etc.), ensuring all data fragments interact with each other in subsequent rounds.

### Step 5: Post-whitening and Assembly
Once all 20 rounds are completed, the block undergoes final modifications.
* **Post-whitening:** Registers `A` and `C` are modified by adding the final two subkeys from the schedule.
* The four 32-bit registers are converted back into a 16-byte array (Little-Endian) and appended to the final output stream. 
* For decryption, the unpadding method inspects the last byte of the stream to seamlessly slice off the PKCS#7 padding, returning the exact original plaintext.

