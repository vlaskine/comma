#ifndef COMMA_CSV_IMPL_EPOCH_H_
#define COMMA_CSV_IMPL_EPOCH_H_

#include <boost/date_time/posix_time/posix_time.hpp>

namespace comma { namespace csv { namespace impl {

static const boost::gregorian::date epoch( 1970, 1, 1 );

} } } // namespace comma { namespace csv { namespace impl {

#endif // #ifndef COMMA_CSV_IMPL_EPOCH_H_
