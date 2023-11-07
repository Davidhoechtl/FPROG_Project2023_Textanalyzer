all: TextAnalyzer

.outputFolder:
	mkdir -p out

TextAnalyzer: .outputFolder
	clang -std=c++17 -lstdc++ -lm Program.cpp -Wall -Wextra -Werror -o out/TextAnalyzer
	./out/TextAnalyzer