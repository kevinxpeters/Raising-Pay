//*****************************************************************************
// RaisingPay.cpp
// Author: Kevin Peters
// Date: October 23, 2017
// Class: CIS 350, Bruce Elenbogen
// Description/Purpose: The purpose of this program is to use multiway trees to build out a company's org chart.
// With that org chart you can do calculations that we will build out in part2
// We use the forest class as a vector of trees that will all combine into tree[0] once it is complete. 
// A tree is made up of Nodes with an id, minRequests4Raise and underlings (vector of nodes)
// Using the find functions we will traverse the tree to check if the boss or employee are in the already
// Based off of that we will have four scenerios.
//  -Boss and employee is found
//  -Boss is found, employee is not
//  -Boss not found, employee is found
//  -Boss not found, employee not found
// I have created an insert for each scenerio because I think it requires less to pass in and each insert function is only one line of code
// The program should loop until the user inputs 0 0. To loop correctly I have used similar logic to the print function,
// but instead of printing each node, i delete them to clear out for the next calculation
// The tree loops through the root and then the root's underlings and then the underling's underlings recursively
// This program should consider a company that's philosophy is that either all employees get a raise or no employees get a raise.
// In this company each employee (except the owner) has one boss. The boss only sends the raise request up if t% of employees directly 
// under them is greater than t%. If the owner receives more than t% requests, then all workers will get a raise. This program will 
// determine the minimum number of workers needed to make requests so that all workers will receive a raise.

// Input:  int employees (not including owner)
//		   int minEmpPercent (minimum percent of employees that must ask for a raise to move up to the next boss's level)
//		   int employeeBoss (this is the boss of the employee ie - 0 0 0 - emp 1 reports to the owner, 2 reports to owner, 3 reports to owner)
// Output: prints out vector<Tree> trees (should print out your tree with 5 spaces indented each level
//		   int minNum4Raise (min to pass up to owner for a raise) This will be added in part 2

//*****************************************************************************

/********************INCLUDES**************************************************/

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

/********************DATA TYPES************************************************/

struct Node {
	int id;
	int minRequests4Raise;
	std::vector< Node *> underlings;
	Node(int n = 0); //constructor
};

class Tree {
private:
	Node * root;
	void print(Node*ptr, int level);//private version of print
	void printMinWorkers(Node*ptr);
	void calcMin(Node*ptr, int minRaisePerc);//private version of calc
	void calcTermNodes(Node * ptr, int minRaisePerc);
	void clear(Node*ptr, int level);//private version of clear, added
	Node * find(Node*root, int target);//private version of find
public:
	Tree(Node * rt = nullptr) {
		root = rt;
	}
	Tree(int id) {
		root = new Node;
		root->id = id;
		root->minRequests4Raise = 0;
	}
	Tree(int boss, int emp) {
		root = new Node;
		root->id = boss;
		root->underlings.push_back(new Node(emp));
	}
	Node * getRoot();
	void printTree();//public print
	void printMin();
	void calcMinTree(int minRaisePerc);
	void clearTree();//public print
	Node * find(int target);//public find
};

class Forest {
private:
	std::vector< Tree *> trees;
public:
	void print();
	void printMin();
	Node * find(int target);
	void insert(int boss, int underling);
	void insertNBNE(int boss, int underling);//added
	void insertNBE(int boss, int underling);//added
	void insertBNE(int boss, int underling);//added
	void insertBE(int boss, int underling);//added
	Forest();
	void clear();
	void calcMin(int percThreshold);
};

//******************************************************************
// Node(int n)
// Purpose: Node struct contructor.
// Pre:  N > 0 and less <= total workers excluding owner
// Post: Creates a node employee with the ID passed in
//******************************************************************
Node::Node(int n)
{
	id = n;
	minRequests4Raise = 0;
}

bool sortRoot(Node* a, Node *b) {
	return a->minRequests4Raise < b->minRequests4Raise;
}

