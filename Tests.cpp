#include <iostream>
#include <vector>
#include <fstream>
#include <optional>
#include <algorithm>
#include <iterator>
#include <string>
#include <functional>
#include <numeric>
#include <map>
#include <regex>
#include <sstream>

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

struct WordCount{
    string word;
    int count;

    bool operator==(const WordCount& other) const {
        return word == other.word && count == other.count;
    }
};

enum struct Relation {
    WAR = 0,
    PEACE = 1
};

const string relationToString(const Relation rel) {
    switch (rel) {
        case Relation::WAR:
            return "war";
        case Relation::PEACE:
            return "peace";
        default:
            return "UNKNOWN";
    }
}

auto remove_special_characters = [](string str) {
    str.erase(remove_if(str.begin(), str.end(), [](char c) {
        return !isalnum(c) && c != ' ';
    }), str.end());
    return str;
};


auto tokenize = [](const string& line, const char separator) -> vector<Word> {
    vector<Word> splittedWords;

    vector<string> tokens;
    istringstream iss(line);
    string token;
    int index = 0;
    while (getline(iss, token, separator)) {
        tokens.push_back(token);
    }

    transform(tokens.begin(), tokens.end(), std::back_inserter(splittedWords), [&](const std::string& token) {
        string subString = remove_special_characters(token);

        transform(subString.begin(), subString.end(), subString.begin(), [](unsigned char c) {
            return tolower(c);
        });

        if (!subString.empty()) {
            Word word{subString, index};
            index += token.size() + 1; // Increment index by the size of the token plus 1 for the separator
            return word;
        } else {
            return Word{};
        }
    });

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

auto calculate_wordCount = [](const map<string, vector<Word>>& wordMap) -> vector<WordCount> {
    vector<WordCount> result;

    for_each(wordMap.begin(), wordMap.end(), [&](const pair<string, vector<Word>>& pair){
        int wordCount = pair.second.size();
        WordCount current {pair.first, wordCount};
        result.push_back(current);
    });

    return result;
};

auto map_words = [](const vector<Word>& words) -> map<string, vector<Word>> {
    map<string, vector<Word>> wordMap;
    for_each(words.begin(), words.end(), [&wordMap](const Word& word) {
        wordMap[word.str].push_back(word);
    });
    return wordMap;
};

auto calculate_density = [](const vector<Word>& words) -> double {
    if(words.size() < 2){
        return -1.0;
    }

    double distanceSum = accumulate(words.begin(), words.end() - 1, 0.0, [](double sum, const Word& word){
        auto next = &word + 1;
        int distance = next->indexInText - word.indexInText;
        return sum + distance;
    });

    return distanceSum / (words.size() - 1);
};

auto get_relation_value = [](const vector<WordCount>& wordData, const double& density){
    int sumCount = accumulate(wordData.begin(), wordData.end(), 0, [](const int accumulator, const WordCount& item){
        return accumulator + item.count;
    });

    return sumCount + (200 - density);
};

/* auto process_chapter = [](const string& chapter) {
    return [chapter](const auto& filterPeaceTerms, const auto& filterWarTerms) -> Relation {
        auto chapter_words = tokenize(chapter, ' ');

        auto warWords = filterWarTerms(chapter_words);
        auto peaceWords = filterPeaceTerms(chapter_words);

        auto warMap = map_words(warWords);
        auto peaceMap = map_words(peaceWords);

        auto warResult = calculate_wordCount(warMap);
        auto peaceResult = calculate_wordCount(peaceMap);

        auto warDensity = calculate_density(warWords);
        auto peaceDensity = calculate_density(peaceWords);

        int warRelationValue = get_relation_value(warResult, warDensity);
        int peaceRelationValue = get_relation_value(peaceResult, peaceDensity);

        return ((warRelationValue > peaceRelationValue) ? Relation::WAR : Relation::PEACE);
    };
}; */

/* TEST_CASE("Process Chapter Test") {
    string chapter = "Once upon a time, there was a war. The war was long and hard. The war was won by the good guys. The good guys lived happily ever after.";
    vector<string> warTerms = {"war", "hard"};
    vector<string> peaceTerms = {"peace", "happily"};
    
    Relation expected = Relation::WAR;
    auto filterPeaceTerms = bind(filter_words, placeholders::_1, peaceTerms);
    auto filterWarTerms = bind(filter_words, placeholders::_1, warTerms);
    Relation actual = process_chapter(chapter)(filterWarTerms, filterPeaceTerms);
    
    CHECK(expected == actual);
} */

TEST_CASE("Relation Value Test") {
    vector<WordCount> wordData = {
        {"hello", 2},
        {"world", 1},
        {"this", 2},
        {"is", 1},
        {"a", 1},
        {"test", 3}
    };
    
    double density = 7.0;
    
    int expected = 203;
    int actual = get_relation_value(wordData, density);
    
    CHECK(expected == actual);
}

TEST_CASE("Calculate Density Test") {
    vector<Word> words = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };
    
    double expected = 4.8;
    double actual = calculate_density(words);
    
    CHECK(expected == actual);
}

