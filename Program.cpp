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

using namespace std;
using namespace std::placeholders;

// WordResult{
//     Word word;
//     int density;
//     int count;
// }

struct Word{
    string str;
    int indexInText;

    bool operator==(Word other){
        return str == other.str && indexInText == other.indexInText;
    }
};

// nicht mehr verwenden
// key value pair of the count word result
struct WordCount{
    string word;
    int value;
};

#pragma region IO Reading
auto read_lines = [](const string& filePath) -> optional<vector<string>>{
    ifstream inputFile(filePath);

    if(inputFile.is_open()){
        vector<string> values;

        string line;
        while(!inputFile.eof()){
            getline(inputFile, line);
            values.push_back(line);
        }

        return values;
    }
    else{
        return nullopt;
    }
};

auto split_line = [](const string& line, const char seperator){
    int startIndex = 0;
    int endIndex = 0;
    int currentIteration = 0;
    vector<Word> splittedWords;
    for_each(line.begin(), line.end(), [&](const char c){
        if(c == seperator || currentIteration == line.length() - 1){
            endIndex = currentIteration;
            string subString = "";
            subString.append(line, startIndex, endIndex - startIndex);
            Word word {subString, startIndex};
            splittedWords.push_back(word);
            currentIteration++;
            startIndex = endIndex + 1;
        }
        else{
            currentIteration++;
        }
    });

    return splittedWords;
};

auto filter_words = [](const vector<Word>& words, const vector<string>& filter){
    vector<Word> filteredWords;

    int wordIndex;
    copy_if(words.begin(), words.end(), back_inserter(filteredWords), [=, &wordIndex](const Word word){
        return find(filter.begin(), filter.end(), word.str) != filter.end();
    });

    return filteredWords;
};
#pragma endregion IO Reading

// verwende map um count zu ermitteln
#pragma region count words
auto make_vector_unique = [](const vector<string>& vec) -> const vector<string>{
    vector<string> copy = vec;

    sort(copy.begin(), copy.end());
    auto eraseBegin = unique(copy.begin(), copy.end());
    copy.erase(eraseBegin, copy.end());

    sort(vec.begin(), vec.end());
};

auto count_word = [](const string& word, const vector<string>& words) -> int{
    return accumulate(words.begin(), words.end(), 0, [=](int result, const string element){
        if(strcmp(word.c_str(), element.c_str()) == 0){
            result++;
        }

        return result;
    });
};

auto count_words = [](const vector<string>& words) -> vector<WordCount>{
    const vector<string> uniqueWords = make_vector_unique(words);

    vector<WordCount> wordCount;
    transform(uniqueWords.begin(), uniqueWords.end(), wordCount, [=](const string& element){
        return WordCount { element, count_word(element, words)};
    });
    
    return wordCount;
};
#pragma endregion

#pragma region map words
auto map_words = [](const vector<Word>& words) -> map<string, vector<Word>>{
    map<string, vector<Word>> map;
    for(Word word : words){
        vector<Word> words;
        for(Word word2: words){
            if(word.str == word2.str){
                words.push_back(word2);
            }
        }
        map.insert(make_pair(word.str, words));
    }

    return map;
};

auto get_term_density = [](const map<string, vector<Word>>& wordMap, const string& term) -> double{
    auto foundPointer = wordMap.find(term);
    if(foundPointer == wordMap.end()){
        return -1;
    }
    else{
        vector<Word> words = foundPointer->second;

        if(words.size() < 2){
            return 0;
        }

        double distanceSum = 0;
        for (auto it = words.begin(); it != words.end(); ++it) {
            auto next = it + 1;
            if(next != words.end()){
                int distance = it->indexInText - next->indexInText;
                distanceSum += distance;
            }
        }

        return distanceSum / words.size();
    }
};
#pragma endregion map words

// output (debug)
void outputStrings(const vector<string>& terms){
    for(string term : terms){
        cout << term << endl;
    }
}

void outputWords(const vector<Word>& terms){
    for(Word term : terms){
        cout << "(" << term.str << "," << term.indexInText << ")" << endl;
    }
}

int main(){
    vector<string> peaceTerms = read_lines("./data/peace_terms.txt").value();
    vector<string> warTerms = read_lines("./data/war_terms.txt").value();

    auto filterPeaceTerms = bind(filter_words, _1, peaceTerms);
    auto filterWarTerms = bind(filter_words, _1, warTerms);

    outputStrings(peaceTerms);
    outputStrings(warTerms);

    vector<string> textLines = read_lines("./data/test.txt").value();
    string tmp;
    for(string line : textLines){
        tmp.append(line);
    }

    vector<Word> words = split_line(tmp, ' ');
    // vector<Word> splittedWords = split_line(unsplittedline, ' ');

    // outputWords(splittedWords);

    return 0;
}