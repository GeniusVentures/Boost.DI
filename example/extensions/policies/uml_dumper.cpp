//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//[uml_dumper
//<-
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <typeinfo>
//->
#include <boost/di.hpp>

auto int_1 = []{ return "first int"; };
auto int_2 = []{ return "second int"; };

struct i0 { virtual ~i0() { }; };
struct c0 : i0 { };
struct c1 { c1(std::shared_ptr<i0>, int) { } };
struct c2 { BOOST_DI_INJECT(c2, (named = int_1) int, (named = int_2) int, char) { } };
struct c3 { c3(std::shared_ptr<c1>, std::shared_ptr<c2>) { } };

namespace di = boost::di;

// doesn't work inside polices yet / tested with gcc-5.1 and clang-3.7
std::vector<const std::type_info*> v = { &typeid(nullptr) };
int i = 1;

/**
 * http://plantuml.sourceforge.net/objects.html
 * ./uml_dumper | java -jar plantuml.jar -p > uml_dumper.png
 */
class uml_dumper : public di::config {
public:
    uml_dumper() {
        std::cout << "@startuml uml_dumper.png" << std::endl;
    }

    ~uml_dumper() {
        std::cout << "@enduml" << std::endl;
    }

    template<class _>
    static auto policies(const _&) noexcept {
        /*<<define `uml dumper` directly in policies configuration>>*/
        return di::make_policies(
            [&](auto type, auto dependency, BOOST_DI_UNUSED auto... ctor) {
                using T = decltype(type);
                using name = typename T::name;
                using given = typename decltype(dependency)::given;

                auto root = v[i - 1];
                if (root != &typeid(nullptr)) {
                    std::cout << "\"" << root->name() << "\" .. \"" << typeid(given).name()
                              << ((*(name*)(0))() ? std::string(" [") + (*(name*)(0))() + std::string("]") : "")
                              << "\"" << std::endl;
                }

                auto ctor_size = sizeof...(ctor);
                while (ctor_size--) {
                    v.insert((v.begin()+i), &typeid(given));
                }
                ++i;
            }
        );
    }
};

int main() {
    /*<<define injector>>*/
    auto injector = di::make_injector<uml_dumper>(
        di::bind<i0, c0>()
      , di::bind<int>().named(int_1).to(42)
      , di::bind<int>().named(int_2).to(42)
    );

    /*<<iterate through created objects with `types_dumper`>>*/
    injector.create<c3>();

    /*<<output [@images/uml_dumper.png [$images/uml_dumper.png [width 75%] [height 75%] ]]>>*/
    return 0;
}

//]
