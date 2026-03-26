// import java.util.Arrays;

public class RC6Algo {

    public RC6Algo(byte[] key) {
        generateKeyArr(key);
    }

    // Basic parametrs
    private int w = 32; // bits in a word
    private int r = 20; // number of rounds in algo
    private int u = w / 8; // bytes in a word

    // Formula: P = Odd((e-2)*2^w)
    private static final int P = 0xb7e15163;
    // Formula: Q = Odd((phi-1)*2^w)
    private static final int Q = 0x9e3779b9;

    private int[] S;

    private int fx(int x) {
        return Integer.rotateLeft(x * (2 * x + 1), 5);
    }

    private void generateKeyArr(byte[] key) {
        int c = (key.length + 3) / u;
        int[] L = new int[c];
        // Print to check results
        // System.out.print("Key in HEX: [ ");
        // for (byte b : key) {
        // System.out.printf(" %02X", b);
        // if (b != key[key.length - 1])
        // System.out.print(",");
        // }
        // System.out.println(" ]");

        for (int i = 0; i < key.length; i++) {
            L[i / 4] |= (key[i] & 0xFF) << (8 * (i % 4));
            // System.out.printf("\nCell %d: %02X ", i / 4, key[i] & 0xFF);
        }

        // System.out.println("\nArray L: " + Arrays.toString(L));

        S = new int[2 * r + 4];
        S[0] = P;
        for (int i = 1; i < S.length; i++) {
            S[i] = S[i - 1] + Q;
        }
        int A = 0;
        int B = 0;
        int i = 0;
        int j = 0;

        int v = 3 * Math.max(c, S.length);
        for (int k = 0; k < v; k++) {
            A = S[i] = Integer.rotateLeft((S[i] + A + B), 3);
            B = L[j] = Integer.rotateLeft((L[j] + A + B), (A + B));
            i = (i + 1) % (2 * r + 4);
            j = (j + 1) % c;

        }
    }

    private int bytesToInt(byte[] src, int offset) {
        return (src[offset] & 0xFF) |
                ((src[offset + 1] & 0xFF) << 8) |
                ((src[offset + 2] & 0xFF) << 16) |
                ((src[offset + 3] & 0xFF) << 24);
    }

    private void intToBytes(int word, byte[] dst, int offset) {
        dst[offset] = (byte) word;
        dst[offset + 1] = (byte) (word >>> 8);
        dst[offset + 2] = (byte) (word >>> 16);
        dst[offset + 3] = (byte) (word >>> 24);
    }

    public byte[] encryptBlock(byte[] in) {
        byte[] out = new byte[16];
        int A = bytesToInt(in, 0);
        int B = bytesToInt(in, 4);
        int C = bytesToInt(in, 8);
        int D = bytesToInt(in, 12);

        B = B + S[0];
        D = D + S[1];
        for (int i = 1; i <= r; i++) {
            int t = fx(B);
            int u = fx(D);
            A = Integer.rotateLeft((A ^ t), u) + S[2 * i];
            C = Integer.rotateLeft((C ^ u), t) + S[2 * i + 1];
            int tempA = A;
            A = B;
            B = C;
            C = D;
            D = tempA;
        }

        A = A + S[2 * r + 2];
        C = C + S[2 * r + 3];
        intToBytes(A, out, 0);
        intToBytes(B, out, 4);
        intToBytes(C, out, 8);
        intToBytes(D, out, 12);

        return out;

    }

    public byte[] decryptBlock(byte[] in) {
        byte[] out = new byte[16];

        int A = bytesToInt(in, 0);
        int B = bytesToInt(in, 4);
        int C = bytesToInt(in, 8);
        int D = bytesToInt(in, 12);

        C = C - S[2 * r + 3];
        A = A - S[2 * r + 2];
        for (int i = r; i >= 1; i--) {
            int tempA = A;
            A = D;
            D = C;
            C = B;
            B = tempA;

            int u = fx(D);
            int t = fx(B);
            C = Integer.rotateRight((C - S[2 * i + 1]), t) ^ u;
            A = Integer.rotateRight((A - S[2 * i]), u) ^ t;
        }

        D = D - S[1];
        B = B - S[0];
        intToBytes(A, out, 0);
        intToBytes(B, out, 4);
        intToBytes(C, out, 8);
        intToBytes(D, out, 12);

        return out;
    }

    public byte[] encrypt(byte[] plaintext) {
        byte[] padded = pad(plaintext);
        byte[] ciphertext = new byte[padded.length];

        for (int i = 0; i < padded.length; i += 16) {
            byte[] block = new byte[16];
            System.arraycopy(padded, i, block, 0, 16);

            byte[] encryptedBlock = encryptBlock(block);

            System.arraycopy(encryptedBlock, 0, ciphertext, i, 16);
        }
        return ciphertext;
    }

    // Головний метод розшифрування
    public byte[] decrypt(byte[] ciphertext) {
        byte[] paddedPlaintext = new byte[ciphertext.length];

        for (int i = 0; i < ciphertext.length; i += 16) {
            byte[] block = new byte[16];
            System.arraycopy(ciphertext, i, block, 0, 16);

            byte[] decryptedBlock = decryptBlock(block);

            System.arraycopy(decryptedBlock, 0, paddedPlaintext, i, 16);
        }
        return unpad(paddedPlaintext);
    }

    private byte[] pad(byte[] data) {
        int padLength = 16 - (data.length % 16);
        byte[] padded = new byte[data.length + padLength];
        System.arraycopy(data, 0, padded, 0, data.length);
        for (int i = data.length; i < padded.length; i++) {
            padded[i] = (byte) padLength;
        }
        return padded;
    }

    private byte[] unpad(byte[] data) {
        int padLength = data[data.length - 1];
        byte[] unpadded = new byte[data.length - padLength];
        System.arraycopy(data, 0, unpadded, 0, unpadded.length);
        return unpadded;
    }
}
