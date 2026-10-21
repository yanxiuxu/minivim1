//accomplished
#include <fstream>

#include "Buffer.hpp"

namespace sjtu {

Buffer::Buffer(const std::filesystem::path& path){//打开一个已有文件来新建buffer
    if (path.empty()) {
        //新建一个空文件,即lines_中只有一个空的string作为占位
        lines_.emplace_back("");
        path_ = path;
        modified_ = false;
    } else {
        //打开一个已有文件,读取内容到lines_
        std::ifstream file(path);
        if (!file.is_open()) {//新建一个文件
            lines_.emplace_back("");
            path_ = path;
            modified_ = false;
        }
        else{
        std::string line;
        while (std::getline(file, line)) {
            lines_.push_back(line);
        }
        if (lines_.empty()) {
            lines_.emplace_back(""); //确保至少有一行
        }
        path_ = path;
        modified_ = false;
        }
    }
}

Buffer::Buffer(std::vector<std::string> lines, std::filesystem::path path) {//使用已有的lines来新建buffer
    if (lines.empty()) {
        lines_.emplace_back(""); //确保至少有一行
    } 
    else {
        for (const auto& line : lines) {
            lines_.push_back(line);
        }
    }
    path_ = path;
    modified_ = false;
}

std::size_t Buffer::GetLineCount() const {
    //返回文件行数
    return lines_.size();
}

const std::string& Buffer::GetLineAt(std::size_t row) const {
    //返回第row行的内容
    return lines_.at(row); //使用at()进行越界检查
}


std::string Buffer::GetDisplayName() const {
    //返回文件名,若是新文件,返回"[No Name]"
    if (path_.empty()) {
        return "[No Name]";
    } else {
        return path_.filename().string();
    }
}

bool Buffer::IsModified() const {
    //返回文件和上次保存比起来是否被修改过
    return modified_;
}

void Buffer::InsertCharacter(std::size_t row, std::size_t column, char value) {
    //在第row行第col列插入一个value, 注意越界检查
    if (row >= lines_.size()) {
        throw std::out_of_range("Row index out of range");
    }
    if (column > lines_[row].size()) {
        throw std::out_of_range("Column index out of range");
    }
    lines_[row].insert(column, 1, value);
    modified_ = true;
}

void Buffer::EraseCharacter(std::size_t row, std::size_t column) {
   //在第row行第col列删除一个value
    if (row >= lines_.size()) {
          throw std::out_of_range("Row index out of range");
     }
     if (column >= lines_[row].size()) {
          throw std::out_of_range("Column index out of range");
     }
     lines_[row].erase(column, 1);
     modified_ = true;
}

void Buffer::SplitLine(std::size_t row, std::size_t column) {
    //在第row行第col列分割,即在此处敲了回车键
    if (row >= lines_.size()) {
          throw std::out_of_range("Row index out of range");
     }
     if (column > lines_[row].size()) {
          throw std::out_of_range("Column index out of range");
     }
        std::string newLine = lines_[row].substr(column);
        lines_[row].resize(column);
        lines_.insert(lines_.begin() + row + 1, newLine);
        modified_ = true;
}

void Buffer::JoinLine(std::size_t row) {
   //把第row + 1行合并进第row行
   if (row >= lines_.size()) {
          throw std::out_of_range("Row index out of range");
     }
    if (row + 1 >= lines_.size()) {
        throw std::out_of_range("No line to join with");
    }

    lines_[row] += lines_[row + 1];
    lines_.erase(lines_.begin() + row + 1);
    modified_ = true;
}

void Buffer::Save() {
   //把文件内容保存, 直接调用WriteTo方法
   Buffer::WriteTo(path_);
   modified_ = false;
}

void Buffer::SaveAs(const std::filesystem::path& path) {
  Buffer::WriteTo(path);
  path_ = path;
  modified_ = false;
}


void Buffer::WriteTo(const std::filesystem::path& path) const {
   //实际将缓冲区中的内容写入path指向的文件中
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path.string());
    }
    for (const auto& line : lines_) {
        file << line << '\n';
    }
    file.close();
}

} // namespace sjtu
