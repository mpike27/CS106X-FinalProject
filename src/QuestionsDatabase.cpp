/**
 * Name: Max Pike
 * --------------
 * QuestionsDatabase
 * --------------
 * The database class parses the questions and answers into a format that is easy to access
 * and manipulate.  It also is responsible for determining the best questions to ask so that the
 * response to the question evenly divides the possibilities, so that the computer can figure out
 * what they word is that the user is thinking of. It also updates the probabilites for each answer after each response and
 * refines the list by removing outliers that are no longer possibilities.
 */

#include <iostream>
#include "fstream"
#include "console.h"
#include "filelib.h"
#include "strlib.h"
#include "QuestionsDatabase.h"
#include "hashmap.h"
#include "tokenscanner.h"
#include "dividemap.h"
using namespace std;

/* Constants */
static const string headerOne = "WIKICAT";
static const string headerTwo = "WORDNET";
static const int initialSize = 500;
static const int maxNumQuestions = 20;
static const string SENTINEL = "EMPTY_SET";
static const double thresholdValue = 0.75;
static const int minNumPossibilities = 3;
static const int questionsScalingFactor = 5;
static const int answersScalingFactor = 2;

/**
 * Function: QuestionsDatabase::QuestionsDatabase
 * ----------------------------------------------
 * Initializes a dynamically allocated SparseGrid with the inital dimensions defined in the constants.
 */
QuestionsDatabase::QuestionsDatabase() {
    boolGrid = new SparseGrid<bool>(initialSize, initialSize);
};

/**
 * Function: QuestionsDatabase::~QuestionsDatabase
 * -----------------------------------------------
 * The destructor deletes the dynamically allocated SparseGrid to free up memory on the heap.
 */
QuestionsDatabase::~QuestionsDatabase() {
    delete boolGrid;
};

/**
 * Function: QuestionsDatabase::removeIncorrectGuess
 * -------------------------------------------------
 * Eliminates the incorrect guess from the subset of answers which the program is still considering.
 */
void QuestionsDatabase::removeIncorrectGuess(const string& guess) {
    probabilities.refineSubset(guess);
}

/**
 * Function: QuestionsDatabase::contains
 * -------------------------------------
 * Returns whether or not the given string is contained within the total database of possible solutions.
 */
bool QuestionsDatabase::contains(const string& response) {
    return probabilities.containsKey(response);
}

/**
 * Function: QuestionsDatabase::findDifference
 * -------------------------------------------
 * This function is called whenever the program cannot figure out what the user was thinking of
 * when the word is contained within the database.  It will iterate through the responses to
 * the questions that the program asked, and will alert the user of the questions that they
 * answered incorrectly for the solution.  If the user answered everything correctly, and the program
 * still does not answer it correctly (due to bad dataset), it will prompt the user with a different
 * statement.
 */
void QuestionsDatabase::findDifference(const string& response) {
    int counter = 0;
    while(!questionsAsked.isEmpty()) {
        questionInfo question = questionsAsked.dequeue();
        if(((*boolGrid)[probabilities.get(response).index][question.index] == true && !question.response) ||
                ((*boolGrid)[probabilities.get(response).index][question.index] != true && question.response)) {
            cout << "You incorrectly answered " << question.question << endl;
            counter++;
        }
    }
    if(counter == 0) cout << "I was about to get to that one.." << endl;
}

/**
 * Function: QuestionsDatabase::removeHeaders
 * ------------------------------------------
 * This function will remove the two main headers found within the sample file.  It will
 * cut out one of the two headers, as well as the series of digits found with header two.
 */
void QuestionsDatabase::removeHeaders(string& question) {
    if(stringContains(question, headerOne)) {
        question = trim(question.substr(headerOne.length()));
    }
    if(stringContains(question, headerTwo)) {
        question = trim(question.substr(headerTwo.length()));
        while(isdigit(question[question.length() - 1]))
            question = question.substr(0, question.length() - 1);
    }
}

/**
 * Function: QuestionsDatabase::showAnswerKey
 * ------------------------------------------
 * This is called to display all of the possible answers that the program knows of which the user can
 * pick for the game.  It will space them all out for easy readability.
 */
void QuestionsDatabase::showAnswerKey() {
    int counter = 0;
    Set<string> answers = probabilities.getMapKeys();
    for(string answer: answers) {
        if(counter++ % 5 == 0) cout << endl << endl;
        cout << answer << "     ";
    }
    cout << endl << endl;
}

/**
 * Function: QuestionsDatabase::readKey
 * ------------------------------------
 * This will seperate the string from within a token market with '<' and '>'.
 */
void QuestionsDatabase::readKey(ifstream& input, string& str, char& curr) {
    curr = input.get();
    while(curr != '>') {
        str += (curr == '_')? ' ': curr;
        curr = input.get();
    }
}

/**
 * Function: QuestionsDatabase::bestGuess
 * --------------------------------------
 * This function is called when the computer is forced to make a guess as to what
 * the word is that the user is thinking of.  It will return the answer from the remaining subset
 * with the highest probability of being correct.
 */
string QuestionsDatabase::bestGuess() {
    string mostProbable = "";
    int highestProbability = 0.0;
    Set<string> remainingAnswers = probabilities.getSubMapKeys();
    for(string answer: remainingAnswers) {
        if(probabilities.get(answer).prob > highestProbability) {
            mostProbable = answer;
            highestProbability = probabilities[answer].prob;
        }
    }
    return mostProbable;
}

