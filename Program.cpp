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

struct Word{
    string str;
    int indexInText;

    bool operator==(Word other){
        return str == other.str && indexInText == other.indexInText;
    }
};

struct WordCount{
    string word;
    int count;
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

auto filter_wordResult = [](const vector<WordCount>& words, const vector<string>& filter){
    vector<WordCount> filteredWordResult;

    int wordIndex;
    copy_if(words.begin(), words.end(), back_inserter(filteredWordResult), [=, &wordIndex](const WordCount wordData){
        return find(filter.begin(), filter.end(), wordData.word) != filter.end();
    });

    return filteredWordResult;
};

auto filter_words = [](const vector<Word>& words, const vector<string>& filter){
    vector<Word> filterWords;

    int wordIndex;
    copy_if(words.begin(), words.end(), back_inserter(filterWords), [=, &wordIndex](const Word word){
        return find(filter.begin(), filter.end(), word.str) != filter.end();
    });

    return filterWords;
};
#pragma endregion IO Reading

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

auto get_term_densityv2 = [](const vector<Word>& words){
    if(words.size() < 2){
        return -1.0;
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
};

auto calculate_wordCount = [](const map<string, vector<Word>>& wordMap ) ->
vector<WordCount>{
    vector<WordCount> result;

    for_each(wordMap.begin(), wordMap.end(), [&](const pair<string, vector<Word>>& pair){
        int wordCount = pair.second.size();
        double wordDensity = get_term_densityv2(pair.second);
        WordCount current {pair.first, wordCount};
        result.push_back(current);
    });

    return result;
};

auto get_relation_value = [](const vector<WordCount>& wordData, const double& density){
    int sumCount = accumulate(wordData.begin(), wordData.end(), 0, [](const int accumulator, const WordCount& item){
        return accumulator + item.count;
    });

    return sumCount + (1000 - density);
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
    // get all terms
    vector<string> peaceTerms = read_lines("./data/peace_terms.txt").value();
    vector<string> warTerms = read_lines("./data/war_terms.txt").value();

    // define filter functions
    auto filterPeaceTerms = bind(filter_words, _1, peaceTerms);
    auto filterWarTerms = bind(filter_words, _1, warTerms);

    outputStrings(peaceTerms);
    outputStrings(warTerms);

    // get all lines from file
    vector<string> textLines = read_lines("./data/test.txt").value();

    // flatten the lines to one string
    string tmp;
    for(string line : textLines){
        tmp.append(line);
    }

    // split text to Words
    vector<Word> words = split_line(tmp, ' ');

    // filter war and peace tearms from text
    vector<Word> warWords = filterWarTerms(words);
    vector<Word> peaceWords = filterPeaceTerms(words);

    // map words (key: string, value: list of words)
    map<string, vector<Word>> warMap = map_words(warWords);
    map<string, vector<Word>> peaceMap = map_words(peaceWords);

    // calculate word count
    vector<WordCount> warResult = calculate_wordCount(warMap);
    vector<WordCount> peaceResult = calculate_wordCount(peaceMap);

    // calculate term density
    double warDensity = get_term_densityv2(warWords);
    double peaceDensity = get_term_densityv2(peaceWords);

    // calculate relation score
    int warRelationValue = get_relation_value(warResult, warDensity);
    int peaceRelationValue = get_relation_value(peaceResult, peaceDensity);

    // compare relation score
    if(warRelationValue > peaceRelationValue){
        cout << "war related" << endl;
    } 
    else{
        cout << "peace related" << endl;
    }

    // vector<Word> splittedWords = split_line(unsplittedline, ' ');
    // outputWords(splittedWords);

    return 0;
}