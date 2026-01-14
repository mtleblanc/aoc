#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <year> <max_day>"
    exit 1
fi

YEAR=$1
MAX_DAY=$2
DIR="src/${YEAR}"

mkdir -p "$DIR"
mkdir -p "inputs/${YEAR} tests/${YEAR}"

for DAY in $(seq 1 $MAX_DAY); do
    FILE="${DIR}/day$(printf '%02d' $DAY).cc"

    if [ -f "$FILE" ]; then
        echo "Skipping $FILE (already exists)"
        continue
    fi

    cat > "$FILE" << EOF
#include "aoc.hh"

/* https://adventofcode.com/${YEAR}/day/${DAY}
 */
namespace aoc
{
constexpr size_t YEAR = ${YEAR};
constexpr size_t DAY = ${DAY};

template <> Solution solve<YEAR, DAY>(std::istream& input)
{
    (void)input;
    return {};
}
} // namespace aoc
EOF

    echo "Created $FILE"
done
