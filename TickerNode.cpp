#include "TickerNode.h"

//insert a ticker into the Trie
void TickerNode::insert(std::string ticker, int pos){
	unsigned long length = ticker.size();
	std::string edge = ticker.substr(pos, 1);
	auto i = children.find(edge);
	//current position is in the last char of ticker
	if (pos == length - 1) {
		if (i != children.end()) {//this node already exists
			(i->second).isATicker = true;
		}
		else {
			children[edge] = TickerNode(ticker.substr(0, pos + 1), true);
		}
	}
	//before the last char of ticker
	else {
		if (i != children.end()) {
			(i->second).insert(ticker, pos + 1);
		}
		else {
			children[edge] = TickerNode(ticker.substr(0, pos + 1), false);
			children[edge].insert(ticker, pos + 1);
		}
	}
}


//private function for search (use stack to iterate all children in pre-order)
std::vector<std::string> TickerNode::preOrder(){
	std::stack<TickerNode*> preorder_stack;
	preorder_stack.push(this);
	std::vector<std::string> result;
	while (!preorder_stack.empty()) {
		TickerNode* tmp = preorder_stack.top();
		preorder_stack.pop();
		if (tmp->isATicker) {
			result.push_back(tmp->value);
		}
		//push children from right to left
		if (tmp->children.size() != 0) {
			for (auto i = --(tmp->children).end(); i != tmp->children.begin(); i--) {
				preorder_stack.push(&(i->second));
			}
			preorder_stack.push(&(tmp->children.begin()->second));
		}
	}
	return result;
}

//search all tickers start with "ticker"
std::vector<std::string> TickerNode::search(std::string ticker){
	TickerNode* position = this;
	std::string prefix;
	while (ticker.size()) {
		std::string temp = ticker.substr(0, 1);
		unsigned long length = ticker.size();
		std::string body = ticker.substr(1, length - 1);
		//position = &(position->children[temp]);
		//previous one is risky, because ticker maybe invalid
		auto i = position->children.find(temp);
		if(i != position->children.end()){
			position = &(i->second);
			ticker = body;
			prefix += temp;
		}
		else {
			return std::vector<std::string>();
		}
		
	}
	//return all children from this position, including this position(if it is a valid ticker)
	return position->preOrder();
}

