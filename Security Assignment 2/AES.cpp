#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
#define n 4
vector<int> Sbox, InvSbox, rcon;
vector<vector<int>> Mixer, InverseMixer, keys;
void initialize();
vector<int> g(vector<int> output, int round)
{
    // shift
    int temp = output[0];
    for (int i = 0; i < 3; ++i)
    {
        output[i] = output[i + 1];
    }
    output[3] = temp;

    // subBytes
    for (int i = 0; i < 4; ++i)
        output[i] = Sbox[output[i]];

    // add round constant
    output[0] ^= rcon[round];
    return output;
}

vector<int> Xor(vector<int> a, vector<int> b)
{
    for (int i = 0; i < n; i++)
        a[i] ^= b[i];
    return a;
}
void expandKeys(vector<vector<int>> given)
{
    for (int i = 0; i < n; i++)
        keys.push_back({given[0][i], given[1][i], given[2][i], given[3][i]});
    for (int step = 1; step <= 10; step++)
    {
        keys.push_back(Xor(keys[keys.size() - 4], g(keys.back(), step)));
        for (int i = 0; i < 3; i++)
            keys.push_back(Xor(keys[keys.size() - 4], keys.back()));
    }
    return;
}
void subBytes(vector<vector<int>> &v)
{
    for (auto &a : v)
        for (auto &i : a)
            i = Sbox[i];
    return;
}
void inverseSubBytes(vector<vector<int>> &v)
{
    for (auto &a : v)
        for (auto &i : a)
            i = InvSbox[i];
    return;
}
void shiftRows(vector<vector<int>> &v)
{
    for (int i = 0; i < n; i++)
        rotate(v[i].begin(), v[i].begin() + i, v[i].end());
    return;
}
void InverseShiftRows(vector<vector<int>> &v)
{
    for (int i = 1; i < n; i++)
        rotate(v[i].begin(), v[i].begin() + (n - i), v[i].end());
    return;
}

int galois_multiply(const unsigned char &a, const unsigned char &c)
{
    unsigned char b = c;
    unsigned char result = 0;
    unsigned char temp = a;
    for (int i = 0; i < 8; ++i)
    {
        if (b & 1)
        {
            result ^= temp;
        }
        bool carry = temp & 0x80;
        temp <<= 1;
        if (carry)
        {
            temp ^= 0x1B;
        }
        b = b >> 1;
    }
    return result;
}

