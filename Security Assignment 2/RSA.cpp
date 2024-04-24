#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
#define ll long long
ll mul_mod(ll x, ll y, ll m)
{
    ll res = __int128(x) * y % m;
    return res;
}
ll pow_mod(ll x, ll n, ll m)
{
    ll res = 1 % m;
    for (; n; n >>= 1)
    {
        if (n & 1)
            res = mul_mod(res, x, m);
        x = mul_mod(x, x, m);
    }
    return res;
}
bool miller_robin(ll n)
{
    if (n <= 2 || (n & 1 ^ 1))
        return (n == 2);
    ll c, d, s = 0, r = n - 1;
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

auto generate_primes(int k)
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
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    int k = 96;
    auto start = high_resolution_clock::now();
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
    auto stop = high_resolution_clock::now();
    auto key_duration = duration_cast<microseconds>(stop - start);

    cout << "Bit Size = " << k << "\n\n";
    cout << "Public Key:(e,n) = (" << e << ",";
    print(n);
    cout << ")\n";
    cout << "Private Key:(d,n) = (";
    print(d);
    cout << ",";
    print(n);
    cout << ")\n\n";

    string plainText = "BUETCSEVSSUSTCSE";
    cout << "Plain Text:\n"
         << plainText << "\n";
    vector<string> encrypted;
    string E = to_string(e);
    // string D = to_string(d);
    for (auto p : plainText)
    {
        string P = to_string((int)p);
        cout << P << "\n";
    }

    return 0;
}
