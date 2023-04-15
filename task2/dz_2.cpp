#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <sstream>

class IOperation {
 public:
  virtual void ProcessLine(const std::string &str);
  virtual void HandleEndOfInput() = 0;
  virtual void SetNextOperation(std::unique_ptr<IOperation> nextOperation) = 0;
  virtual std::unique_ptr<IOperation> GetNextOperation() { return nullptr; };
 private:
  std::unique_ptr<IOperation> nextOperation_ = nullptr;

};
void IOperation::ProcessLine(const std::string &str) {

}

class CatOperation : public IOperation {
 public:
  explicit CatOperation(std::string filename) : filename_(std::move(filename)) {}

  void ProcessLine(const std::string &str) override {
    std::cout << str << std::endl;
  }

  void HandleEndOfInput() override {
    std::ifstream file(filename_);
    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        std::cout << line << std::endl;
      }
      file.close();
    }
    else {
      std::cerr << "Failed to open file: " << filename_ << std::endl;
    }
    if (nextOperation_ != nullptr) {
      auto a = std::move(nextOperation_);
      a->HandleEndOfInput();
    }
  }

  void SetNextOperation(std::unique_ptr<IOperation> nextOperation) override {
    nextOperation_ = std::move(nextOperation);
  }

  std::unique_ptr<IOperation> GetNextOperation() override {
    return std::move(nextOperation_);
  }

 private:
  std::string filename_;
  std::unique_ptr<IOperation> nextOperation_ = nullptr;
};

class EchoOperation : public IOperation {
 public:
  explicit EchoOperation(std::string str) : str_(std::move(str)) {}

  void ProcessLine(const std::string &str) override {
  }

  void HandleEndOfInput() override {
    std::cout << str_ << std::endl;
    if (nextOperation_ != nullptr) {
      auto a = std::move(nextOperation_);
      a->HandleEndOfInput();
    }
  }

  void SetNextOperation(std::unique_ptr<IOperation> nextOperation) override {
    nextOperation_ = std::move(nextOperation);
  }

  std::unique_ptr<IOperation> GetNextOperation() override {
    return std::move(nextOperation_);
  }

 private:
  std::string str_;
  std::unique_ptr<IOperation> nextOperation_ = nullptr;

};

class ReplaceOperation : public IOperation {
 public:
  ReplaceOperation(std::string strA, std::string strB, std::string fileName)
      : strA_(std::move(strA)), strB_(std::move(strB)), fileName_(std::move(fileName)) {}

  void ProcessLine(std::string &line) {
    size_t pos = 0;
    while ((pos = line.find(strA_, pos)) != std::string::npos) {
      line.replace(pos, strA_.length(), strB_);
      pos += strB_.length();
    }
  }

  void HandleEndOfInput() override {
    std::ifstream fileIn(fileName_);
    if (fileIn.is_open()) {
      std::string line;
      std::string output;
      while (std::getline(fileIn, line)) {
        ProcessLine(line);
        output += line + "\n";
      }
      fileIn.close();
      std::ofstream fileOut(fileName_);
      if (fileOut.is_open()) {
        fileOut << output;
        fileOut.close();
      }
    }
    if (nextOperation_ != nullptr) {
      auto a = std::move(nextOperation_);
      a->HandleEndOfInput();
    }
  }

  void Execute() {}

  void SetNextOperation(std::unique_ptr<IOperation> nextOperation) override {
    nextOperation_ = std::move(nextOperation);
  }

  std::unique_ptr<IOperation> GetNextOperation() override {
    return std::move(nextOperation_);
  }

 private:
  std::string strA_;
  std::string strB_;
  std::string fileName_;
  std::unique_ptr<IOperation> nextOperation_ = nullptr;
};

int main(int argc, char *argv[]) {
  std::string expression;

  for (int i = 1; i < argc; ++i) {
    expression += argv[i];
    if (i < argc - 1) {
      expression += ' ';
    }
  }
  std::vector<std::unique_ptr<IOperation>> operations;
  size_t pos = 0;
  while ((pos = expression.find('|')) != std::string::npos) {

    std::string op = expression.substr(0, pos - 1);
    expression.erase(0, pos + 2);

    if (op.substr(0, 4) == "echo") {
      std::string arg = op.substr(5);
      operations.push_back(std::make_unique<EchoOperation>(arg));
    }
    else if (op.substr(0, 3) == "cat") {
      std::string arg = op.substr(4);
      operations.push_back(std::make_unique<CatOperation>(arg));
    }
    else if (op.substr(0, 7) == "replace") {
      std::stringstream x;
      x << op;
      std::string word;
      x >> word;
      x >> word;
      std::string str1 = word;
      x >> word;
      std::string str2 = word;
      x >> word;
      std::string file_name = word;
      operations.push_back(std::make_unique<ReplaceOperation>(str1, str2, file_name));
    }
  }
  if (!expression.empty()) {
    if (expression.substr(0, 4) == "echo") {
      std::string arg = expression.substr(5);
      operations.push_back(std::make_unique<EchoOperation>(arg));
    }
    else if (expression.substr(0, 3) == "cat") {
      std::string arg = expression.substr(4);
      operations.push_back(std::make_unique<CatOperation>(arg));
    }
    else if (expression.substr(0, 7) == "replace") {
      std::stringstream x;
      x << expression;
      std::string word;
      x >> word;
      x >> word;
      std::string str1 = word;
      x >> word;
      std::string str2 = word;
      x >> word;
      std::string file_name = word;
      operations.push_back(std::make_unique<ReplaceOperation>(str1, str2, file_name));
    }
  }
  for (size_t i = operations.size() - 1; i > 0; i--) {
    operations[i - 1]->SetNextOperation(std::move(operations[i]));
  }
  operations[0]->HandleEndOfInput();
  return 0;
}
/*пример
file_1.txt = "lol"
Ввод
./text_processor 'replace lol gogo file_1.txt | cat file_1.txt | replace gogo tot file_1.txt | cat file_1.txt | echo 1'
Вывод:
gogo
tot
1*/

