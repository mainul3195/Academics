#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
int n = 4;
vector<int> Sbox, InvSbox, rcon;
vector<vector<int>> Mixer, InverseMixer, keys;
void initialize();
vector<int> g(vector<int> output, int round);
vector<int> Xor(vector<int> a, vector<int> b);
void expandKeys(vector<vector<int>> given);
void subBytes(vector<vector<int>> &v);
void inverseSubBytes(vector<vector<int>> &v);
void shiftRows(vector<vector<int>> &v);
void InverseShiftRows(vector<vector<int>> &v);
int galois_multiply(const unsigned char &a, const unsigned char &c);
auto MultiplyMatrix(vector<vector<int>> &a, vector<vector<int>> &b);
void mixCol(vector<vector<int>> &v);
void inverseMixCol(vector<vector<int>> &v);
vector<vector<int>> makeState(string s);
vector<vector<int>> Xor(vector<vector<int>> a, vector<vector<int>> b);
void addRoundKey(vector<vector<int>> &v, int round);
string encrypt(string keyText, string textString);
string decrypt(string keyText, string textString);
void print(string s, string to_print);
long long mul_mod(long long x, long long y, long long m);
long long pow_mod(long long x, long long n, long long m);
bool miller_robin(long long n);
long long generate_random_prime(long long lower_limit, long long upper_limit);
pair<long long, long long> generate_primes(int k);
__int128_t egcd(__int128_t a, __int128_t b, __int128_t &x, __int128_t &y);
void print(__int128 x);
string to_string(__int128 x);
string multiply(const string &num1, const string &num2);
string subtract(const string &num1, const string &num2);
int compare(const string &num1, const string &num2);
string divideByTwo(const string &bigInteger);
string calc_mod(string a, string m);
string bigmod(string a, string b, string m);

int main()
{
    initialize();
    string key;
    string plaintext, cypherText, again_plainText;
    cout << "Enter Key: ";
    getline(cin, key);
    cout << "Enter Plaintext: ";
    getline(cin, plaintext);
    while (key.size() > 16)
        key.pop_back();
    while (key.size() < 16)
        key.push_back('Z');
    expandKeys(makeState(key));

    for (int i = 0; i < plaintext.size(); i += 16)
    {
        string tmp_plainText = plaintext.substr(i, min(16, (int)plaintext.size() - i));
        while (tmp_plainText.size() < 16)
            tmp_plainText.push_back('Z');
        cypherText += encrypt(key, tmp_plainText);
    }
    fstream ct("Don't Open This/CT.txt");
    for (auto c : cypherText)
        ct << (int)(unsigned char)c << "\n";

    int k = 32;
    auto [p, q] = generate_primes(k);
    __int128_t n = (__int128_t)p * q;
    __int128_t phi_n = (__int128_t)(p - 1) * (q - 1);
    long long low = 1;
    long long high = min((__int128_t)100000000000000000LL, phi_n);
    long long e;
    do
    {
        e = generate_random_prime(low, high);
    } while (phi_n % e == 0);

    __int128_t x, y;
    egcd(e, phi_n, x, y);
    __int128_t d = (x % phi_n + phi_n) % phi_n;

    string E = to_string(e);
    string D = to_string(d);
    string N = to_string(n);

    string plainText = key;
    vector<string> encrypted;
    for (auto p : plainText)
    {
        string P = to_string((int)p);
        string tmp = bigmod(P, E, N);
        encrypted.push_back(tmp);
    }
    fstream puk("Don't Open This/PUK.txt");
    puk << E << "\n"
        << N << "\n";
    fstream prk("Don't Open This/PRK.txt");
    prk << D << "\n"
        << N << "\n";
    fstream ek("Don't Open This/EK.txt");
    for (auto cypher : encrypted)
        ek << cypher << "\n";

    cout << "All the information are successfully stored in specific files.";
    return 0;
}

long long mul_mod(long long x, long long y, long long m)
{
    long long res = __int128(x) * y % m;
    return res;
}
long long pow_mod(long long x, long long n, long long m)
{
    long long res = 1 % m;
    for (; n; n >>= 1)
    {
        if (n & 1)
            res = mul_mod(res, x, m);
        x = mul_mod(x, x, m);
    }
    return res;
}
bool miller_robin(long long n)
{
    if (n <= 2 || (n & 1 ^ 1))
        return (n == 2);
    long long c, d, s = 0, r = n - 1;
    for (; !(r & 1); r >>= 1, s++)
    {
    }
    long long primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
    for (int i = 0; primes[i] < n && primes[i] < 32; i++)
    {
        c = pow_mod(primes[i], r, n);
        for (int j = 0; j < s; j++)
        {
            d = mul_mod(c, c, n);
            if (d == 1 && c != 1 && c != (n - 1))
                return false;
            c = d;
        }
        if (c != 1)
            return false;
    }
    return true;
}

