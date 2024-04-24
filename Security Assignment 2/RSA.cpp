#include <bits/stdc++.h>
using namespace std;
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

long long generate_random_prime(int number_of_bits)
{
    int num_digits = floor(number_of_bits / 3.322 + 1e-9) + 1;
    long long lower_limit = static_cast<long long>(pow(10, num_digits - 1));
    long long upper_limit = static_cast<long long>(pow(10, num_digits));

    long long random_number;
    do
    {
        random_number = rand() % (upper_limit - lower_limit) + lower_limit;
    } while (!miller_robin(random_number));

    return random_number;
}

auto generate_primes(int k)
{
    long long prime1 = generate_random_prime(k / 2);
    long long prime2;
    do
    {
        prime2 = generate_random_prime(k / 2);
    } while (prime2 == prime1);
    return (pair<long long, long long>){prime1, prime2};
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));
    int k = 96;
    auto [prime1, prime2] = generate_primes(k);

    cout << "Random prime 1: " << prime1 << endl;
    cout << "Random prime 2: " << prime2 << endl;

    return 0;
}
