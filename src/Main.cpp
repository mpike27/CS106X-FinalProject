/**
 * Name: Max Pike
 * --------------
 * 20 Questions
 * --------------
 * This program mimics the novelty game "20 questions" where a user thinks of a word,
 * and the program will systematically be able to figure out what the word by asking up
 * to 20 questions.  This program is a simplified version of the actual program
 * which is a massive constantly evolving database.  "20 Questions" accepts in a file containing
 * many answers and assoicated categories/questions, and will determine what the answer is the user is thinking of.
 */

#include <iostream>
#include "fstream"
#include "console.h"
#include "simpio.h"
#include <string>
#include "strlib.h"
#include "QuestionsDatabase.h"

using namespace std;

/* Constants */
static const string sampleFile = "yagoTypes.tsv";
static const int maxNumQuestions = 20;
static const string SENTINEL = "EMPTY_SET";

/**
 * Function: loadDatabase
 * ----------------------
 * This prompts the user whether or not they want to use the sample database, or try to read in a file of their
 * own.  If they want to read in a file of their own, the program will alert the user of the format
 * that the text file has to be in, and that the questions have to be good enough so that the
 * computer can deduce what the user is thinking of.
 */
void loadDatabase(QuestionsDatabase& database) {
    ifstream input;
    cout << "You have the option to load in your own 20 questions database, " << endl;
    cout << "or you can use the sample database that is preloaded." << endl;
    if(getYesOrNo("Would you like to use the sample database?")) {
        input.open(sampleFile);
        database.readFile(input);
        if(getYesOrNo("Would you like to see the famous people, places or things that you can choose from?"))
            database.showAnswerKey();
    } else {
        cout << "If you want to read in your own file.  It must be in the format" << endl;
        cout << "<answer>  <question/category>" << endl;
        cout << "Also, you may want to keep in mind that if your dataset is not thorough" << endl;
        cout << "where there are not ample enough questions that evenly divide the dataset," << endl;
        cout << "then it will be tough for me to figure out what you are thinking of!" << endl;
        input.open(getLine("Enter filename: "));
        if(input.fail()) error("That was an invalid filename.");
        database.readFile(input);
    }
    input.close();
}

/** Prints the introduction as well as the rules for the game */
void printRules() {
    cout << "Welcome to '20 Questions'!!" << endl;
    cout << "This program mimics the classic game '20 Questions' where the user thinks" << endl;
    cout << "of a word, and the computer will guess that word within 20 questions." << endl;
    cout << "Less than '20 Questions' is a play on that in that it will read in a dataset" << endl;
    cout << "with a vast amount of answers and identifying categories and characterstics" << endl;
    cout << "and  will use that information to figure out which word the user is thinking of from within" << endl;
    cout << "the dataset" << endl;
    cout << "All of the questions will be yes or no questions.  Make sure to answer as accurately as possible." << endl;
    cout << "Good Luck!" << endl << endl;
}

/**
 * Function: giveUp
 * ----------------
 * If the computer does not know what the user is thinking of (either because of incorrect
 * responses or because of a subpar dataset), it will ask the user for the word that
 * they were thinking of.  If the word is new, it will alert the user of that, otherwise it
 * will display all of the questions that the user answered incorrectly.
 */
void giveUp(QuestionsDatabase& database) {
    string response = getLine("Hmm I am stumped. What was you word?");
    if(database.contains(response)) database.findDifference(response);
    else cout << "It seems as though that word was not in the database." << endl;
}

/**
 * Function: manageGame
 * --------------------
 * Initializes the database for the game, as well as controls the questions asked.  It will
 * call to the database to find the next question, and if that question is a guess,
 * it will guess the word that the user was thinking of.  If thew question is the
 * sentinel value, it will call the giveUp method and will break the loop.  Otherwise,
 * it will ask the question, and will update the database with the response that the computer
 * receives. After 20 questions have been asked, it will ask if they want to quit.
 */
void manageGame() {
    while(true) {
        QuestionsDatabase database;
        loadDatabase(database);
        getLine("Hit enter when you have selected a word for me to guess!");
        for(int i = 1; i <= maxNumQuestions; i++) {
            string question;
            if(database.getNextQuestion(question, i)) {
                if(getYesOrNo("Is the word that you were thinking of: " + question)) {
                    cout << "The computer wins agains!!!" << endl;
                    break;
                } else {
                    cout << "That is unfortunate.  Let me think." << endl;
                    database.removeIncorrectGuess(question);
                }
            } else {
                if(question == SENTINEL) {
                    giveUp(database);
                    break;
                } else {
                    cout << "Does it fit in the category ";
                    database.updateDatabase(getYesOrNo(question + "?"), question, i);
                }
            }
            if(i == maxNumQuestions) giveUp(database);
        }
        if(!getYesOrNo("Would you like to play again?")) break;
        cout << endl;
    }
}

/** Main */
int main() {
    printRules();
    manageGame();
    return 0;
}