/**
 * Function: QuestionsDatabase::updateDatabase
 * -------------------------------------------
 * This will update the internal database after the user answers the given question.  It will first add
 * the information with the last question into the questionsAsked queue, which is called only when
 * the user answers questions incorrectly.  This function will next remove the question from the map, so
 * as to not ask the same question more than once.  It then iteratively goes through the remaining possible
 * answers, and will update the answerInfo for each answer where the probabilites are increased for each
 * answer that corresponds with the response from the user for the question that the computer asked.
 * It keeps track of all of the probabilites that fall below the threshold value, and will then
 * refine the subset of the dividemap so that it can eliminate outliers.
 */
void QuestionsDatabase::updateDatabase(bool response, const string& question, int numQuestions) {
    questionInfo lastQuestion = {question, questions[question], response};
    questionsAsked.enqueue(lastQuestion);
    int questionIndex = questions[question];
    questions.remove(question);
    Set<string> toRemove;
    Set<string> currentCandidates = probabilities.getSubMapKeys();
    for(string answer: currentCandidates) {
        if(((*boolGrid)[probabilities[answer].index][questionIndex] == true && response) ||
                ((*boolGrid)[probabilities[answer].index][questionIndex] != true && !response)) probabilities[answer].prob++;
        if((double(probabilities[answer].prob) / double(numQuestions)) < thresholdValue) toRemove += answer;
    }
    for(string incorrectAnswer: toRemove)
        probabilities.refineSubset(incorrectAnswer);
}

/**
 * Function: QuestionsDatabase::getNextQuestion
 * --------------------------------------------
 * This deterministically finds the next question to ask the user.  It will first check to see
 * whether or not there are any remaining possibilities in the submap.  If so it will return a sentinel
 * string that alerts the main program that there are no possibilites left.  If the submap is small enough
 * or the computer is on its last question, it will return the best guess for what the user is thinking of,
 * and will return true to alert the main program that it is guessing the word and not a question.
 * Otherwise, it will iteratively go through the submap, and finds the question that most evenly splits the remaining
 * possible answer and sets it equal to the 'question' string passed in.
 */
bool QuestionsDatabase::getNextQuestion(string& question, const int numQuestion) {
    if(probabilities.subMapSize() == 0) {
        question = SENTINEL;
        return false;
    }
    if(probabilities.subMapSize() < minNumPossibilities || numQuestion == maxNumQuestions) {
        question = bestGuess();
        return true;
    }
    cout << "Hmm.. Ok Let me think..." << endl;
    string bestQuestion = "";
    double divide = 0.0;
    double answerKeySize = probabilities.subMapSize();
    for(string question: questions) {
        double counter = 0.0;
        Set<string> currentCandidates = probabilities.getSubMapKeys();
        for(string answer: currentCandidates)
            if((*boolGrid)[probabilities[answer].index][questions[question]]) counter++;
        if(abs(0.5 - (counter / answerKeySize)) < abs(0.5 - divide)) {
            divide = counter / answerKeySize;
            bestQuestion = question;
        }
    }
    question = bestQuestion;
    return false;
}

/**
 * Function: QuestionsDatabase::enlargeGrid
 * ----------------------------------------
 * This method increases the size of the dynamically allocated Sparse Grid while also
 * freeing memore associated with the old grid.  It increases the questions side
 * by a higher scaling factor since there tend to be more questions in the database than answers.
 */
void QuestionsDatabase::enlargeGrid() {
    SparseGrid<bool> *oldGrid = boolGrid;
    boolGrid = new SparseGrid<bool>(oldGrid->numRows() * answersScalingFactor, oldGrid->numCols() * questionsScalingFactor);
    for(int row = 0; row < oldGrid->numRows(); row++) {
        for(int col = 0; col < oldGrid->numCols(); col++) {
            (*boolGrid)[row][col] = (*oldGrid)[row][col];
        }
    }
    delete oldGrid;
}

/**
 * Function: QuestionsDatabase::readFile
 * This will read in the file and will store all of the appropriate information.  It
 * will first isolate the answer and will store it in the divideMap if it is not already contained
 * in it.  It will read in the question and will also store the question in its own separate map
 * if it is new.  It will then go to the appropriate location on the sparse grid, and will put
 * a true for the index associate with that particular quesiton and answer.  By doing this, it allows
 * for O(1) look up for a quesiton and answer.
 */
void QuestionsDatabase::readFile(ifstream& input) {
    cout << "Reading in File..." << endl << endl;
    while(input) {
        char curr = input.get();
        if(curr == '<') {
            string name;
            readKey(input, name, curr);
            if(!probabilities.containsKey(name)) {
                answerInfo answer = {0, probabilities.size()};
                probabilities.put(name, answer);
                if(boolGrid->numRows() < probabilities.size()) enlargeGrid();
            }
            while(curr != '<')
                curr = input.get();
            string question;
            readKey(input, question, curr);
            question = toUpperCase(question);
            removeHeaders(question);
            if(!questions.containsKey(question)) questions[question] = questions.size();
            if(questions.size() > boolGrid->numCols()) enlargeGrid();
            (*boolGrid)[probabilities[name].index][questions[question]] = true;
        }
    }
}
