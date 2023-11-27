all: clean build run

build: Test TextAnalyzer

run: Test_run TextAnalyzer_run

Test_run: .outputFolder
	./out/Tests

TextAnalyzer_run: .outputFolder
	./out/TextAnalyzer

.outputFolder:
	mkdir -p out

TextAnalyzer: .outputFolder
	clang -std=c++17 -lstdc++ -lm Program.cpp -Wall -Wextra -Werror -o out/TextAnalyzer

Test: .outputFolder
	clang -std=c++17 -lstdc++ -lm Tests.cpp -Wall -Wextra -Werror -o out/Tests
	
clean:
	rm -rf out