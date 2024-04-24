#include <iostream>
#include <vector>

// S-Box lookup table (substitution)
const unsigned char sbox[256] = {
    // ... (S-Box values here)
};

// Round constants (rcon)
const unsigned char rcon[11] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// Circular byte left shift
void ShiftWord(std::vector<unsigned char>& word) {
    unsigned char temp = word[0];
    for (size_t i = 0; i < 3; ++i) {
        word[i] = word[i + 1];
    }
    word[3] = temp;
}

// SubByte transformation
void SubByte(std::vector<unsigned char>& word) {
    for (size_t i = 0; i < 4; ++i) {
        word[i] = sbox[word[i]];
    }
}

// Add round constants
void AddRoundConstants(std::vector<unsigned char>& word, unsigned int round) {
    word[0] ^= rcon[round];
}

// AES key expansion g function
std::vector<unsigned char> KeyExpansionG(const std::vector<unsigned char>& input) {
    std::vector<unsigned char> output(4);

    // Initialize output with input
    for (size_t i = 0; i < 4; ++i) {
        output[i] = input[i];
    }

    // Perform the g function steps
    ShiftWord(output);
    SubByte(output);
    AddRoundConstants(output, 1); // Assuming round 1 (0-based index)

    return output;
}

int main() {
    // Example usage
    std::vector<unsigned char> input = {0x54, 0x68, 0x61, 0x74}; // Example input
    std::vector<unsigned char> result = KeyExpansionG(input);

    // Print the resulting word
    std::cout << "Resulting word: ";
    for (const auto& byte : result) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    return 0;
}
