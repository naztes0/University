
import java.nio.charset.StandardCharsets;

public class ShaAlgo {
    private String message;
    private static final int[] H_init = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    private static final int[] K = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    private int[] H;

    public ShaAlgo(String message) {
        this.message = message;
    }

    public String hashFunc() {

        this.H = new int[8];
        System.arraycopy(H_init, 0, this.H, 0, 8);

        byte[] originalData = textToByte(this.message);
        byte[] paddedData = padMessage(originalData);
        int[] allWords = bitsToWords(paddedData);

        for (int i = 0; i < allWords.length; i += 16) {

            int[] currentBlock = new int[16];
            System.arraycopy(allWords, i, currentBlock, 0, 16);
            processBlock(currentBlock);
        }

        StringBuilder hexString = new StringBuilder();
        for (int i = 0; i < 8; i++) {
            hexString.append(String.format("%08x", H[i]));
        }

        return hexString.toString();
    }

    private byte[] textToByte(String text) {
        return text.getBytes(StandardCharsets.UTF_8);
    }

    private byte[] padMessage(byte[] data) {
        int dataLength = data.length;

        long dataLengthBits = (long) dataLength * 8;
        int minLength = dataLength + 1 + 8;
        int numBlocks = (minLength + 63) / 64;
        int newLength = numBlocks * 64;

        byte[] paddedData = new byte[newLength];
        System.arraycopy(data, 0, paddedData, 0, dataLength);
        paddedData[dataLength] = (byte) 0x80;
        for (int i = 0; i < 8; i++) {
            paddedData[newLength - 1 - i] = (byte) (dataLengthBits >>> (8 * i));
        }
        return paddedData;
    }

    private int[] bitsToWords(byte[] data) {
        int[] words = new int[data.length / 4];

        for (int i = 0; i < words.length; i++) {
            words[i] = ((data[i * 4] & 0xFF) << 24) |
                    ((data[i * 4 + 1] & 0xFF) << 16) |
                    ((data[i * 4 + 2] & 0xFF) << 8) |
                    (data[i * 4 + 3] & 0xFF);
        }
        return words;
    }

    private int rotr(int x, int num) {
        return Integer.rotateRight(x, num);
    }

    private int ch(int x, int y, int z) {
        return (x & y) ^ (~x & z);
    }

    private int maj(int x, int y, int z) {
        return (x & y) ^ (x & z) ^ (y & z);

    }

    private int S0(int x) {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }

    private int S1(int x) {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }

    private int s0(int x) {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >>> 3);
    }

    private int s1(int x) {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >>> 10);
    }

    private void processBlock(int[] words) {
        int[] W = new int[64];
        System.arraycopy(words, 0, W, 0, 16);

        for (int i = 16; i < 64; i++) {
            W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];
        }

        int a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];

        for (int i = 0; i < 64; i++) {
            int temp1 = h + S1(e) + ch(e, f, g) + K[i] + W[i];
            int temp2 = S0(a) + maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;

    }
}