all: clean Test TextAnalyzer

.outputFolder:
	mkdir -p out

TextAnalyzer: .outputFolder
	clang -std=c++17 -lstdc++ -lm Program.cpp -Wall -Wextra -Werror -o out/TextAnalyzer
	./out/TextAnalyzer

Test: .outputFolder
	clang -std=c++17 -lstdc++ -lm Tests.cpp -Wall -Wextra -Werror -o out/Tests
	./out/Tests

clean:
	rm -rf out