auto MultiplyMatrix(vector<vector<int>> &a, vector<vector<int>> &b)
{
    vector<vector<int>> v(n, vector<int>(n, 0));
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            for (int k = 0; k < n; k++)
                v[r][c] ^= galois_multiply(a[r][k], b[k][c]);
    return v;
}
void mixCol(vector<vector<int>> &v)
{
    v = MultiplyMatrix(Mixer, v);
    return;
}
void inverseMixCol(vector<vector<int>> &v)
{
    v = MultiplyMatrix(InverseMixer, v);
    return;
}
vector<vector<int>> makeState(string s)
{
    int k = 0;
    vector<vector<int>> v(n, vector<int>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            v[j][i] = (int)(unsigned char)s[k++];
    return v;
}
vector<vector<int>> Xor(vector<vector<int>> a, vector<vector<int>> b)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            a[i][j] ^= b[i][j];
    return a;
}
void addRoundKey(vector<vector<int>> &v, int round)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            v[i][j] ^= keys[round * 4 + j][i];
    return;
}
string encrypt(string keyText, string textString)
{
    vector<vector<int>> key, plaintext;
    key = makeState(keyText);
    plaintext = makeState(textString);
    vector<vector<int>> cur_state = Xor(plaintext, key);

    for (int round = 1; round <= 10; round++)
    {
        subBytes(cur_state);
        shiftRows(cur_state);
        if (round != 10)
            mixCol(cur_state);
        addRoundKey(cur_state, round);
    }
    string cipherText;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cipherText.push_back((char)cur_state[j][i]);
    return cipherText;
}
string decrypt(string keyText, string textString)
{
    vector<vector<int>> key, cyphertext;
    key = makeState(keyText);
    cyphertext = makeState(textString);
    vector<vector<int>> cur_state = cyphertext;
    addRoundKey(cur_state, 10);
    for (int round = 9; round >= 0; round--)
    {
        InverseShiftRows(cur_state);
        inverseSubBytes(cur_state);
        addRoundKey(cur_state, round);
        if (round)
            inverseMixCol(cur_state);
    }
    string plainText;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            plainText.push_back((char)cur_state[j][i]);
    return plainText;
}
void print(string s, string to_print)
{
    cout << s << ":\n";
    cout << "In ASCII: " << to_print << "\n";
    cout << "In HEX: ";
    for (auto c : to_print)
        cout << hex << ((int)(unsigned char)c < 0x10 ? "0" : "") << (int)(unsigned char)c;
    cout << "\n\n";
}
int main()
{
    initialize();
    string key;
    string plaintext, cypherText, again_plainText;
    getline(cin, key);
    getline(cin, plaintext);
    auto start = high_resolution_clock::now();
    while (key.size() > 16)
        key.pop_back();
    while (key.size() < 16)
        key.push_back('Z');
    expandKeys(makeState(key));
    auto stop = high_resolution_clock::now();
    auto key_duration = duration_cast<microseconds>(stop - start);

    start = high_resolution_clock::now();
    for (int i = 0; i < plaintext.size(); i += 16)
    {
        string tmp_plainText = plaintext.substr(i, min(16, (int)plaintext.size() - i));
        while (tmp_plainText.size() < 16)
            tmp_plainText.push_back('Z');
        cypherText += encrypt(key, tmp_plainText);
    }
    stop = high_resolution_clock::now();
    auto encryption_duration = duration_cast<microseconds>(stop - start);

    start = high_resolution_clock::now();
    for (int i = 0; i < plaintext.size(); i += 16)
    {
        string tmp_cypherText = cypherText.substr(i, min(16, (int)cypherText.size() - i));
        again_plainText += decrypt(key, tmp_cypherText);
    }
    stop = high_resolution_clock::now();
    auto decryption_duration = duration_cast<microseconds>(stop - start);
    cypherText = cypherText.substr(0, plaintext.size());
    again_plainText = again_plainText.substr(0, plaintext.size());
    cout << plaintext.size() << " " << cypherText.size() << " " << again_plainText.size() << "\n";
    print("Key", key);
    print("Plain Text", plaintext);
    print("Cipher Text", cypherText);
    print("Decipher Text", again_plainText);

    cout << "Execution Time:\n";
    cout << fixed << setprecision(15) << "Key Scheduling: " << key_duration.count() / 1000.0 << " seconds"
         << "\n";
    cout << fixed << setprecision(15) << "Encryption Time: " << encryption_duration.count() / 1000.0 << " seconds"
         << "\n";
    cout << fixed << setprecision(15) << "Decryption Time: " << decryption_duration.count() / 1000.0 << " seconds"
         << "\n";
    return 0;
}