//******************************************************************
// print(Node * ptr, int level)
// Purpose: Recursively print the tree incrementing the level to indent to the right level in the tree
// Pre:  ptr should point to root emp to start printing, level should be >= 0 in relation to the employees level in the org
// Post: Recursively print the org chart, indenting 5 spaces for each level down the employee is 
//******************************************************************
void Tree::print(Node * ptr, int level)
{
	Node * root = ptr;
	if (root != nullptr) {
		std::cout << std::setw(level * 5);
		std::cout << root->id << std::endl;
		level++;
		for (int i = 0; i < root->underlings.size(); i++) {
			std::cout << std::setw(level * 5);
			std::cout << root->underlings[i]->id << std::endl;
			for (int j = 0; j < root->underlings[i]->underlings.size(); j++) {
				print(root->underlings[i]->underlings[j], level + 1);
			}
		}
	}
}

//******************************************************************
// printMinWorkers(Node * ptr)
// Purpose: Pass in the root (owner) and print out the minimum workers that are needed to pass up for everyone to receive a raise
// Pre:  Node's pointer passed in should be a valid employee
// (as used in main, pass in the owner to display all employees needed to pass up, but could be used with any employee's pointer to display the related min emp's)
// Post: Print out the minimum workers that are needed to pass up for everyone to receive a raise
//******************************************************************
void Tree::printMinWorkers(Node * ptr) {
	Node * root = ptr;
	if (root != nullptr) {
		std::cout << root->minRequests4Raise << std::endl;
	}
}

//******************************************************************
// calcMin(Node * ptr, int minRaisePerc)
// Purpose: Recursively traverse the tree to calculate all the inner nodes min workers required for a raise
// Pre:  Node's pointer passed in should be a valid employee, min raise should be a valid int between 0 and 100
// Post: Calculates the minimum workers required for a specific Node passed in
//******************************************************************
void Tree::calcMin(Node * ptr, int minRaisePerc)
{
	calcTermNodes(ptr, minRaisePerc);
	Node * root = ptr;
	int minToPass, passInc = 0;
	bool noMinCalc = false;
	if (root != nullptr) {
		if (root->minRequests4Raise == 0) {
			for (int i = 0; i < root->underlings.size(); i++) {
				//if the node doesn't have a min, set it here
				if (root->underlings[i]->minRequests4Raise == 0) {
					noMinCalc = true;
					calcMin(root->underlings[i], minRaisePerc);
					for (int j = 0; j < root->underlings[i]->underlings.size(); j++) {
						if (root->underlings[i]->underlings[j]->minRequests4Raise == 0) {
							calcMin(root->underlings[i]->underlings[j], minRaisePerc);
						}
					}
				}
			}
				minToPass = ceil((minRaisePerc * .01) * root->underlings.size());
				std::sort(root->underlings.begin(), root->underlings.end(),sortRoot);
				for (int i = 0; i < minToPass; i++) {
					passInc = passInc + root->underlings[i]->minRequests4Raise;
				}
				root->minRequests4Raise = passInc;
		}
	}
}

//******************************************************************
// calcTermNodes(Node * ptr, int minRaisePerc)
// Purpose: Recursively traverse the tree to calculate all the terminal nodes min workers required for a raise (terminal nodes always = 1)
// Pre:  Node's pointer passed in should be a valid employee, min raise should be a valid int between 0 and 100
// Post: Recursively sets all terminal node's minRequest4Raise = 1
//******************************************************************
void Tree::calcTermNodes(Node * ptr, int minRaisePerc)
{
	Node * root = ptr;
	int minToPass, passInc = 0;
	if (root != nullptr) {
		if (root->underlings.size() == 0) {
			root->minRequests4Raise = 1;
		}
		else {
			for (int i = 0; i < root->underlings.size(); i++) {
				if (root->underlings[i]->minRequests4Raise == 0) {
					if (root->underlings[i]->underlings.size() == 0) {
						root->underlings[i]->minRequests4Raise = 1;
					}
					else {
						for (int j = 0; j < root->underlings[i]->underlings.size(); j++) {
							calcTermNodes(root->underlings[i]->underlings[j], minRaisePerc);
						}
					}
				}
			}
		}
	}
}

