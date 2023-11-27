#include <iostream>
#include <vector>
#include <regex>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;

struct Word {
    string str;
    int indexInText;

    bool operator==(const Word& other) const {
        return str == other.str && indexInText == other.indexInText;
    }
};

auto remove_special_characters = [](string str) {
    str.erase(remove_if(str.begin(), str.end(), [](char c) {
        return !isalnum(c) && c != ' ';
    }), str.end());
    return str;
};

auto tokenize = [](const std::string& line, const char separator) -> std::vector<Word> {
    std::vector<Word> splittedWords;

    size_t startIndex = 0;
    size_t endIndex = line.find(separator);

    while (endIndex != std::string::npos) {
        std::string subString = line.substr(startIndex, endIndex - startIndex);
        subString = remove_special_characters(subString);

        std::transform(subString.begin(), subString.end(), subString.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        if (!subString.empty()) {
            Word word{subString, static_cast<int>(startIndex)};
            splittedWords.push_back(word);
        }

        startIndex = endIndex + 1;
        endIndex = line.find(separator, startIndex);
    }

    // Process the last token after the last separator
    std::string lastToken = line.substr(startIndex, endIndex);
    lastToken = remove_special_characters(lastToken);
    std::transform(lastToken.begin(), lastToken.end(), lastToken.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    if (!lastToken.empty()) {
        Word word{lastToken, static_cast<int>(startIndex)};
        splittedWords.push_back(word);
    }

    return splittedWords;
};

auto split_book_into_chapters = [](const string& book) -> vector<string> {
    regex chapter_regex(R"(CHAPTER \d+)");
    sregex_token_iterator chapters_begin(book.begin(), book.end(), chapter_regex, -1);
    sregex_token_iterator chapters_end;
    vector<string> chapters(chapters_begin, chapters_end);
    chapters.erase(chapters.begin());
    return chapters;
};

auto filter_words = [](const vector<Word>& words, const vector<string>& filter) -> vector<Word> {
    vector<Word> filterWords;

    copy_if(words.begin(), words.end(), back_inserter(filterWords), [=](const Word& word){
        return std::find_if(filter.begin(), filter.end(), [&](const std::string& filterWord) {
            return filterWord == word.str;
        }) != filter.end();
    });

    return filterWords;
};

TEST_CASE("Filter Words 1:1") {
    vector<Word> words = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };

    vector<string> filter = {"hello", "world", "this", "is", "a", "test"};

    vector<Word> expected = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };

    vector<Word> actual = filter_words(words, filter);

    CHECK(expected == actual);
}

TEST_CASE("Filter Words 1:0.9") {
    vector<Word> words = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };

    vector<string> filter = {"hello", "world", "this", "is", "test"};

    vector<Word> expected = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"test", 24}
    };

    vector<Word> actual = filter_words(words, filter);

    CHECK(expected == actual);
}

TEST_CASE("Split Book into Chapters") {
    string book = "CHAPTER 1 ABC CHAPTER 2 DEF";
    vector<string> expected = {" ABC ", " DEF"};
    vector<string> actual = split_book_into_chapters(book);
    CHECK(expected == actual);
}

TEST_CASE("Remove Special Characters") {
    string str = "Hello, World! This is a test.";
    string expected = "Hello World This is a test";
    string actual = remove_special_characters(str);
    CHECK(expected == actual);
}

TEST_CASE("Tokenize function") {
    string line = "Hello, World! This is a test.";

    vector<Word> expected = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };

    vector<Word> actual = tokenize(line, ' ');

    CHECK(expected == actual);
}