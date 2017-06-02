#pragma once
#include <vector>
#include <string>
#include <map>
#include <stack>

class TickerNode
{
public:
	std::string value;//from root to this node
	std::map<std::string, TickerNode> children;
	bool isATicker;

public:
	TickerNode() {}
	TickerNode(std::string value_, bool isATicker_) :value(value_), isATicker(isATicker_) {}
	~TickerNode() {}

	//insert a ticker into the Trie
	void insert(std::string ticker, int pos);
	//search all tickers start with "ticker"
	std::vector<std::string> search(std::string ticker);
	//private function for search (use stack to iterate all children in pre-order)
	std::vector<std::string> preOrder();
};