//******************************************************************
// clear(Node*ptr, int level) 
// Purpose: This function is used after the tree prints to clear tree to use for the next input 
// Pre:  Node's pointer should be the owner at level 0 so the whole tree is deleted
// Post: Works similar to the print tree, deleting as it traverses the tree
//******************************************************************
void Tree::clear(Node*ptr, int level) 
{
	Node * root = ptr;
	if (root != nullptr) {
		level++;
		for (int i = 0; i < root->underlings.size(); i++) {
			for (int j = 0; j < root->underlings[i]->underlings.size(); j++) {
				clear(root->underlings[i]->underlings[j], level + 1);
			}
			delete root->underlings[i];
			root->underlings[i] = nullptr;
		}
		delete root;
		root = nullptr;
	}
}
//******************************************************************
// find(Node * root, int target)
// Purpose: This loops through the root and any underlings and 
// recursively searchs for an employee and returns either the node to that employee or nullptr
// Pre:  
// Post: 
//******************************************************************
Node * Tree::find(Node * root, int target)
{
	bool targetFound = false;
	if (root == nullptr) {
		return nullptr;
	}
	else if (root->id == target) {
		return root;
		targetFound = true;
	}
	else if (root->id != target) {
		for (int i = 0; i < root->underlings.size(); i++) {
			if (root->underlings[i]->id == target) {
				return root->underlings[i];
				targetFound = true;
			}
			else {
				if (find(root->underlings[i], target) != nullptr) {
					return find(root->underlings[i], target);
					targetFound = true;
				}
			}
		}
	}
	else {
		return nullptr;
	}
	return nullptr;
}

//******************************************************************
// getRoot()
// Purpose: Not used in part1
// Pre:  
// Post: 
//******************************************************************
Node * Tree::getRoot()
{
	return nullptr;
}

//******************************************************************
// printTree()
// Purpose: Public Tree print function that calls the private print function with more parameters
// Pre:  None
// Post: Calls private print function with private parameters
//******************************************************************
void Tree::printTree()
{
	int level = 0;
	print(root, level);
}

//******************************************************************
// printMin()
// Purpose: Public Tree min worker print function that calls the private min worker print function with more parameters
// Pre:  None
// Post: Calls private function with private parameters
//******************************************************************
void Tree::printMin() {
	printMinWorkers(root);
}

//******************************************************************
// calcMinTree(int minRaisePerc)
// Purpose: Public Tree min worker calc function that calls the private min worker print function with more parameters
// Pre:  0 < MinRaisePerc =< 100
// Post: Calls private function with private parameters
//******************************************************************
void Tree::calcMinTree(int minRaisePerc) {
	calcMin(root, minRaisePerc);
}
//******************************************************************
// clearTree()
// Purpose: Public Tree clear function that calls the private clear function with more parameters
// Pre:  None
// Post: Calls private function with private parameters
//******************************************************************
void Tree::clearTree()
{
	int level = 0;
	clear(root, level);
}
//******************************************************************
// find(int target)
// Purpose: Public Tree find function that calls the private find function with more parameters
// Pre:  Should pass in a 
// Post: 
//******************************************************************
Node * Tree::find(int target)
{
	return find(root, target);
}

//******************************************************************
// print()
// Purpose: Forest print function used to loop through each tree in the forest and call printTree for each 
// Pre:  
// Post: 
//******************************************************************
void Forest::print()
{
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->printTree();
	}
}

void Forest::printMin() {
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->printMin();
	}
}
//******************************************************************
// find(int target)
// Purpose: The forest find is used to loop through each tree in the forest and call the tree's find function to find or return nullptr
// Pre:  target =< numEmp (can't find an employee greater than the amount of employees in the company)
// Post: Returns either a pointer to the target's Node or nullptr if not found
//******************************************************************
Node * Forest::find(int target)
{
	Node * targetFoundNode;
	targetFoundNode = nullptr;
	for (int i = 0; i < trees.size(); i++) {
		if (trees[i]->find(target) != nullptr && targetFoundNode == nullptr) {
			targetFoundNode = trees[i]->find(target);
			return trees[i]->find(target);
		}
	}if (targetFoundNode == nullptr) {
		return nullptr;
	}
	else {
		return targetFoundNode;
	}
}

//general insert not used. see below functions
void Forest::insert(int boss, int underling)
{
	//used in 4 different insert functions, this function is not used
	//I think this appoarch is more straight forward since there are 4 scenerios that would insert different ways
	// and this way uses if statements and each function is simply one line of code
}

