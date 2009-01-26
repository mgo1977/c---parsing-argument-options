// g++ example1.cc -I. -o example1

#include <iostream>
#include <Parser.h>


using namespace std;

int main(int argc, char** argv) {

    /*
     * I need to define:
     *   . short option
     *   . long option
     *   . has argument  <= where to store the value
     *   . is mandatory? <= impacts on usage() and automatic lack
     *			  of parameters detected
     *	 . default value 
     */

    BoolOption    debug('d', "debug", false, "enables the debug mode");
    StringOption  inputScript('i', "inputScript", false, "set the inputs script to be read");
    StringOption  myMandOption1('m', "mandatory-option-1", true, "example of mandatory 1");
    StringOption  myMandOption2('n', "mandatory-option-2", true, "example of mandatory 2");


    Parser parser;

    parser.addOption(debug)
	  .addOption(inputScript)
	  .addOption(myMandOption1)
	  .addOption(myMandOption2);


    vector<string> otherArguments = parser.parse(argc, argv);

    cout << "------------------" << endl;
    cout << "debug 	 isSet='" << debug.isSet() << "', getValue='" << debug.getValue() << "'" << endl;
    cout << "inputString isSet='" << inputScript.isSet() << "', getValue='" << inputScript.getValue() << "'" << endl;

    cout << "otherArguments: " << endl;
    cout << " ";

    bool isFirst = true;

    for(vector<string>::iterator argIter = otherArguments.begin();
	argIter != otherArguments.end();
	++argIter
    ) {

	if ( !isFirst )
	    cout << ", ";

	cout << *argIter;

	isFirst = false;
    }

    cout << endl;



}

