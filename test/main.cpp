#include <libyomu/Dictionary.hpp>
#include <libyomu/Glossary.hpp>
#include <libyomu/Term.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <ranges>

#define TAB "\t"
#define NL  "\n"

void printTerm(const yomu::Term& term)
{
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

    constexpr auto chars = 50;
    for (auto& gloss : term.glossary()) {
        if (auto txt = gloss.text()) {
            std::cout << std::string(chars, '-') << NL;
            std::cout << txt->text() << NL;
        }
        std::cout << std::string(chars, '-') << NL;
    }

    std::cout << TAB "Sequence: " << term.sequenceNum() << NL;
    std::cout << TAB "Term tag count: " << term.termTags().size() << NL;

    for (auto& tag : term.termTags()) {
        std::cout << TAB TAB << "Term tag: " << tag << NL;
    }

    std::cout << NL;
}

int main(int argc, char* argv[])
{
    auto arg = std::string{argc > 1 ? argv[1] : "テスト"};

    auto dict = yomu::Dictionary::fromFile("./dict.zip");

    std::cout << "Title: " << dict->title() << NL;
    std::cout << "Revision: " << dict->revision() << NL;
    std::cout << "Format: " << dict->format() << NL;
    std::cout << NL;

    std::cout << "Terms:" << NL;
    for (auto& term :
         dict->terms() | std::views::filter([&](auto& t) { return t.reading() == arg; })) {
        printTerm(term);
    }

    /*
    auto& terms = dict->terms();
    if (auto term = std::ranges::find(terms, arg, &yomu::Term::reading); term != terms.end()) {
        std::cout << "Term:" << NL;
        printTerm(*term);
    }
    */

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
