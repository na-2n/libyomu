#include <libyomu/Dictionary.hpp>
#include <libyomu/Error.hpp>
#include <libyomu/Frequency.hpp>
#include <libyomu/Glossary.hpp>
#include <libyomu/Kanji.hpp>
#include <libyomu/StructuredContent.hpp>
#include <libyomu/Tag.hpp>
#include <libyomu/Term.hpp>
#include <libyomu/TermMeta.hpp>

#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#define TAB "\t"
#define NL  "\n"

template <typename T>
std::string orNone(const std::optional<T>& value)
{
    if (!value) {
        return "(none)";
    }

    if constexpr (std::is_same_v<T, std::string>) {
        return *value;
    } else {
        return std::format("{}", *value);
    }
}

std::string join(const std::vector<std::string>& parts)
{
    auto str = std::string{};

    for (auto& part : parts) {
        str += str.empty() ? part : ", " + part;
    }

    return str.empty() ? "(none)" : str;
}

std::string join(const std::vector<int>& parts)
{
    auto strs = std::vector<std::string>{};

    for (auto part : parts) {
        strs.push_back(std::to_string(part));
    }

    return join(strs);
}

std::string frequency(const yomu::Frequency& freq)
{
    if (auto& display = freq.displayValue()) {
        return std::format("{} ({})", freq.value(), *display);
    }

    return std::format("{}", freq.value());
}

std::string_view tagName(yomu::ContentTag tag)
{
    using enum yomu::ContentTag;

    switch (tag) {
        case Br: return "br";
        case Ruby: return "ruby";
        case Rt: return "rt";
        case Rp: return "rp";
        case Table: return "table";
        case Thead: return "thead";
        case Tbody: return "tbody";
        case Tfoot: return "tfoot";
        case Tr: return "tr";
        case Td: return "td";
        case Th: return "th";
        case Span: return "span";
        case Div: return "div";
        case Ol: return "ol";
        case Ul: return "ul";
        case Li: return "li";
        case Details: return "details";
        case Summary: return "summary";
        case Img: return "img";
        case A: return "a";
    }

    return "?";
}

void printContent(const std::vector<yomu::ContentNode>& nodes, int depth = 0)
{
    auto indent = std::string(static_cast<size_t>(depth) * 2, ' ');

    for (auto& node : nodes) {
        if (auto text = node.text()) {
            std::cout << indent << '"' << *text << '"' << NL;
            continue;
        }

        auto& el = *node.element();
        std::cout << indent << "<" << tagName(el.tag());

        if (auto& lang = el.lang()) {
            std::cout << " lang=" << *lang;
        }
        if (auto& title = el.title()) {
            std::cout << " title=\"" << *title << "\"";
        }
        if (el.colSpan() != 1 || el.rowSpan() != 1) {
            std::cout << " span=" << el.colSpan() << "x" << el.rowSpan();
        }
        if (el.open()) {
            std::cout << " open";
        }
        if (auto& href = el.href()) {
            std::cout << " href=" << *href << (el.internalLink() ? " (internal)" : "");
        }
        if (auto& img = el.image()) {
            std::cout << " path=" << img->path() << " size=" << orNone(img->width()) << "x"
                      << orNone(img->height());
        }
        for (auto& [key, value] : el.data()) {
            std::cout << " data-" << key << "=" << value;
        }
        if (auto& style = el.style()) {
            std::cout << " style{";
            if (style->fontWeight == yomu::ContentStyle::FontWeight::Bold) {
                std::cout << " bold";
            }
            if (auto& size = style->fontSize) {
                std::cout << " size=" << *size;
            }
            if (auto& color = style->color) {
                std::cout << " color=" << *color;
            }
            if (auto& margin = style->marginTop) {
                std::cout << " marginTop=" << *margin;
            }
            if (auto& lines = style->textDecorationLine) {
                std::cout << " decorations=" << lines->size();
            }
            std::cout << " }";
        }

        std::cout << ">" << NL;
        printContent(el.content(), depth + 1);
    }
}

void printMetadata(const yomu::Dictionary& dict)
{
    std::cout << "Title: " << dict.title() << NL;
    std::cout << "Revision: " << dict.revision() << NL;
    std::cout << "Format: " << dict.format() << NL;
    std::cout << "Sequenced: " << std::boolalpha << dict.sequenced() << NL;
    std::cout << "Author: " << orNone(dict.author()) << NL;
    std::cout << "URL: " << orNone(dict.url()) << NL;
    std::cout << "Description: " << orNone(dict.description()) << NL;
    std::cout << "Attribution: " << orNone(dict.attribution()) << NL;
    std::cout << "Source language: " << orNone(dict.sourceLanguage()) << NL;
    std::cout << "Target language: " << orNone(dict.targetLanguage()) << NL;
    std::cout << "Minimum Yomitan version: " << orNone(dict.minimumYomitanVersion()) << NL;

    std::cout << "Frequency mode: ";
    if (auto mode = dict.frequencyMode()) {
        std::cout << (*mode == yomu::Dictionary::FrequencyMode::RankBased ? "rank-based"
                                                                          : "occurrence-based");
    } else {
        std::cout << "(none)";
    }
    std::cout << NL;

    if (auto& updateInfo = dict.updateInfo()) {
        std::cout << "Update index URL: " << updateInfo->indexUrl << NL;
        std::cout << "Update download URL: " << updateInfo->downloadUrl << NL;
    }

    std::cout << "Counts: " << dict.terms().size() << " terms, " << dict.termMeta().size()
              << " term meta, " << dict.kanji().size() << " kanji, " << dict.kanjiMeta().size()
              << " kanji meta, " << dict.tags().size() << " tags" << NL;
}

