#pragma once

namespace inpainting 
{
    class IndexInterval
    {
    public:
        IndexInterval();
        IndexInterval( int first, int last);

        bool contains();
        unsigned int length() const;

        bool operator==(const IndexInterval& other) const;
        bool operator!=(const IndexInterval& other) const;

        int first;
        int last;
        bool containsExactResult;
    };

    std::ostream& operator<<(std::ostream& ofs, const IndexInterval& interval);

} // namespace inpainting