//******************************************************************
// insertNBNE(int boss, int underling)
// Purpose: Inserts a new tree in the forest if neither boss nor employee are found
// Pre:  Boss => 0 underlings > 0 and =< numEmployees
// Post: Creates a new Tree in the trees vector 
//******************************************************************
void Forest::insertNBNE(int boss, int underling) {
	trees.push_back(new Tree(boss, underling));
}

//******************************************************************
// insertNBE(int boss, int underling)
// Purpose: Inserts a new tree in the forest where the underling's node is if the boss is not found, but the employee is
// Pre:  Boss => 0 underlings > 0 and =< numEmployees
// Post: Creates a new Tree in the trees vector 
//******************************************************************
void Forest::insertNBE(int boss, int underling) {
	trees.push_back(new Tree(find(underling)));
}

//******************************************************************
// insertBNE(int boss, int underling)
// Purpose: Inserts a new node where it finds the boss's underlings if the boss is found, but the employee is not
// Pre:  Boss => 0 underlings > 0 and =< numEmployees
// Post: Creates a new Node for the underling of the boss passed in
//******************************************************************
void Forest::insertBNE(int boss, int underling) {
	find(boss)->underlings.push_back(new Node(underling));
}

//******************************************************************
// insertBE(int boss, int underling)
// Purpose: Inserts the employee node where it finds the boss's underlings if the boss is found and the employee is found
// Pre:  Boss => 0 underlings > 0 and =< numEmployees
// Post: Inserts the Node for the underling as the passed in underling's boss
//******************************************************************
void Forest::insertBE(int boss, int underling) {
	find(boss)->underlings.push_back(find(underling));
}

//******************************************************************
// Forest()
// Purpose: Forest construction to create a forest with a tree at position 0
// Pre:  None, default constructor
// Post: Adds a new tree to the trees vector
//******************************************************************
Forest::Forest()
{
	//1st tree should always be CEO
	trees.push_back(new Tree(0));
}

//******************************************************************
// clear()
// Purpose: Forest clear function used to loop through each tree in the forest and call clearTree for each 
// Pre:  None
// Post: Loops through all trees in the vector to clear each tree
//******************************************************************
void Forest::clear()
{
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->clearTree();
	}
}

//******************************************************************
// calcMin(int percThreshold)
// Purpose: Forest calc function used to loop through each tree in the forest and call calcMinTree for each
// Pre:  0 =< percThreshold < 100
// Post: Loops through all trees in the vector to calculate min workers for each tree
//******************************************************************
void Forest::calcMin(int percThreshold) {
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->calcMinTree(percThreshold);
	}
}

//******************************************************************
// readInput(Forest myFor, int employees, int threshold)
// Purpose: This function reads in all the input and does the logic to decide how to find and insert the employees
// Pre:  Forest must be a defined object, employees > 0, 0 =< threshold =< 100
// Post: Loop through all employees and insert in the tree where appropriate
//******************************************************************
void readInput(Forest myFor, int employees, int threshold) {
	int boss;
	Node * bossFound;
	Node * empFound;
	for (int i = 0; i < employees; i++) {
		//std::cout << "enter boss of employee " << i + 1 << std::endl;
		std::cin >> boss;
		bossFound = myFor.find(boss);
		empFound = myFor.find(i + 1);

		//didn't find the boss, finds the employee
		if (bossFound == nullptr && empFound) {
			myFor.insertNBE(boss, i + 1);
		}//didn't find the boss, din't find the employee
		else if (bossFound == nullptr && empFound == nullptr) {
			myFor.insertNBNE(boss, i + 1);
		}//finds the boss, finds the employee
		else if (bossFound && empFound) {
			myFor.insertBE(boss, i + 1);
		}//finds the boss, didn't find the employee
		else if (bossFound && empFound == nullptr) {
			myFor.insertBNE(boss, i + 1);
		}
	}
}
int main() {
	int numEmp, raiseThreshold, minWorkers;
	Tree bossTree;
	Forest myForest;
	do {
		std::cin >> numEmp;
		std::cin >> raiseThreshold;
		if (numEmp != 0 || raiseThreshold != 0) {
			readInput(myForest, numEmp, raiseThreshold);
			//myForest.print();
			myForest.calcMin(raiseThreshold);
			myForest.printMin();
			myForest.clear();
		}
	} while (numEmp != 0 || raiseThreshold != 0);
	return 0;
}
