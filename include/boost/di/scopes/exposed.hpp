// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_SCOPES_EXPOSED_HPP
#define BOOST_DI_SCOPES_EXPOSED_HPP

#include "boost/di/type_traits/scope_traits.hpp"

namespace boost {
namespace di {
namespace scopes {

class exposed {
public:
    static constexpr auto priority = 0; // 0 - lowest, N - highest

    template<class T>
    struct provider {
        provider() {}

        template<typename TProvider>
        explicit provider(const TProvider& provider)
            : f([&]{ return provider.get(); })
        { }

        T* get() const { return f(); }

        std::function<T*()> f;
    };

    template<class T>
    class scope {
    public:
        void create3(int);

        scope() {}

        template<class TProvider>
        explicit scope(const TProvider& provider)
            : provider_{provider}
        { }

        template<class TExpected, class TProvider>
        auto create(const TProvider&) {
            using scope = typename type_traits::scope_traits_t<TExpected>::template scope<T>;
            return scope{}.template create<T>(provider_);
        }

    private:
        provider<T> provider_;
    };
};

} // namespace scopes
} // namespace di
} // namespace boost

#endif
