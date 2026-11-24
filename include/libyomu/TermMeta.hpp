#pragma once

#include <vector>
#include <string>

namespace yomu {
    enum class TermMetaType {
        Frequency,
        Pitch,
        IPA,
        Unknown,
    };

    class TermMeta {
    public:
        const std::string& expression() const;

        TermMetaType type() const;
    };

    class FreqTermMeta : public TermMeta {
    public:
        int frequency() const;

        const std::string& freqString() const;
    };

    class PitchTermMeta {
    public:
        class Pitch {
        private:
            //Pitch(int downstep, std::vector<int>) 

            int _downstep;
            std::vector<int> _nasal;
            std::vector<int> _devoice;
            std::vector<std::string> _tags;

            friend class PitchTermMeta;
        public:
            inline int downstep() const { return _downstep; }

            inline const std::vector<int>& nasalPositions() const { return _nasal; }

            inline const std::vector<int>& devoicePositions() const { return _devoice; }

            inline const std::vector<std::string>& tags() const { return _tags; }
        };

        const std::string& reading() const;

        const std::vector<Pitch>& pitches() const;
    };
}

