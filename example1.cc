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
     *
     *      1. BoolOption   : will be set if the flag is specified.
     *                        it doesn't need more arguments, ie:
     *                          "-d" or "--debug"
     *
     *      2. StringOption : will be set if specified, and the value will be
     *                        the next argument, ie:
     *                          "-uMariano" or "-u Mariano" or
     *                          "--username Mariano" or "--username=Mariano"
     *
     *      3. IntegerOption: will be set if specified, and the value will be
     *         FloatOption  : the next argument, ie:
     *         DoubleOption :   "-p23" or "-p 23" or "--port 23" or "--port=23"
     *
     *     Options can be mandatory, most of them can have a default value and
     *     passing the description information will autogenerate the usage legend,
     *     ie:
     *
     *          BoolOption    debug   ('d', "debug",        false, "enables the debug mode");
     *          StringOption  username('u', "username",     true , "set the username");
     *          IntegerOption port    ('p', "port",         false, 23, "server port");
     *          FloatOption   port    ('n', "portability",  false, 1.11, "smart option...");
     *
     *     Note that
     *      
     *      + the first argument specifies the character to be used as the short option
     *      + the second argument specifies the string to be used as the long option
     *      + the third argument tells if the option is mandatory (if not specified
     *        and error will be printed and the program will stop)
     *      + in the cases of Options that receives values (StringOption, IntegerOption, etc...)
     *        a default value can be specified
     *      + the description to be used in the usage text
     *
     *    In our previous example:
     *
     *       $ ./example2
     *       The following arguments are mandatory: -u|--username
     *       Usage: ./example2 [-h|--help] [-d|--debug] -u|--username value [-p|--port value] [-n|--portability value]
     *       Options:
     *        -h, --help                             print this help
     *        -d, --debug                            enables the debug mode
     *        -u, --username value                   set the username
     *        -p, --port value                       server port
     *        -n, --portability value                smart option...
     *
     *    It also tries to do full matching of the option, and if it's not successfull then it
     *    tries to get the option that best matches the argument. It can also detect ambiguities, ie:
     *
     *        -p, --port           value
     *        -n, --portability    value
     *
     *       $ ./example2 --por
     *       Option 'por' is ambiguous: port, portability
     *       Usage: ./example2 [-h|--help] [-d|--debug] -u|--username value [-p|--port value] [-n|--portability value]
     *       Options:
     *        -h, --help                             print this help
     *        -d, --debug                            enables the debug mode
     *        -u, --username value                   set the username
     *        -p, --port value                       server port
     *        -n, --portability value                smart option...
     *
     *    Also, it's case independent, so, "--port" will be equal to "--PoRt"
     *
     *  b. Now, the easy part: create the parser, add the options and parse it:
     *
     *       Parser parser;
     *
     *       parser.addOption(debug)
     *             .addOption(username)
     *             .addOption(port)
     *             .addOption(portability);
     *
     *      vector<string> otherArguments = parser.parse(argc, argv);
     *
     *     Note that the "parse()" will return you the other arguments that were specified in the
     *     command line and that weren't options, ie:
     *
     *      $ ./example2 --username=Mariano ARGUMENT1 -p 34 ARGUMENT2 --debug ARGUMENT3 ARGUMENT4
     *
     *     then, the otherArguments vector will contain:
     *
     *          { ARGUMENT1, ARGUMENT2, ARGUMENT3, ARGUMENT4 }
     *
     *     we do this in order to provide an easy way to recover them without needed to do some
     *     extra parsing of argv.
     *
     *     Later, on your program, you can use the following methods to inspect the options:
     *
     *          a. bool isSet() : will return you true if the option was specified, false otherwise
     *
     *          b. TYPE getValue(): the value specified for this option or default value it was not
     *                              specified. TYPE will depend on the type of option, ie:
     *                                      BoolOption    will return a bool
     *                                      StringOption  will return a string
     *                                      IntegerOption will return an int
     *                                      so on...
     *                                                                                    
     *
     *      Also please note, that's highly convenient to check is the option was set, prior to get
     *      the value (specially in non-defaulted value cases)
     *
     *
     *
     *  Full Example:
     *  ============
     *
     *      BoolOption    debug         ('d', "debug",      false, "enables the debug mode");
     *      StringOption  username      ('u', "username",   true , "set the username");
     *      IntegerOption port          ('p', "port",       false, 23, "server port");
     *      FloatOption   portability   ('n', "portability",false, "smart option...");
     *
     *
     *      Parser parser;
     *
     *      parser.addOption(debug)
     *            .addOption(username)
     *            .addOption(port)
     *            .addOption(portability);
     *
     *      vector<string> otherArguments = parser.parse(argc, argv);
     *
     *      if ( debug.isSet() ) {
     *          cout << "debug flag was set" << endl;
     *      }
     *
     *      // username is mandatory, so we don't need to check it
     *      cout << "username was set to '" << username.getValue() << "'" << endl;
     *
     *      // port has a default value, so we can choose not to check it
     *      // and work with the default value transparently
     *      cout << "port was set to '" << port.getValue() << "'" << endl;
     *
     *
     *      if ( portability.isSet() ) {
     *          cout << "portability was set to '" << portability.getValue() << "'" << endl;
     *      }
     *
     *
     *      // if other arguments were specified, we can easily access them
     *      if ( ! otherArguments.empty() ) {
     *
     *          cout << "otherArguments: ";
     *
     *          bool isFirst = true;
     *
     *          for(vector<string>::iterator argIter = otherArguments.begin();
     *              argIter != otherArguments.end();
     *              ++argIter
     *          ) {
     *
     *              if ( !isFirst )
     *                  cout << ", ";
     *
     *              cout << "'" << *argIter << "'";
     *
     *              isFirst = false;
     *          }
     *
     *          cout << endl;
     *
     *      }
     *
     *  As an example of its execution:
     *
     *      $ ./example2 arg1 -u Mariano arg2 --portab=2.3333 arg3 arg4 arg5
     *      username was set to 'Mariano'
     *      port was set to '23'
     *      portability was set to '2.3333'
     *      otherArguments: 'arg1', 'arg2', 'arg3', 'arg4', 'arg5'
     *
     */

    BoolOption    debug         ('d', "debug",      false, "enables the debug mode");
    StringOption  username      ('u', "username",   true , "set the username");
    IntegerOption port          ('p', "port",       false, 23, "server port");
    FloatOption   portability   ('n', "portability",false, "smart option...");
    // defaul value is the limit of the type. for int types => (sizeof(int) * 1024)
    IntegerRange  portRange     ('r', "portrange",  true, "range of ports");


    Parser parser;

    parser.addOption(debug)
          .addOption(username)
          .addOption(port)
          .addOption(portability)
          .addOption(portRange);

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

    if ( portRange.isSet() ) {
        std::list<int> list = portRange.getValue();
        if ( list.size() > 0 ) {
            cout << "Range seted from '" << list.front() << "' to '" << list.back() << "'." << endl;
        } else {
            cout << "is null" << std::endl;
        }
    }
 
    // if other arguments were specified, we can easily access them
    if ( ! otherArguments.empty() ) {

        cout << "otherArguments: ";

        bool isFirst = true;

        for(vector<string>::iterator argIter = otherArguments.begin();
            argIter != otherArguments.end();
            ++argIter
        ) {

            if ( !isFirst )
                cout << ", ";

            cout << "'" << *argIter << "'";

            isFirst = false;
        }

        cout << endl;

    }


}

