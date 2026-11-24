#include <libyomu/Dictionary.hpp>
#include <libyomu/Glossary.hpp>
#include <libyomu/Term.hpp>

#include <iostream>
#include <memory>
#include <string>

#define TAB "\t"
#define NL  "\n"

int main()
{
    auto dict = std::unique_ptr<yomu::Dictionary>{yomu::Dictionary::fromFile("./dict.zip")};

    std::cout << "Title: " << dict->title() << NL;
    std::cout << "Revision: " << dict->revision() << NL;
    std::cout << "Format: " << dict->format() << NL;
    std::cout << NL;

    //return 0;

    std::cout << "Terms:" << NL;
    for (auto& term : dict->terms()) {
        std::cout << TAB "Expression: "
            << term.expression() << "(" << term.reading() << ")"
            << NL;
        std::cout << TAB "Definition tag count: " << term.definitionTags().size() << NL;
        std::cout << TAB "Rule count: " << term.rules().size() << NL;

        for (auto& rule : term.rules()) {
            std::cout << TAB TAB "Rule: " << rule << NL;
        }

        std::cout << TAB "Score: " << term.score() << NL;
        std::cout << TAB "Glossary count: " << term.glossary().size() << NL;

        for (auto& gloss : term.glossary()) {
            if (auto txt = gloss.text()) {
                std::cout << std::string(50, '-') << NL;
                std::cout << txt->text() << NL;
            }
            std::cout << std::string(50, '-') << NL;
        }

        std::cout << TAB "Sequence: " << term.sequenceNum() << NL;
        std::cout << TAB "Term tag count: " << term.termTags().size() << NL;

        for (auto& tag : term.termTags()) {
            std::cout << TAB TAB << "Term tag: " << tag << NL;
        }

        std::cout << NL;
    }

    /*
    for (yomu::TermMeta& termMeta : dict.termMeta()) {
    }
    */

    /*
    std::cout << "Kanji:" << NL;
    for (yomu::Kanji& kanji : dict.kanji()) {
        std::cout << NL;
    }
    */

    return 0;
}