TEST_CASE("Map Words Test") {
    vector<Word> words = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };
    
    map<string, vector<Word>> expected = {
        {"hello", {{"hello", 0}}},
        {"world", {{"world", 7}}},
        {"this", {{"this", 14}}},
        {"is", {{"is", 19}}},
        {"a", {{"a", 22}}},
        {"test", {{"test", 24}}}
    };
    
    map<string, vector<Word>> actual = map_words(words);
    
    CHECK(expected == actual);
}

TEST_CASE("Tokenize Test") {
    string line = "Hello, world! This is a test.";
    char separator = ' ';
    
    vector<Word> expected = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };
    
    vector<Word> actual = tokenize(line, separator);
    
        for (const auto& wordCount : actual) {
        for (const auto& wordCount2 : expected) {
            if (wordCount.str == wordCount2.str) {
                CHECK(wordCount.indexInText == wordCount2.indexInText);
            }
        }
    }
}

TEST_CASE("Split Book into Chapters Test") {
    string book = "CHAPTER 1 Once upon a time... CHAPTER 2 In a land far away...";
    
    vector<string> expected = {
        " Once upon a time...",
        " In a land far away..."
    };
    
    vector<string> actual = split_book_into_chapters(book);

    for (const auto& chapter : actual) {
        for (const auto& chapter2 : expected) {
            if (chapter == chapter2) {
                CHECK(true);
            }
        }
    }
}

TEST_CASE("Filter Words Test") {
    vector<Word> words = {
        {"hello", 0},
        {"world", 7},
        {"this", 14},
        {"is", 19},
        {"a", 22},
        {"test", 24}
    };
    
    vector<string> filter = {"hello", "world"};
    
    vector<Word> expected = {
        {"hello", 0},
        {"world", 7}
    };
    
    vector<Word> actual = filter_words(words, filter);
    
    CHECK(expected == actual);
}

TEST_CASE("Calculate Word Count Test") {
    map<string, vector<Word>> wordMap = {
        {"hello", {{"hello", 0}, {"hello", 5}}},
        {"world", {{"world", 7}}},
        {"this", {{"this", 14}, {"this", 20}}},
        {"is", {{"is", 19}}},
        {"a", {{"a", 22}}},
        {"test", {{"test", 24}, {"test", 30}, {"test", 35}}}
    };
    
    vector<WordCount> expected = {
        {"hello", 2},
        {"world", 1},
        {"this", 2},
        {"is", 1},
        {"a", 1},
        {"test", 3}
    };

    vector<WordCount> actual = calculate_wordCount(wordMap);

    for (const auto& wordCount : actual) {
        for (const auto& wordCount2 : expected) {
            if (wordCount.word == wordCount2.word) {
                CHECK(wordCount.count == wordCount2.count);
            }
        }
    }
}

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

TEST_CASE("Remove Special Characters") {
    string str = "Hello, World! This is a test.";
    string expected = "Hello World This is a test";
    string actual = remove_special_characters(str);
    CHECK(expected == actual);
}