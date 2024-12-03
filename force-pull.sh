#!/bin/bash

# 获取当前分支名
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)

# 拉取远程最新代码并重置当前分支
git fetch --all
git reset --hard origin/$BRANCH_NAME
git pull

# 等待用户输入以暂停（类似 pause）
read -p "Press any key to continue..."
