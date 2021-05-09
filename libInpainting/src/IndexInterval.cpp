#include "stdafx.h"

#include "IndexInterval.h"

namespace inpainting 
{  

    IndexInterval::IndexInterval()
		: containsExactResult(true)
    {

    }

    IndexInterval::IndexInterval( int first, int last)
		: first(first), last(last), containsExactResult(true)
    {

    }

    unsigned int IndexInterval::length() const
    {
        if ( !containsExactResult || first > last  )
            return 0;
        return last - first + 1;
    }

    bool IndexInterval::operator!=(const IndexInterval& other) const
    {
        return first != other.first || last != other.last;
    }

    bool IndexInterval::operator==(const IndexInterval& other) const
    {
        return first == other.first && last == other.last;
    }

    std::ostream& operator<<(std::ostream& ofs, const IndexInterval& interval )
    {
        ofs << interval.first << "-" << interval.last << " (length: " << interval.length() << ")";
        return ofs;
    }

} // namespace inpainting
