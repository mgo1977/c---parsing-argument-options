/*
 *   C++ Command Line Options Parser (yet another one! :~)
 *
 *   Copyright (C) 2009 Mariano Ortega  <mgo1977@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   You can freely clone it from:
 *	git://github.com/mgo1977/c---parsing-argument-options.git
 *
 *   This is a very beta version, it has been developed in ~5 hours, so
 *   probably you'll find some problems/bugs.
 * 
 *   Feel free to send me your feedback and comments (patches also are welcome)
 *
 */

#ifndef __PARSER__
#define __PARSER__

#include <iostream>
#include <iomanip>
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

        bool isSet() {
        return found;
    }

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
        return ( toLower(lOption) == toLower(longOption) );
    }

    std::string toLower(const std::string& original) {
        std::string lc = original;

        for(int index=0; index<lc.size(); ++index)
            lc[index] = std::tolower(lc[index]);

        return lc;
    }

    int bestMatch(std::string lOption) {

        // The idea is to determine the number of chars
        // that matches the requested option
        int matches = 0;

        // is greater don't waste time
        if ( lOption.size() > longOption.size() )
            return matches;

        for(int index=0; index < lOption.size(); ++index) {
            if ( std::tolower(lOption[index]) == std::tolower(longOption[index]) )
                matches++;
            else
                return matches;
        }

        return matches;

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

template<typename T>
class NumberOption : public Option<T> {

    public:

    NumberOption(char sOption, const char* lOption, bool mandatory, const char* descr)
     : Option<T>(sOption, lOption, mandatory, true, descr)
    {}

    NumberOption(char sOption, const char* lOption, bool mandatory, T defValue, const char* descr)
     : Option<T>(sOption, lOption, mandatory, true, defValue, descr)
    {}

    T getValue() {


        if ( ! Option<T>::found )
            return Option<T>::defaultValue;
        else {

            if ( canBeConvertedTo<T>( Option<T>::value ) ) {
                return fromString<T>( Option<T>::value );
            }
            else {
                // this is a tricky condition... we should be reporting an error
                // possible throwing an exception
                return -1;
            }

        }

    }

};

typedef NumberOption<int>       IntegerOption;
typedef NumberOption<float>     FloatOption;
typedef NumberOption<double>    DoubleOption;


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

        std::string possibleValue;

        // now, if the next char is a '-' it's a long option,
        // if not, it's a short one
        if ( argument[1] != '-' ) {

            option = findOption(argument[1]);

            // this looks like a short option, so let's check if there
            // are no more chars here, then we pick the value from here
            if ( argument.length() > 2 ) {
                possibleValue = argument.substr(2);
            }


        }
        else {
            
            // this looks like a long option, so let's check if there
            // are no more chars here, if not, that's malformed
            if ( argument.length() < 3 ) {
                std::stringstream error;
                error << "Malformed argument! (see arg number " << argNumber << ")";
                usage(error.str());
            }

            // let's allow the separation between key and value by '='
            // on long options
            std::string optionAndValueStr = argument.substr(2);

            // initially we suppose there is no value
            std::string optionStr = optionAndValueStr;

            int separator = optionStr.find('=');

            if ( separator != std::string::npos ) {
                optionStr     = optionAndValueStr.substr(0, separator);
                possibleValue = optionAndValueStr.substr(separator+1);
            }

            option = findOption( optionStr );

        }

        if ( option == NULL ) {
            std::stringstream error;
            error << "Unknown option '" << argument << "' (see arg number " << argNumber << ")";
            usage(error.str());
        }

        // let's see if this needs an argument
        if ( option->needArgument() ) {

            if ( possibleValue.empty() ) {

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
                // the value we got directly from the option:
                //   --key=value or -kvalue
                option->setValue( possibleValue.c_str() );
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

    int maxWidth = 30;


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
        fullDescription << " " << std::setw(maxWidth) << std::left << fullOptionBase << "\t\t" << option->getDescription() << std::endl;

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

    // iterate over the array and search for the exact match option
    for(int index=0; index < options.size(); ++index) {
        BaseOption* option = options.at(index);
        if ( option->matches( longOption ) )
            return option;
    }

    // now, let's search for better matching ones...
    int bestMatchSize   = 0;
    BaseOption* bestMatchOption = NULL;
    std::vector<std::string> ambiguousOptions;

    for(int index=0; index < options.size(); ++index) {
        BaseOption* option = options.at(index);

        int matchSize = option->bestMatch(longOption);

        // we have a new winner
        if ( matchSize > bestMatchSize ) {
            bestMatchSize   = matchSize;
            bestMatchOption = option;

            // clear the list
            ambiguousOptions.clear();
        }
        else if (bestMatchSize>0 && matchSize==bestMatchSize ) {
            // this is conflicting with other option
            ambiguousOptions.push_back(option->getLongOption());
        }

    }

    if ( ambiguousOptions.size() > 0 ) {
        // if we're in an ambiguous case, it's better to report it
        std::stringstream error;
        error << "Option '" << longOption << "' is ambiguous: ";

        error << bestMatchOption->getLongOption();

        for(int index=0; index < ambiguousOptions.size(); ++index) {
            error << ", " << ambiguousOptions[index];
        }

        usage(error.str());
    }



    return bestMatchOption;

}


#endif
