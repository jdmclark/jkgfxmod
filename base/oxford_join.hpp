#pragma once

#include "format.hpp"
#include "runtime_assert.hpp"
#include <vector>

namespace jkgm {
    namespace detail {
        template <class RangeT,
                  class ValueT = std::decay_t<decltype(*std::declval<RangeT>().begin())>>
        class oxford_join_format_script : public format_script {
        private:
            std::vector<formatter_type_t<ValueT>> formatters;
            std::string_view connective;

        public:
            oxford_join_format_script(RangeT const &list, std::string_view connective)
                : connective(connective)
            {
                for(auto const &em : list) {
                    formatters.emplace_back(em);
                }
            }

            size_t estimate_buffer() const override
            {
                size_t rv = 0;
                for(auto const &em : formatters) {
                    rv += em.estimate_buffer();
                }

                if(formatters.size() == 2) {
                    // Two elements adds a single space
                    rv += 1;
                }
                else if(formatters.size() > 2) {
                    // Three or more elements adds N commas and spaces, connective and space.
                    rv += (formatters.size() * 2) + 1 + connective.size();
                }

                return rv;
            }

            void fill_buffer(format_buffer *buffer) const override
            {
                auto lookahead_it = formatters.begin();
                auto curr_it = lookahead_it++;

                // Write out first word, which is always valid.
                curr_it->fill_buffer(buffer);

                // Advance if possible
                if(lookahead_it == formatters.end()) {
                    return;
                }

                curr_it = lookahead_it++;

                // Special case: If the second word is the last word, output without comma.
                if(lookahead_it == formatters.end()) {
                    buffer->append(" ");
                    buffer->append(connective);
                    buffer->append(" ");
                    curr_it->fill_buffer(buffer);
                    return;
                }

                // List has at least 3 elements
                while(lookahead_it != formatters.end()) {
                    buffer->append(", ");
                    curr_it->fill_buffer(buffer);
                    curr_it = lookahead_it++;
                }

                // Output last element
                buffer->append(", ");
                buffer->append(connective);
                buffer->append(" ");
                curr_it->fill_buffer(buffer);
            }
        };
    }

    template <class RangeT>
    detail::oxford_join_format_script<RangeT> oxford_join(RangeT const &list,
                                                          std::string_view connective = "and")
    {
        runtime_assert(!list.empty(), "cannot format empty list");
        return detail::oxford_join_format_script<RangeT>(list, connective);
    }
}
