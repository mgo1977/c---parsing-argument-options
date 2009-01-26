#include <iostream>
#include <string>
#include <vector>
#include <sstream>

/*
 * The goal is able to simple parse cmd line options:
 *
 *  program [--debug|-d] [--file=prueba|--file prueba|-f prueba]
 *
 */

using namespace std;

class BaseOption {

    public:

    static const char NO_OPTION = 0;

    BaseOption(char sOption, const char* lOption, bool mandat, bool fArgument, const char* descr = "")
     : shortOption(sOption), longOption(lOption), mandatory(mandat), followsArgument(fArgument), description(descr),
       found(false)
    {}

    virtual
    bool isSet() {
	return found;
    }

    virtual
    string getValue() const {
	return value;
    }

    void setValue(const char* readValue) {
	found = true;
	value = readValue;
    }

    void markAsFound() {
	found = true;
    }

    bool matches(char shOption) {
	return ( shOption == shortOption );
    }

    bool needArgument() const { return followsArgument; }
    bool isMandatory()  const { return mandatory; }

    bool hasShortOption() {
	return (shortOption != NO_OPTION);
    }

    bool hasLongOption() {
	return !longOption.empty();
    }

    char getShortOption()  const { return shortOption; }
    string getLongOption() const { return longOption;  }


    string getDescription() const { return description; }

    protected:
    // configuration
    char    shortOption;
    string  longOption;
    bool    mandatory;
    bool    followsArgument;
    string  description;

    // status & value
    bool    found; // was it found?
    string  value;

};


template<typename TYPE>
class Option : public BaseOption {

    public:
    Option(char sOption, const char* lOption, bool mandatory, bool fArgument, const char* descr = "")
     : BaseOption(sOption, lOption, mandatory, fArgument, descr)
    {}

    Option(char sOption, const char* lOption, bool mandatory, bool fArgument, const TYPE& defValue, const char* descr = "")
     : BaseOption(sOption, lOption, mandatory, fArgument, descr), defaultValue(defValue)
    {}

    // implement according the Option type...
    // only the dev can know how to convert from a string
    // to a TYPE type
    virtual TYPE getValue() = 0;

    protected:
    // configuration
    TYPE    defaultValue;
};

class BoolOption : public Option<bool> {

    public:

    BoolOption(char sOption, const char* lOption, bool mandatory, const char* descr)
     : Option<bool>(sOption, lOption, mandatory, false, descr)
    {}

    bool getValue() {
	if ( ! found )
	    return false;
	else
	    return true;
    }

};

class StringOption : public Option<string> {

    public:

    StringOption(char sOption, const char* lOption, bool mandatory, const char* descr)
     : Option<string>(sOption, lOption, mandatory, true, descr)
    {}

    StringOption(char sOption, const char* lOption, bool mandatory, const char* defValue, const char* descr)
     : Option<string>(sOption, lOption, mandatory, true, defValue, descr)
    {}

    string getValue() {
	if ( ! found )
	    return defaultValue;
	else
	    return value;
    }

};


class Parser {

    public:
    Parser() : helpOption('h', "help", false, "print this help") {
	// we provide the help option by default
	addOption(helpOption);
    }

    Parser& addOption(BaseOption& option) {
	options.push_back(&option);
	return *this;
    }

    vector<string> parse(int argc, char** argv);

    void usage(const string& text) { usage(text.c_str()); }
    void usage(const char* text = "");

    private:
    BoolOption helpOption;

    vector<BaseOption*> options;
    string programName;

    BaseOption* findOption(char shortOpt);

    string getSummaryOptionText(BaseOption* option) {
	return getOptionText(option, "|");
    }

    string getFullOptionText(BaseOption* option) {
	return getOptionText(option, ", ");
    }

    string getOptionText(BaseOption* option, const char* separator);

};


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

