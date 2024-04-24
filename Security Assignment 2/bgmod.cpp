#include <bits/stdc++.h>
using namespace std;
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

void solve()
{
    string a, b, c;
    while (cin >> a >> b >> c)
        cout << bigmod(a, b, c) << "\n";
    return;
}
int32_t main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    solve();
    return 0;
}