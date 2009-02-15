// g++ example1.cc -I. -o example1

#include <iostream>
#include <Parser.h>


using namespace std;

int main(int argc, char** argv) {

    /*
     * This is a simple example, to show the usage of this tool.
     * These are the main steps to use:
     *
     *  a. create all options, between set of the existing ones you have:
     *  b. Now, the easy part: create the parser, add the options and parse it
     *
     *  Please, take a look at the documentation in the header file "Parser.h"
     *  for a full description of each option
     *
     */

    BoolOption    debug         ('d', "debug",      false, "enables the debug mode");
    StringOption  username      ('u', "username",   true , "set the username");
    IntegerOption port          ('p', "port",       false, 23, "server port");
    FloatOption   portability   ('n', "portability",false, "smart option...");
    StringListOption  book      ('j', "book",       false, "book of interest. could be more than one");


    Parser parser;

    parser.addOption(debug)
          .addOption(username)
          .addOption(port)
          .addOption(portability)
          .addOption(book);

    vector<string> otherArguments = parser.parse(argc, argv);

    if ( debug.isSet() ) {
        cout << "debug flag was set" << endl;
    }

    // username is mandatory, so we don't need to check it
    cout << "username was set to '" << username.getValue() << "'" << endl;

    // port has a default value, so we can choose not to check it
    // and work with the default value transparently
    cout << "port was set to '" << port.getValue() << "'" << endl;


    if ( portability.isSet() ) {
        cout << "portability was set to '" << portability.getValue() << "'" << endl;
    }

    if ( book.isSet() ) {
        std::list<std::string> values = book.getValue();

        cout << "book was set with " << values.size() << " entries: ";

        for(std::list<string>::iterator entry = values.begin();
            entry != values.end();
            ++entry
        ) {
            if ( entry != values.begin() )
                cout << ", ";

            cout << "'" << *entry << "'";
        }

        cout << endl;

    }

 
    // if other arguments were specified, we can easily access them
    if ( ! otherArguments.empty() ) {

        cout << "otherArguments: ";

        for(vector<string>::iterator argIter = otherArguments.begin();
            argIter != otherArguments.end();
            ++argIter
        ) {

            if ( argIter != otherArguments.begin() )
                cout << ", ";

            cout << "'" << *argIter << "'";

        }

        cout << endl;

    }


}