long long generate_random_prime(long long lower_limit, long long upper_limit)
{
    long long random_number;
    do
    {
        random_number = rand() % (upper_limit - lower_limit) + lower_limit;
    } while (!miller_robin(random_number));

    return random_number;
}

pair<long long, long long> generate_primes(int k)
{
    int num_digits = floor(k / 2 / 3.322 + 1e-9) + 1;
    long long lower_limit = static_cast<long long>(pow(10, num_digits - 1));
    long long upper_limit = static_cast<long long>(pow(10, num_digits));

    long long prime1 = generate_random_prime(lower_limit, upper_limit);
    long long prime2;
    do
    {
        prime2 = generate_random_prime(lower_limit, upper_limit);
    } while (prime2 == prime1);
    return (pair<long long, long long>){prime1, prime2};
}
__int128_t egcd(__int128_t a, __int128_t b, __int128_t &x, __int128_t &y)
{
    if (b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }
    __int128_t x1, y1;
    __int128_t d = egcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}
void print(__int128 x)
{
    if (x < 0)
    {
        putchar('-');
        x = -x;
    }
    if (x > 9)
        print(x / 10);
    putchar(x % 10 + '0');
}
string to_string(__int128 x)
{
    string s = "";
    if (x < 0)
    {
        s += '-';
        x = -x;
    }
    while (x > 0)
    {
        s += (char)(x % 10 + '0');
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return s;
}

string multiply(const string &num1, const string &num2)
{
    int len1 = num1.size();
    int len2 = num2.size();
    vector<int> result(len1 + len2, 0);
    for (int i = len1 - 1; i >= 0; --i)
    {
        for (int j = len2 - 1; j >= 0; --j)
        {
            int product = (num1[i] - '0') * (num2[j] - '0');
            int sum = product + result[i + j + 1];
            result[i + j + 1] = sum % 10;
            result[i + j] += sum / 10;
        }
    }
    string finalResult;
    for (int digit : result)
        finalResult += to_string(digit);
    finalResult.erase(0, finalResult.find_first_not_of('0'));
    return finalResult.empty() ? "0" : finalResult;
}
string subtract(const string &num1, const string &num2)
{
    int len1 = num1.size();
    int len2 = num2.size();
    string result;
    int carry = 0;
    for (int i = len1 - 1, j = len2 - 1; i >= 0 || j >= 0 || carry; --i, --j)
    {
        int digit1 = (i >= 0) ? (num1[i] - '0') : 0;
        int digit2 = (j >= 0) ? (num2[j] - '0') : 0;

        int diff = digit1 - digit2 - carry;
        if (diff < 0)
        {
            diff += 10;
            carry = 1;
        }
        else
            carry = 0;
        result += to_string(diff);
    }
    reverse(result.begin(), result.end());
    result.erase(0, result.find_first_not_of('0'));
    return result;
}

// 1 if first one is greater
// 0 if equal
// -1 if second one is greater
int compare(const string &num1, const string &num2)
{
    string a = num1;
    string b = num2;
    a.erase(0, a.find_first_not_of('0'));
    b.erase(0, b.find_first_not_of('0'));
    if (a.length() < b.length())
        return -1;
    else if (a.length() > b.length())
        return 1;
    for (size_t i = 0; i < a.length(); ++i)
    {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }
    return 0;
}
string divideByTwo(const string &bigInteger)
{
    string result;
    int carry = 0;
    for (char digit : bigInteger)
    {
        int currentDigit = digit - '0';
        int newDigit = (currentDigit + carry * 10) / 2;
        carry = (currentDigit + carry * 10) % 2;
        result += to_string(newDigit);
    }
    result.erase(0, result.find_first_not_of('0'));
    return result.empty() ? "0" : result;
}
string calc_mod(string a, string m)
{
    string rem = "";
    for (auto c : a)
    {
        rem += c;
        if (rem.size() >= m.size())
        {
            for (char i = '9'; i >= '0'; i--)
            {
                string mul;
                mul += i;
                string result = multiply(m, mul);
                if (compare(rem, result) >= 0)
                {
                    rem = subtract(rem, result);
                    break;
                }
            }
        }
    }
    return rem.empty() ? "0" : rem;
}
string bigmod(string a, string b, string m)
{
    if (b == "0")
        return "1";
    string temp = bigmod(a, divideByTwo(b), m);
    temp = multiply(temp, temp);
    temp = calc_mod(temp, m);
    if ((b.back() - '0') % 2)
    {
        temp = multiply(temp, a);
        temp = calc_mod(temp, m);
    }
    return temp;
}
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
