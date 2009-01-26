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
    StringOption  username('u', "username", true, "Username to be used");
    IntegerOption port('p', "port", false, 23, "port where the server is listening");


    Parser parser;

    parser.addOption(debug)
	  .addOption(inputScript)
	  .addOption(username)
	  .addOption(port);


    vector<string> otherArguments = parser.parse(argc, argv);

    cout << "------------------" << endl;
    cout << "debug 	 isSet='" << debug.isSet() << "', getValue='" << debug.getValue() << "'" << endl;
    cout << "inputString isSet='" << inputScript.isSet() << "', getValue='" << inputScript.getValue() << "'" << endl;
    cout << "username    isSet='" << username.isSet() << "', getValue='" << username.getValue() << "'" << endl;
    cout << "port        isSet='" << port.isSet() << "', getValue='" << port.getValue() << "'" << endl;

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