void initialize()
{
    rcon = vector<int>{0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};
    Mixer = vector<vector<int>>{
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}};
    InverseMixer = vector<vector<int>>{
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e}};
    Sbox = vector<int>{
        0x63,
        0x7C,
        0x77,
        0x7B,
        0xF2,
        0x6B,
        0x6F,
        0xC5,
        0x30,
        0x01,
        0x67,
        0x2B,
        0xFE,
        0xD7,
        0xAB,
        0x76,
        0xCA,
        0x82,
        0xC9,
        0x7D,
        0xFA,
        0x59,
        0x47,
        0xF0,
        0xAD,
        0xD4,
        0xA2,
        0xAF,
        0x9C,
        0xA4,
        0x72,
        0xC0,
        0xB7,
        0xFD,
        0x93,
        0x26,
        0x36,
        0x3F,
        0xF7,
        0xCC,
        0x34,
        0xA5,
        0xE5,
        0xF1,
        0x71,
        0xD8,
        0x31,
        0x15,
        0x04,
        0xC7,
        0x23,
        0xC3,
        0x18,
        0x96,
        0x05,
        0x9A,
        0x07,
        0x12,
        0x80,
        0xE2,
        0xEB,
        0x27,
        0xB2,
        0x75,
        0x09,
        0x83,
        0x2C,
        0x1A,
        0x1B,
        0x6E,
        0x5A,
        0xA0,
        0x52,
        0x3B,
        0xD6,
        0xB3,
        0x29,
        0xE3,
        0x2F,
        0x84,
        0x53,
        0xD1,
        0x00,
        0xED,
        0x20,
        0xFC,
        0xB1,
        0x5B,
        0x6A,
        0xCB,
        0xBE,
        0x39,
        0x4A,
        0x4C,
        0x58,
        0xCF,
        0xD0,
        0xEF,
        0xAA,
        0xFB,
        0x43,
        0x4D,
        0x33,
        0x85,
        0x45,
        0xF9,
        0x02,
        0x7F,
        0x50,
        0x3C,
        0x9F,
        0xA8,
        0x51,
        0xA3,
        0x40,
        0x8F,
        0x92,
        0x9D,
        0x38,
        0xF5,
        0xBC,
        0xB6,
        0xDA,
        0x21,
        0x10,
        0xFF,
        0xF3,
        0xD2,
        0xCD,
        0x0C,
        0x13,
        0xEC,
        0x5F,
        0x97,
        0x44,
        0x17,
        0xC4,
        0xA7,
        0x7E,
        0x3D,
        0x64,
        0x5D,
        0x19,
        0x73,
        0x60,
        0x81,
        0x4F,
        0xDC,
        0x22,
        0x2A,
        0x90,
        0x88,
        0x46,
        0xEE,
        0xB8,
        0x14,
        0xDE,
        0x5E,
        0x0B,
        0xDB,
        0xE0,
        0x32,
        0x3A,
        0x0A,
        0x49,
        0x06,
        0x24,
        0x5C,
        0xC2,
        0xD3,
        0xAC,
        0x62,
        0x91,
        0x95,
        0xE4,
        0x79,
        0xE7,
        0xC8,
        0x37,
        0x6D,
        0x8D,
        0xD5,
        0x4E,
        0xA9,
        0x6C,
        0x56,
        0xF4,
        0xEA,
        0x65,
        0x7A,
        0xAE,
        0x08,
        0xBA,
        0x78,
        0x25,
        0x2E,
        0x1C,
        0xA6,
        0xB4,
        0xC6,
        0xE8,
        0xDD,
        0x74,
        0x1F,
        0x4B,
        0xBD,
        0x8B,
        0x8A,
        0x70,
        0x3E,
        0xB5,
        0x66,
        0x48,
        0x03,
        0xF6,
        0x0E,
        0x61,
        0x35,
        0x57,
        0xB9,
        0x86,
        0xC1,
        0x1D,
        0x9E,
        0xE1,
        0xF8,
        0x98,
        0x11,
        0x69,
        0xD9,
        0x8E,
        0x94,
        0x9B,
        0x1E,
        0x87,
        0xE9,
        0xCE,
        0x55,
        0x28,
        0xDF,
        0x8C,
        0xA1,
        0x89,
        0x0D,
        0xBF,
        0xE6,
        0x42,
        0x68,
        0x41,
        0x99,
        0x2D,
        0x0F,
        0xB0,
        0x54,
        0xBB,
        0x16,
    };
    InvSbox = vector<int>{
        0x52,
        0x09,
        0x6A,
        0xD5,
        0x30,
        0x36,
        0xA5,
        0x38,
        0xBF,
        0x40,
        0xA3,
        0x9E,
        0x81,
        0xF3,
        0xD7,
        0xFB,
        0x7C,
        0xE3,
        0x39,
        0x82,
        0x9B,
        0x2F,
        0xFF,
        0x87,
        0x34,
        0x8E,
        0x43,
        0x44,
        0xC4,
        0xDE,
        0xE9,
        0xCB,
        0x54,
        0x7B,
        0x94,
        0x32,
        0xA6,
        0xC2,
        0x23,
        0x3D,
        0xEE,
        0x4C,
        0x95,
        0x0B,
        0x42,
        0xFA,
        0xC3,
        0x4E,
        0x08,
        0x2E,
        0xA1,
        0x66,
        0x28,
        0xD9,
        0x24,
        0xB2,
        0x76,
        0x5B,
        0xA2,
        0x49,
        0x6D,
        0x8B,
        0xD1,
        0x25,
        0x72,
        0xF8,
        0xF6,
        0x64,
        0x86,
        0x68,
        0x98,
        0x16,
        0xD4,
        0xA4,
        0x5C,
        0xCC,
        0x5D,
        0x65,
        0xB6,
        0x92,
        0x6C,
        0x70,
        0x48,
        0x50,
        0xFD,
        0xED,
        0xB9,
        0xDA,
        0x5E,
        0x15,
        0x46,
        0x57,
        0xA7,
        0x8D,
        0x9D,
        0x84,
        0x90,
        0xD8,
        0xAB,
        0x00,
        0x8C,
        0xBC,
        0xD3,
        0x0A,
        0xF7,
        0xE4,
        0x58,
        0x05,
        0xB8,
        0xB3,
        0x45,
        0x06,
        0xD0,
        0x2C,
        0x1E,
        0x8F,
        0xCA,
        0x3F,
        0x0F,
        0x02,
        0xC1,
        0xAF,
        0xBD,
        0x03,
        0x01,
        0x13,
        0x8A,
        0x6B,
        0x3A,
        0x91,
        0x11,
        0x41,
        0x4F,
        0x67,
        0xDC,
        0xEA,
        0x97,
        0xF2,
        0xCF,
        0xCE,
        0xF0,
        0xB4,
        0xE6,
        0x73,
        0x96,
        0xAC,
        0x74,
        0x22,
        0xE7,
        0xAD,
        0x35,
        0x85,
        0xE2,
        0xF9,
        0x37,
        0xE8,
        0x1C,
        0x75,
        0xDF,
        0x6E,
        0x47,
        0xF1,
        0x1A,
        0x71,
        0x1D,
        0x29,
        0xC5,
        0x89,
        0x6F,
        0xB7,
        0x62,
        0x0E,
        0xAA,
        0x18,
        0xBE,
        0x1B,
        0xFC,
        0x56,
        0x3E,
        0x4B,
        0xC6,
        0xD2,
        0x79,
        0x20,
        0x9A,
        0xDB,
        0xC0,
        0xFE,
        0x78,
        0xCD,
        0x5A,
        0xF4,
        0x1F,
        0xDD,
        0xA8,
        0x33,
        0x88,
        0x07,
        0xC7,
        0x31,
        0xB1,
        0x12,
        0x10,
        0x59,
        0x27,
        0x80,
        0xEC,
        0x5F,
        0x60,
        0x51,
        0x7F,
        0xA9,
        0x19,
        0xB5,
        0x4A,
        0x0D,
        0x2D,
        0xE5,
        0x7A,
        0x9F,
        0x93,
        0xC9,
        0x9C,
        0xEF,
        0xA0,
        0xE0,
        0x3B,
        0x4D,
        0xAE,
        0x2A,
        0xF5,
        0xB0,
        0xC8,
        0xEB,
        0xBB,
        0x3C,
        0x83,
        0x53,
        0x99,
        0x61,
        0x17,
        0x2B,
        0x04,
        0x7E,
        0xBA,
        0x77,
        0xD6,
        0x26,
        0xE1,
        0x69,
        0x14,
        0x63,
        0x55,
        0x21,
        0x0C,
        0x7D,
    };
}
