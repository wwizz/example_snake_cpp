/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <cmath>
#include <memory>
#include <type_traits>
#include <vector>

#include "boost/optional.hpp"

namespace support {

    /* This facility provides an interface to compare parts of domain objects, according to our rules (e.g. don't compare
     * addresses of pointers, but the data that is pointed to).
     * Using libraries that provide introspection for structs/classes (e.g. via boost::hana or boost::fusion), this
     * facility can easily be extended to work for arbitrary domain objects, reducing quite some boiler plate / duplication
     * code in the domain model implementation. */

    template<typename T>
    bool equals(const T& lhs, const T& rhs, double epsilon = 0.0);

    namespace detail {
        template<typename T>
        struct IsEqualFunctor {
            static bool compare(const T& lhs, const T& rhs, double = 0.0) {
                // Domain objects don't offer == operator
                return !(lhs != rhs);
            }
        };

        template<typename T>
        struct IsEqualFunctor<boost::optional<T>> {
            static bool compare(const boost::optional<T>& lhs, const boost::optional<T>& rhs, double epsilon = 0.0) {
                if (lhs == boost::none && rhs == boost::none) {
                    return true;
                } else if ((lhs == boost::none) && (rhs != boost::none)) {
                    return false;
                } else if ((lhs != boost::none) && (rhs == boost::none)) {
                    return false;
                } else {
                    return IsEqualFunctor<T>::compare(*lhs, *rhs, epsilon);
                }
            }
        };

        // This assumes that the elements in the vector are ordered
        template<typename T>
        struct IsEqualFunctor<std::vector<T>> {
            static bool compare(const std::vector<T>& lhs, const std::vector<T>& rhs, double epsilon = 0.0) {
                if (lhs.size() != rhs.size()) {
                    return false;
                } else {
                    bool is_equal = true;
                    for (std::size_t i = 0; i < lhs.size(); i++) {
                        is_equal = (is_equal && IsEqualFunctor<T>::compare(lhs[i], rhs[i], epsilon));
                    }
                    return is_equal;
                }
            }
        };

        template<typename T>
        struct IsEqualFunctorForPointerTypes {
            static bool compare(const T& lhs, const T& rhs, double epsilon = 0.0) {
                if (lhs == nullptr && rhs == nullptr) {
                    return true;
                } else if (lhs != nullptr && rhs == nullptr) {
                    return false;
                } else if (lhs == nullptr && rhs != nullptr) {
                    return false;
                } else {
                    return IsEqualFunctor<typename std::pointer_traits<T>::element_type>::compare(*lhs, *rhs, epsilon);
                }
            }
        };

        template<typename T>
        struct IsEqualFunctor<T *> {
            static bool compare(T *lhs, T *rhs, double epsilon = 0.0) {
                return IsEqualFunctorForPointerTypes<T*>::compare(lhs, rhs, epsilon);
            }
        };

        template<typename T>
        struct IsEqualFunctor<std::unique_ptr<T>> {
            static bool compare(const std::unique_ptr<T>& lhs, const std::unique_ptr<T>& rhs, double epsilon = 0.0) {
                return IsEqualFunctorForPointerTypes<std::unique_ptr<T>>::compare(lhs, rhs, epsilon);
            }
        };

        template<typename T>
        struct IsEqualFunctor<std::shared_ptr<T>> {
            static bool compare(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs, double epsilon = 0.0) {
                return IsEqualFunctorForPointerTypes<std::shared_ptr<T>>::compare(lhs, rhs, epsilon);
            }
        };

        template<>
        struct IsEqualFunctor<double> {
            static bool compare(double lhs, double rhs, double epsilon = 0.0) {
                return std::abs(lhs - rhs) <= epsilon;
            }
        };
    }  // namespace detail

    template<typename T>
    bool equals(const T& lhs, const T& rhs, double epsilon) {
        return detail::IsEqualFunctor<T>::compare(lhs, rhs, epsilon);
    }
}  // namespace support