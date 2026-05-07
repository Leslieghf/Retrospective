#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {
void print_header(std::string_view title) {
    std::cout << "\n=== " << title << " ===\n";
}

void vectors_and_algorithms() {
    print_header("vectors_and_algorithms");

    std::vector<int> numbers{4, 1, 7, 2, 9};
    std::ranges::sort(numbers);

    const int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    std::cout << "sorted: ";
    for (const int n : numbers) {
        std::cout << n << " ";
    }
    std::cout << "\nsum: " << sum << "\n";
}

void refs_vs_values() {
    print_header("refs_vs_values");

    int original = 10;
    int copied = original;
    int& alias = original;

    copied += 5;
    alias += 5;

    std::cout << "original (changed via ref): " << original << "\n";
    std::cout << "copied (independent): " << copied << "\n";
}

std::optional<std::size_t> find_index(std::span<const int> values, const int needle) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] == needle) {
            return i;
        }
    }
    return std::nullopt;
}

void optionals_and_span() {
    print_header("optionals_and_span");

    const std::vector<int> values{8, 3, 5, 13};
    const auto index = find_index(values, 5);
    if (index.has_value()) {
        std::cout << "found 5 at index " << *index << "\n";
    } else {
        std::cout << "5 not found\n";
    }
}

void string_view_warning() {
    print_header("string_view_warning");

    const std::string owned = "hello-from-owned-string";
    std::string_view view = owned;
    std::cout << "view while owner alive: " << view << "\n";

    // If `owned` died here and we still used `view`, that would be a dangling view.
    // This is a common C++ lifetime footgun.
}
} // namespace

int main() {
    std::cout << "Retrospective C++20 Playground\n";
    std::cout << "Edit this file freely. No Source SDK or Steam dependencies.\n";

    vectors_and_algorithms();
    refs_vs_values();
    optionals_and_span();
    string_view_warning();

    return 0;
}
