#include <iostream>
#include <vector>
#include <fstream>
#include <optional>
#include <algorithm>
#include <iterator>
#include <string>
#include <functional>
#include <numeric>

using namespace std;
using namespace std::placeholders;

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
    vector<string> splittedWords;
    for_each(line.begin(), line.end(), [&](const char c){
        if(c == seperator || currentIteration == line.length() - 1){
            endIndex = currentIteration;
            string subString = "";
            subString.append(line, startIndex, endIndex - startIndex);
            splittedWords.push_back(subString);
            currentIteration++;
            startIndex = endIndex + 1;
        }
        else{
            currentIteration++;
        }
    });

    return splittedWords;
};

auto filter_words = [](const vector<string>& words, const vector<string>& filter){
    vector<string> filteredWords;

    copy_if(words.begin(), words.end(), back_inserter(filteredWords), [=](const string word){
        return find(filter.begin(), filter.end(), word) != filter.end();
    });

    return filteredWords;
};

// key value pair of the count word result
struct WordCount{
    string word;
    int value;
};

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

void outputStrings(const vector<string>& terms){
    for(string term : terms){
        cout << term << endl;
    }
}

int main(){
    vector<string> peaceTerms = read_lines("./data/peace_terms.txt").value();
    vector<string> warTerms = read_lines("./data/war_terms.txt").value();

    auto filterPeaceTerms = bind(filter_words, _1, peaceTerms);
    auto filterWarTerms = bind(filter_words, _1, warTerms);

    outputStrings(peaceTerms);
    outputStrings(warTerms);

    string unsplittedline = read_lines("./data/test.txt").value()[0];
    vector<string> splittedWords = split_line(unsplittedline, ' ');

    outputStrings(splittedWords);

    return 0;
}