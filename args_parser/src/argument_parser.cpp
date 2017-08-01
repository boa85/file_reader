//

#include "../include/argument_parser.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

namespace word_counter {
    namespace argument_parser {
        ArgumentParser::ArgumentParser()
                : generalDescription_("program options"), mode(UNKNOWN) {
            initDescriptions();
        }

        void ArgumentParser::initDescriptions() {
            generalDescription_.add_options()
                    ("help,h",  "program options: \n "
                            "file name, mode, word\n"
                            "e.g ./word_counter -f Test.tst -m words -v mother - \n prints the number of the word \"mother\" in the Test.tst file"
                            "\n or \n "
                            "./word_counter -f Test -m checksum - prints a 32-bit checksum\n ")
                    ("file,f", po::value<std::string>(&fileName_),
                     "input filename, e.g. Test.tst")
                    ("mode,m", po::value<std::string>(&mode_),
                     "program mode: words, checksum")
                    ("v,vv", po::value<std::string>(&word_),
                     " search word");
        }

        void ArgumentParser::startParsing(int argc, char *argv[]) {
            po::variables_map vm;//
            po::parsed_options parsed =//magic
                    po::command_line_parser(argc, argv).options(generalDescription_).allow_unregistered().run();

            po::store(parsed, vm);//more magic
            po::notify(vm);//even more magic

            if (vm.count("help") != 0u) {//if found a help flag
                std::cout << generalDescription_;//show help
                return;
            }
            auto error = [this](
                    const std::string &errorString) {//не самое грамотное решение, но чуть чуть сокращает код
                std::cout << errorString << ", see help " << std::endl << generalDescription_ << std::endl;
                throw std::invalid_argument(errorString);
            };

            po::store(po::parse_command_line(argc, argv, generalDescription_), vm);
            mode = (mode_ == "words" ) ? WORDS : (mode_ == "checksum") ? CHECKSUM : UNKNOWN;
            if (mode == UNKNOWN) {
                error("unknown mode ");
            }
            if (fileName_.empty()) {
                auto errorString = "invalid argument filename";
                error(errorString);
            }
            if (!boost::filesystem::exists(fileName_)) {
                error("file " + fileName_ + " not found ");
            }
            if (mode == WORDS && word_.empty()) {
                error("empty search word");
            }
            switch (mode) {
                case WORDS:
                    count(fileName_, word_);
                    break;
                case CHECKSUM:
                    checksum(fileName_);
                    break;
                case UNKNOWN:
                default:
                    error("unknown mode ");
            }
            std::cout << "\nfilename = " << fileName_ << "\nmode = " << mode << "\nword = " << word_ << std::endl;
        }//startParsing
    }//namespace argument_parser
}//namespace word_counter
