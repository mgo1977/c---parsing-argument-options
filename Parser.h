
#ifndef __PARSER__
#define __PARSER__

#include <iostream>
#include <string>
#include <vector>
#include <sstream>



// helpers
template<typename T>
bool canBeConvertedTo(const std::string str) {

    T result;

    std::stringstream text(str);

    if ( text >> result )
	return true;
    else
	return false;

}

template<typename T>
T fromString(const std::string& str) {

    T result;

    std::stringstream text(str);

    text >> result;

    return result;
}


/*
 * The goal is able to simple parse cmd line options:
 *
 *  program [--debug|-d] [--file=prueba|--file prueba|-f prueba]
 *
 */

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
    std::string getValue() const {
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

    bool matches(std::string lOption) {
	return ( lOption == longOption );
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
    std::string getLongOption() const { return longOption;  }


    std::string getDescription() const { return description; }

    protected:
    // configuration
    char    shortOption;
    std::string  longOption;
    bool    mandatory;
    bool    followsArgument;
    std::string  description;

    // status & value
    bool    found; // was it found?
    std::string  value;

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
    // only the dev can know how to convert from a std::string
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

class StringOption : public Option<std::string> {

    public:

    StringOption(char sOption, const char* lOption, bool mandatory, const char* descr)
     : Option<std::string>(sOption, lOption, mandatory, true, descr)
    {}

    StringOption(char sOption, const char* lOption, bool mandatory, const char* defValue, const char* descr)
     : Option<std::string>(sOption, lOption, mandatory, true, defValue, descr)
    {}

    std::string getValue() {
	if ( ! found )
	    return defaultValue;
	else
	    return value;
    }

};

class IntegerOption : public Option<int> {

    public:

    IntegerOption(char sOption, const char* lOption, bool mandatory, const char* descr)
     : Option<int>(sOption, lOption, mandatory, true, descr)
    {}

    IntegerOption(char sOption, const char* lOption, bool mandatory, int defValue, const char* descr)
     : Option<int>(sOption, lOption, mandatory, true, defValue, descr)
    {}

    int getValue() {


	if ( ! found )
	    return defaultValue;
	else {

	    if ( canBeConvertedTo<int>(value) ) {
		return fromString<int>(value);
	    }
	    else {
		// this is a tricky condition... we should be reporting an error
		// possible throwing an exception
		return -1;
	    }

	}

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

    std::vector<std::string> parse(int argc, char** argv);

    void usage(const std::string& text) { usage(text.c_str()); }
    void usage(const char* text = "");

    private:
    BoolOption helpOption;

    std::vector<BaseOption*> options;
    std::string programName;

    BaseOption* findOption(char shortOpt);
    BaseOption* findOption(std::string longOpt);

    std::string getSummaryOptionText(BaseOption* option) {
	return getOptionText(option, "|");
    }

    std::string getFullOptionText(BaseOption* option) {
	return getOptionText(option, ", ");
    }

    std::string getOptionText(BaseOption* option, const char* separator);

};

std::vector<std::string>
Parser::parse(int argc, char** argv) {

    std::vector<std::string> otherArguments;

    // first argument is the program name
    if ( argc >= 1 )
	programName = argv[0];

    // now, start iterating over each argument
    for(int argNumber=1; argNumber < argc; ++argNumber) {

	std::string argument = argv[argNumber];

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
	    std::stringstream error;
	    error << "Malformed argument! (see arg number " << argNumber << ")";
	    usage(error.str());
	}

	BaseOption* option = NULL;

	// now, if the next char is a '-' it's a long option,
	// if not, it's a short one
	if ( argument[1] != '-' ) {

	    // this looks like a short option, so let's check if there
	    // are no more chars here, if not, that's malformed
	    if ( argument.length() > 2 ) {
		std::stringstream error;
		error << "Malformed argument! (see arg number " << argNumber << ")";
		usage(error.str());
	    }

	    option = findOption(argument[1]);

	}
	else {
	    
	    // this looks like a long option, so let's check if there
	    // are no more chars here, if not, that's malformed
	    if ( argument.length() < 3 ) {
		std::stringstream error;
		error << "Malformed argument! (see arg number " << argNumber << ")";
		usage(error.str());
	    }

	    option = findOption( argument.substr(2) );

	}

	if ( option == NULL ) {
	    std::stringstream error;
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
		std::stringstream error;
		error << "Option '" << argument << "' needs an additional argument";
		usage(error.str());
	    }

	}
	else {
	    // set as read
	    option->markAsFound();
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
   
    std::string mandatoriesError; 

    for(std::vector<BaseOption*>::iterator iter = options.begin();
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
	std::cerr << text << std::endl;
    }

    // add the options
    std::cerr << "Usage: ";
    std::cerr << programName << " ";

    std::stringstream optionsSummary;
    std::stringstream fullDescription;


    for(std::vector<BaseOption*>::iterator iter = options.begin();
	iter != options.end();
	++iter
    ) {

	BaseOption* option = *iter;

	// this is the syntax:
	//   [ ] => optional
	//   short|long
	std::string summaryOptionBase = getSummaryOptionText(option);
	std::string fullOptionBase    = getFullOptionText(option);

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
	fullDescription << " " << fullOptionBase << "\t\t" << option->getDescription() << std::endl;

    }

    std::cerr << optionsSummary.str() << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << fullDescription.str();

    // end
    exit(1);

} 

std::string
Parser::getOptionText(BaseOption* option, const char* separator) {

    std::string optionBase;

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

BaseOption*
Parser::findOption(std::string longOption) {

    // iterate over the array and search for the short option
    for(int index=0; index < options.size(); ++index) {

	BaseOption* option = options.at(index);

	if ( option->matches( longOption ) )
	    return option;

    }

    return NULL;

}


#endif