vector<string>
Parser::parse(int argc, char** argv) {

    vector<string> otherArguments;

    // first argument is the program name
    if ( argc > 1 )
	programName = argv[0];

    // now, start iterating over each argument
    for(int argNumber=1; argNumber < argc; ++argNumber) {

	string argument = argv[argNumber];

	// arguments start with "-"
	// if not, push it into "other inputs"
	if ( argument.empty() )
	    continue;

	if ( argument[0] != '-' ) {
	    // add it as other argument and continue with the next arg
	    otherArguments.push_back( argument );
	    continue;	
	}

	// this is a malformed argument:
	// "-"
	if ( argument.length() < 2 ) {
	    stringstream error;
	    error << "Malformed argument! (see arg number " << argNumber << ")";
	    usage(error.str());
	}

	// now, if the next char is a '-' it's a long option,
	// if not, it's a short one
	if ( argument[1] != '-' ) {

	    // this looks like a short option, so let's check if there
	    // are no more chars here, if not, that's malformed
	    if ( argument.length() > 2 ) {
		stringstream error;
		error << "Malformed argument! (see arg number " << argNumber << ")";
		usage(error.str());
	    }

	    BaseOption* option = findOption(argument[1]);

	    if ( option == NULL ) {
		stringstream error;
		error << "Unknown option '" << argument << "' (see arg number " << argNumber << ")";
		usage(error.str());
	    }

	    // let's see if this needs an argument
	    if ( option->needArgument() ) {

		// try to get the next one or fail
		if ( argNumber+1 < argc ) {

		    // let's move to the next argument
		    argNumber++;

		    option->setValue( argv[argNumber] );

		}
		else {
		    stringstream error;
		    error << "Option '" << argument << "' needs an additional argument";
		    usage(error.str());
		}

	    }
	    else {
		// set as read
		option->markAsFound();
	    }

	}

    }

    // now, let's do some basic checking

    // was the help option requested?
    if ( helpOption.isSet() ) {
	usage();
    }

    // let's go thru all the options to get all the
    // ones that are mandatories and that weren't
    // set
   
    string mandatoriesError; 

    for(vector<BaseOption*>::iterator iter = options.begin();
	iter != options.end();
	++iter
    ) {

	BaseOption* option = *iter;

	if ( option->isMandatory() && !option->isSet() ) {

	    if ( ! mandatoriesError.empty() )
		mandatoriesError += ", ";

	    mandatoriesError += getSummaryOptionText(option);
	}

    }

    if ( ! mandatoriesError.empty() ) {
	usage("The following arguments are mandatory: " + mandatoriesError);
    }

    return otherArguments;
}


void
Parser::usage(const char* text) {

    if ( strcmp(text, "") != 0 ) {
	cerr << text << endl;
    }

    // add the options
    cerr << "Usage: ";
    cerr << programName << " ";

    stringstream optionsSummary;
    stringstream fullDescription;


    for(vector<BaseOption*>::iterator iter = options.begin();
	iter != options.end();
	++iter
    ) {

	BaseOption* option = *iter;

	// this is the syntax:
	//   [ ] => optional
	//   short|long
	string summaryOptionBase = getSummaryOptionText(option);
	string fullOptionBase    = getFullOptionText(option);

	if ( option->needArgument() ) {
	    summaryOptionBase += " value";
	    fullOptionBase    += " value";
	}

	// summary 
	if ( ! option->isMandatory() ) {
	    optionsSummary << "[" << summaryOptionBase << "]";
	}
	else {
	    optionsSummary << summaryOptionBase;
	}

	optionsSummary << " ";

	// full description
	fullDescription << " " << fullOptionBase << "\t\t" << option->getDescription() << endl;

    }

    cerr << optionsSummary.str() << endl;
    cerr << "Options:" << endl;
    cerr << fullDescription.str();

    // end
    exit(1);

} 

string
Parser::getOptionText(BaseOption* option, const char* separator) {

    string optionBase;

    if ( option->hasShortOption() ) {
	optionBase += "-";
	optionBase += option->getShortOption();
    }

    if ( option->hasLongOption() ) {

	if ( option->hasShortOption() )
	    optionBase += separator;

	optionBase += "--" + option->getLongOption();
    }

    return optionBase;
}

BaseOption*
Parser::findOption(char shortOption) {

    // iterate over the array and search for the short option
    for(int index=0; index < options.size(); ++index) {

	BaseOption* option = options.at(index);

	if ( option->matches( shortOption ) )
	    return option;

    }

    return NULL;

}

