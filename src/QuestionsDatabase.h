#include <iostream>
#include <string>
#include "fstream"
#include "console.h"
#include "queue.h"
#include "sparsegrid.h"
#include "hashmap.h"
#include "set.h"
#include "dividemap.h"

using namespace std;

class QuestionsDatabase {
public:

    QuestionsDatabase();
    ~QuestionsDatabase();

    void readFile(ifstream& input);
    void showAnswerKey();
    bool getNextQuestion(string& question, const int numQuestion);
    void updateDatabase(bool response, const string& question, int numQuestions);
    void removeIncorrectGuess(const string& guess);
    bool contains(const string& response);
    void findDifference(const string& response);

private:
    struct questionInfo {
        string question;
        int index;
        bool response; // what the user answered for that question
    };
    struct answerInfo {
        int prob; // contains the number of similarities with the responses that the user enters
        int index;
    };

    SparseGrid<bool>* boolGrid;
    DivideMap<answerInfo> probabilities;
    HashMap<string, int> questions; // question to index
    Queue<questionInfo> questionsAsked;
    void enlargeGrid();
    void removeHeaders(string& question);
    void readKey(ifstream& input, string& str, char& curr);
    int selectBestQuestion(Set<int>& potentials);
    string bestGuess();
};
