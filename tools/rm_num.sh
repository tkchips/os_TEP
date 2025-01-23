#!/bin/bash

# 检查是否提供了文件名
if [ -z "$1" ]; then
  echo "Usage: $0 <filename>"
  exit 1
fi

# 检查文件是否存在
if [ ! -f "$1" ]; then
  echo "File not found: $1"
  exit 1
fi

# 使用 sed 命令移除每一行前面的数字和空格，并直接修改文件
sed -i 's/^[0-9]\+ //g' "$1"

echo "Leading numbers removed from $1"