void printTerm(const yomu::Term& term)
{
    std::cout << TAB "Expression: "
        << term.expression() << "(" << term.reading() << ")"
        << NL;
    std::cout << TAB "Definition tags: " << join(term.definitionTags()) << NL;
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
        } else if (auto img = gloss.image()) {
            std::cout << std::string(chars, '-') << NL;
            std::cout << "Image: " << img->path() << NL;
            std::cout << "Size: " << orNone(img->width()) << "x" << orNone(img->height()) << NL;
            std::cout << "Title: " << orNone(img->title()) << NL;
            std::cout << "Alt: " << orNone(img->alt()) << NL;
            std::cout << "Description: " << orNone(img->description()) << NL;
        } else if (auto structured = gloss.structured()) {
            std::cout << std::string(chars, '-') << NL;
            printContent(structured->content());
        } else if (auto deinflection = gloss.deinflection()) {
            std::cout << std::string(chars, '-') << NL;
            std::cout << "Inflection of " << deinflection->uninflected() << ": "
                      << join(deinflection->rules()) << NL;
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

void printTermMeta(const yomu::TermMeta& meta)
{
    std::cout << TAB "Expression: " << meta.expression() << NL;

    if (auto freq = meta.frequency()) {
        std::cout << TAB "Frequency: " << frequency(freq->frequency()) << NL;
        std::cout << TAB "Reading: " << orNone(freq->reading()) << NL;
    } else if (auto pitch = meta.pitch()) {
        std::cout << TAB "Pitch reading: " << pitch->reading() << NL;

        for (auto& p : pitch->pitches()) {
            auto position = std::visit(
                [](auto& pos) { return std::format("{}", pos); }, p.position());

            std::cout << TAB TAB "Position: " << position << NL;
            std::cout << TAB TAB "Nasal: " << join(p.nasalPositions()) << NL;
            std::cout << TAB TAB "Devoice: " << join(p.devoicePositions()) << NL;
            std::cout << TAB TAB "Tags: " << join(p.tags()) << NL;
        }
    } else if (auto ipa = meta.ipa()) {
        std::cout << TAB "IPA reading: " << ipa->reading() << NL;

        for (auto& t : ipa->transcriptions()) {
            std::cout << TAB TAB "IPA: " << t.ipa << " [" << join(t.tags) << "]" << NL;
        }
    }

    std::cout << NL;
}

void printKanji(const yomu::Kanji& kanji)
{
    std::cout << TAB "Character: " << kanji.character() << NL;
    std::cout << TAB "Onyomi: " << join(kanji.onyomi()) << NL;
    std::cout << TAB "Kunyomi: " << join(kanji.kunyomi()) << NL;
    std::cout << TAB "Tags: " << join(kanji.tags()) << NL;
    std::cout << TAB "Meanings: " << join(kanji.meanings()) << NL;

    for (auto& [name, value] : kanji.stats()) {
        std::cout << TAB TAB "Stat " << name << ": " << value << NL;
    }

    std::cout << NL;
}

void printKanjiMeta(const yomu::KanjiMeta& meta)
{
    std::cout << TAB "Character: " << meta.character() << NL;
    std::cout << TAB "Frequency: " << frequency(meta.frequency()) << NL;
    std::cout << NL;
}

void printTag(const yomu::Tag& tag)
{
    std::cout << TAB << tag.name() << " (" << tag.category() << "): " << tag.notes()
              << " [order " << tag.order() << ", score " << tag.score() << "]" << NL;
}

int main(int argc, char* argv[])
{
    auto arg = std::string{argc > 1 ? argv[1] : "テスト"};
    auto path = std::string{argc > 2 ? argv[2] : "./dict.zip"};

    auto dict = std::unique_ptr<yomu::Dictionary>{};
    try {
        dict = yomu::Dictionary::fromFile(path);
    } catch (const yomu::DictionaryError& e) {
        std::cerr << "Failed to load dictionary: " << e.what() << NL;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error while loading dictionary: " << e.what() << NL;
        return 1;
    }

    printMetadata(*dict);
    std::cout << NL;

    std::cout << "Terms:" << NL;
    for (auto& term :
         dict->terms() | std::views::filter([&](auto& t) { return t.expression() == arg || t.reading() == arg; })) {
        printTerm(term);
    }

    std::cout << "Term meta:" << NL;
    for (auto& meta :
         dict->termMeta() | std::views::filter([&](auto& m) { return m.expression() == arg; })) {
        printTermMeta(meta);
    }

    std::cout << "Kanji:" << NL;
    for (auto& kanji :
         dict->kanji() | std::views::filter([&](auto& k) { return k.character() == arg; })) {
        printKanji(kanji);
    }

    std::cout << "Kanji meta:" << NL;
    for (auto& meta :
         dict->kanjiMeta() | std::views::filter([&](auto& m) { return m.character() == arg; })) {
        printKanjiMeta(meta);
    }

    std::cout << "Tags:" << NL;
    for (auto& tag : dict->tags()) {
        printTag(tag);
    }

    return 0;
}
