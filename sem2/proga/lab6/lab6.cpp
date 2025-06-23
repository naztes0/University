//У текстовому файлі PROG - "Сі-програма", з ідентифікаторами довжиною не більше 9.
//Надрукувати в алфавітному порядку всі ідентифікатори, вказавши кількість входжень (великі й маленькі літери розрізняються).
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<cstring>
#include<stack>
using namespace std;

struct TreeNode {
	char identifier[10];
	int counter;
	TreeNode* left;
	TreeNode* right;

};
//Структура для симетричного обходу 
struct TreeNodeIndicator {
	TreeNode* node;
	bool rightDone;
};

//створення вузла 
TreeNode* createNode(const char* ident) {
	TreeNode* newNode = new TreeNode();
	strncpy(newNode->identifier, ident, 9);
	newNode->identifier[9] = '\0';
	newNode->counter = 1;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}
//порівнняня чарів за табл ASCII
int comparer(const char* s1, const char* s2) {
	return strcmp(s1, s2);
}
//Додавання нового вузла до дерева бінарного пошукку(НЕЗБАЛАНСОВАНЕ)
TreeNode* insertNode(TreeNode* root, const char* id) {
	if (!root) {
		return createNode(id);
	}
	int compare = comparer(root->identifier, id);
	if (compare == 0) {
		root->counter++;
	}
	if (compare < 0) {
		root->left = insertNode(root->left, id);
	}
	if (compare > 0) {
		root->right = insertNode(root->right, id);
	}
	return root;
}
//БЛОК ПЕРЕВІРОЧНИХ УМОВ
const char separators[] = " ,.*{}[]!=:;+()<>-'\\&|?	";
const char* keywords[] = { "int", "void", "bool", "struct", "const", "unsigned", "float", "double",
	"char", "if", "else", "while", "for", "do", "return","switch", "case", "break",
	"continue", "sizeof","true","false","cin","cout","using","namespace","NULL","nullptr"};

const int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

bool isSeparators(char c) {
	for (int i = 0; separators[i] != '\0'; i++) {
		if (c == separators[i]) return true;
	}
	return false;
}

bool isKeyword(const char* word) {
	for (int i = 0; i < numKeywords; i++) {
		if (strcmp(word, keywords[i]) == 0) return true;
	}
	return false;
}

bool isLetter(char c) {
	bool upperCase = (c >= 'A' && c <= 'Z');
	bool lowerCase = (c >= 'a' && c <= 'z');
	if (upperCase || lowerCase) return true;
	return false;
}

bool isDigit(char c) {
	bool digit = (c >= '0' && c <= '9');
	if (digit) return true;
	return false;

}
//Перевірка на ідентифікатор за усіма умовами
bool isIdentificator(const char* word) {
	if (!isLetter(word[0])) return false;
	if (isKeyword(word)) return false;
	for (int i = 1; word[i] != '\0'; i++) {
		if (!isLetter(word[i]) && !isDigit(word[i]) && word[i] != '_')
			return false;
	}

	return true;
}
//void processLineChecker(char* line, TreeNode*& root) {
//	char* word = strtok(line, separators);
//	while (word) {
//		// Видалення роздільників з кінця слова
//		while (isSeparators(word[strlen(word) - 1])) {
//			word[strlen(word) - 1] = '\0';
//		}
//		if (strlen(word) > 0 && strlen(word) <= 9) {
//			cout << "Found word: " << word << endl;  // Відладкове повідомлення
//			if (isIdentificator(word)) {
//				cout << "The identifier: " << word << endl;  // Відладкове повідомлення
//				root = insertNode(root, word);
//			}
//			else {
//				cout << "Not an identifier: " << word << endl;  // Відладкове повідомлення
//			}
//		}
//		word = strtok(NULL, separators);
//	}
//}


//Оброка рядка(без викорситання strtok)
void processLine(char* line, TreeNode*& bstRoot) {
	char* wordStart = NULL;//початок слова
	char* ptr = line;

	while (*ptr != '\0') {
		while (*ptr != '\0' && isSeparators(*ptr)) {
			++ptr;
		}
		if (*ptr == '\0') break;
		wordStart = ptr;
		while (*ptr != '\0' && !isSeparators(*ptr)) {
			++ptr;

		}
		char temp = *ptr;//зберігаємо роздільник
		*ptr = '\0';//ініціалізуємо кінець слова
		if (strlen(wordStart) > 0 && strlen(wordStart) <= 9) {
			if (isIdentificator(wordStart)) {
				bstRoot = insertNode(bstRoot, wordStart);
			}
		}
		*ptr = temp;//відновлення роздільника
		if (*ptr != '\0')++ptr;//перехід до нового слова 
	}

}
//Читання файлу 
void readF(TreeNode*& bstRoot) {
	FILE* f = fopen("PROG1.txt", "r");
	if (!f) {
		cerr << "Error: PROG id empty or cannot be opened!";
		return;
	}
	char line[256];
	while (fgets(line, sizeof(line), f)) {
		/*processLineChecker(line, root);*/
		processLine(line, bstRoot);
	}
	fclose(f);
}
void symetricOrder(TreeNode* root) {
	if (!root) return;
	stack<TreeNodeIndicator>s;
	s.push({ root,false });
	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();
		if (current.rightDone) {
			cout << "id:" << current.node->identifier << "\n";
			cout << "counter:" << current.node->counter << "\n\n";
		}
		else {
			if (current.node->left) {
				s.push({ current.node->left,false });
			}
			s.push({ current.node,true });
			if (current.node->right) s.push({ current.node->right,false });
		}
	}
}
int main() {
	TreeNode* root = NULL;
	readF(root);
	cout << "Binary search tree sorted:\n";
	symetricOrder(root);
	return 0;
}
