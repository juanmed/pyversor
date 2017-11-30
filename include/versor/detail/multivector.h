// Versor Geometric Algebra Library
// Copyright (c) 2017 Lars Tingelstad
// Copyright (c) 2010 Pablo Colapinto
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// The views and conclusions contained in the software and documentation are
// those of the authors and should not be interpreted as representing official
// policies, either expressed or implied, of the FreeBSD Project.

#pragma once

#include <versor/detail/algebra.h>

#include <math.h>
#include <iostream>

namespace vsr {

// Generic Geometric Number Types (templated on an algebra and a basis )
// Multivector is the main value-storing class. All geometric algebra types are
// template instantiations of this class. The types of **values** contained here
// (float, double, etc) are deterimined by the algebra. The way in which the
// data is combined with other methods is determined by their respective basis,
// in coordination with the algebra.

template <typename algebra_type, typename basis_type>
struct Multivector {
  // Algebra (a metric and a field)
  using algebra = algebra_type;

  // basis is an algebraic data type created with compile-time list processing
  using basis = basis_type;

  // number of bases
  static const int Num = basis::Num;

  // the field over which the algebra is defined (e.g. float or double)
  using value_t = typename algebra::value_t;

  // MultivectorType>::space::<MultivectorName> to declare another named Type
  // within the same algebra
  using space = typename algebra::types;

  // another type within same algebra
  template <class B>
  using MultivectorB = Multivector<algebra, B>;

  // the dual type (product of this and algebra::types::pseudoscalar)
  using Dual = typename algebra::template make_gp<
      typename blade<algebra::dim, algebra::dim>::type, basis>;

  // the Euclidean subspace dual type (product of this and
  // algebra::types::euclidean_pseudoscalar)
  using DualE =
      typename algebra::template make_gp<Basis<bits::pss(algebra::dim - 2)>,
                                         basis>;

 static constexpr auto basis_blades() {
  auto arr = basis::array;
  auto earr = std::array<std::string, arr.size()>{};
  auto size = arr.size();
  for (int i = 0; i < arr.size(); ++i) {
    earr[i] = vsr::bits::estring(arr[i]);
  }
  return earr;
}

  std::array<value_t, Num> val;
  // // data array
  // value_t val[Num];

  // // data array type
  // typedef const value_t array_type[Num];

  // pointer to first data
  // array_type &begin() const { return val; }

  // get value at idx
  constexpr value_t operator[](int idx) const { return val[idx]; }
  // set value at idx
  value_t &operator[](int idx) { return val[idx]; }

  // Construct from list of args (cannot be longer than Num)
  template <typename... Args>
  constexpr explicit Multivector(Args... v) : val{static_cast<value_t>(v)...} {}

  // construct from different basis within same algebra
  template <typename B>
  constexpr Multivector(const Multivector<algebra, B> &b)
      : Multivector(b.template cast<Multivector<algebra, basis>>()) {}

  // construct from different algebra signature and different basis
  template <class alg, typename B>
  constexpr Multivector(const Multivector<alg, B> &b)
      : Multivector(b.template cast<Multivector<algebra, basis>>()) {}

  // immutable get value of blade type IDX
  template <bits::type IDX>
  value_t get() const;
  // mutable get value of blade type IDX
  template <bits::type IDX>
  value_t &get();
  // set value of blade type IDX
  template <bits::type IDX>
  Multivector &set(value_t v);

  /// Reset
  Multivector &reset(value_t v = value_t()) {
    std::fill(&(val[0]), &(val[0]) + basis::Num, v);
    return *this;
  }

  /// Conjugation Unary Operation
  Multivector conjugation() const;
  /// Conjugation Unary Operation Shorthand
  Multivector conj() const { return conjugation(); }
  /// Involution Unary Operation
  Multivector involution() const;
  /// Involution Unary Operation Shorthand
  Multivector inv() const { return involution(); }

  /// Casting to type B
  template <class B>
  B cast() const;
  /// Copying to type B
  template <class B>
  B copy() const;

  // comparison
  bool operator==(const Multivector &mv) const {
    for (int i = 0; i < Num; ++i) {
      if (val[i] != mv[i]) return false;
    }
    return true;
  }

  // printing
  void print() {
    basis::beprint();
    VPrint::Call(*this);
  }

  // geometric product
  template <class B>
  auto operator*(const MultivectorB<B> &b) const {
    return algebra::gp(*this, b);
  }

  // geometric product in place Transformation
  template <class B>
  Multivector &operator*=(const MultivectorB<B> &b) {
    *this = *this * b;
    return *this;
  }

  // outer product
  template <class B>
  auto operator^(const MultivectorB<B> &b) const {
    return algebra::op(*this, b);
  }

  // left contraction inner product
  template <class B>
  auto operator<=(const MultivectorB<B> &b) const {
    return algebra::ip(*this, b);
  }

  // commutator product
  template <class B>
  auto operator%(const MultivectorB<B> &b) const {
    return ((*this * b) - (b * (*this))) * .5;
  }

  // rotor (even) transformation
  template <typename B>
  Multivector spin(const MultivectorB<B> &b) const {
    return algebra::spin(*this, b);
  }
  // versor (odd) transformation
  template <typename B>
  Multivector reflect(const MultivectorB<B> &b) const {
    return algebra::reflect(*this, b);
  }

  template <typename B>
  Multivector sp(const MultivectorB<B> &b) const {
    return spin(b);
  }
  template <typename B>
  Multivector re(const MultivectorB<B> &b) const {
    return reflect(b);
  }

  // reversion
  Multivector operator~() const {
    return Reverse<basis>::Type::template Make(*this);
  }

  // inversion
  Multivector operator!() const {
    Multivector tmp = ~(*this);
    value_t v = ((*this) * tmp)[0];
    return (v == 0) ? tmp : tmp / v;
  }

  // division
  template <class B>
  auto operator/(const MultivectorB<B> &b) const {
    return (*this * !b);
  }

  // transformations (note mostly here for conformal metric) for instance
  // null() only works in conformal metric (returns identity in others) these
  // are all defined in generic_op.h or in cga3D_op.h

  // conformal Mapping of Euclidean vectors to conformal points
  Multivector<algebra, typename algebra::vector_basis> null() const;

  // rotate in b plane
  template <class B>
  Multivector rot(const MultivectorB<B> &b) const;
  template <class B>
  Multivector rotate(const MultivectorB<B> &b) const;

  template <class B>
  Multivector trs(const MultivectorB<B> &b) const;
  template <class B>
  Multivector translate(const MultivectorB<B> &b) const;

  template <class... Ts>
  Multivector trs(Ts... v) const;
  template <class... Ts>
  Multivector translate(Ts... v) const;

  template <class B>
  Multivector trv(const MultivectorB<B> &b) const;
  template <class B>
  Multivector transverse(const MultivectorB<B> &b) const;

  template <class... Ts>
  Multivector trv(Ts... v) const;
  template <class... Ts>
  Multivector transverse(Ts... v) const;

  template <class B>
  Multivector mot(const MultivectorB<B> &b) const;
  template <class B>
  Multivector motor(const MultivectorB<B> &b) const;
  template <class B>
  Multivector twist(const MultivectorB<B> &b) const;

  template <class B>
  Multivector bst(const MultivectorB<B> &b) const;
  template <class B>
  Multivector boost(const MultivectorB<B> &b) const;

  template <class B>
  Multivector dil(const MultivectorB<B> &b, VSR_PRECISION t) const;
  template <class B>
  Multivector dilate(const MultivectorB<B> &b, VSR_PRECISION t) const;

  // duality
  auto dual() const { return algebra::gp(*this, typename space::Pss(-1)); }
  auto undual() const { return algebra::gp(*this, typename space::Pss(1)); }
  auto duale() const { return algebra::gp(*this, typename space::Euc(-1)); }
  auto unduale() const { return algebra::gp(*this, typename space::Euc(1)); }

  // norms, weights, and units
  value_t wt() const { return (*this <= *this)[0]; }
  value_t rwt() const { return (*this <= ~(*this))[0]; }
  value_t norm() const {
    value_t a = rwt();
    if (a < 0) return 0;
    return sqrt(a);
  }

  value_t rnorm() const {
    value_t a = rwt();
    if (a < 0) return -sqrt(-a);
    return sqrt(a);
  }

  Multivector unit() const {
    value_t t = sqrt(fabs((*this <= *this)[0]));
    if (t == 0) return Multivector();
    return *this / t;
  }

  Multivector runit() const {
    value_t t = rnorm();
    if (t == 0) return Multivector();
    return *this / t;
  }

  Multivector tunit() const {
    value_t t = norm();
    if (t == 0) {
      return Multivector();
    }
    return *this / t;
  }

  // overloaded operators

  template <class B>
  auto operator+(const MultivectorB<B> &b)
      -> MultivectorB<typename Merge<basis, B>::Type> {
    return algebra::sum(*this, b);
  }

  Multivector operator+(const Multivector &a) const {
    Multivector tmp;
    for (int i = 0; i < Num; ++i) tmp[i] = (*this)[i] + a[i];
    return tmp;
  }

  Multivector operator-(const Multivector &a) const {
    Multivector tmp;
    for (int i = 0; i < Num; ++i) tmp[i] = (*this)[i] - a[i];
    return tmp;
  }

  Multivector operator-() const {
    Multivector tmp = *this;
    for (int i = 0; i < Num; ++i) {
      tmp[i] = -tmp[i];
    }
    return tmp;
  }

  Multivector &operator-=(const Multivector &b) {
    for (int i = 0; i < Num; ++i) (*this)[i] -= b[i];
    return *this;
  }
  Multivector &operator+=(const Multivector &b) {
    for (int i = 0; i < Num; ++i) (*this)[i] += b[i];
    return *this;
  }

  Multivector operator/(value_t f) const {
    Multivector tmp = *this;
    for (int i = 0; i < Num; ++i) {
      tmp[i] /= f;
    }
    return tmp;
  }

  Multivector &operator/=(value_t f) {
    for (int i = 0; i < Num; ++i) {
      (*this)[i] /= f;
    }
    return *this;
  }

  Multivector &operator/=(const Multivector &b) { return (*this) *= !b; }

  Multivector operator*(value_t f) const {
    Multivector tmp = *this;
    for (int i = 0; i < Num; ++i) {
      tmp[i] *= f;
    }
    return tmp;
  }

  Multivector &operator*=(value_t f) {
    for (int i = 0; i < Num; ++i) {
      (*this)[i] *= f;
    }
    return *this;
  }

  MultivectorB<typename ICat<typename NotType<Basis<0>, basis_type>::Type,
                             Basis<0>>::Type>
  operator+(value_t a) const {
    typedef MultivectorB<typename ICat<
        typename NotType<Basis<0>, basis_type>::Type, Basis<0>>::Type>
        Ret;
    return algebra::sum(Ret(a), this->template cast<Ret>());
  }

  static Multivector x, y, z, xy, xz, yz;

  friend ostream &operator<<(ostream &os, const Multivector &m) {
    auto blades = basis_blades();
    for (int i = 0; i < Num; ++i) {
      if (abs(m[i]) > 1e-6)
      {
        os << m[i] << blades[i] << " ";  // std::endl;
      }
    }
    return os;
  }
};

template <typename Algebra, typename Basis>
auto inline operator+(double lhs, const Multivector<Algebra, Basis>& rhs) {
  return rhs + lhs;
}

template <typename Algebra, typename Basis>
auto inline operator-(double lhs, const Multivector<Algebra, Basis>& rhs) {
  return -rhs + lhs;
}

template <typename Algebra, typename Basis>
auto inline operator*(double lhs, const Multivector<Algebra, Basis>& rhs) {
  return rhs * lhs;
}


// conversions (casting, copying)

template <typename Algebra, typename B>
template <class A>
A Multivector<Algebra, B>::cast() const {
  return Cast<typename A::basis, B>::Type::template doCast<A>(*this);
}

template <typename Algebra, typename B>
template <class A>
A Multivector<Algebra, B>::copy() const {
  A tmp;
  for (int i = 0; i < A::basis::Num; ++i) tmp[i] = (*this)[i];
  return tmp;
}

// getters and setters

template <typename Algebra, typename B>
template <bits::type IDX>
typename Algebra::value_t &Multivector<Algebra, B>::get() {
  return val[find<B>(IDX, 0)];
}
template <typename Algebra, typename B>
template <bits::type IDX>
typename Algebra::value_t Multivector<Algebra, B>::get() const {
  return val[find<B>(IDX, 0)];
}
template <typename Algebra, typename B>
template <bits::type IDX>
Multivector<Algebra, B> &Multivector<Algebra, B>::set(
    typename Algebra::value_t v) {
  get<IDX>() = v;
  return *this;
}

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::x =
    Multivector<Algebra, B>().template set<1>(1);

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::y =
    Multivector<Algebra, B>().template set<2>(1);

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::z =
    Multivector<Algebra, B>().template set<4>(1);

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::xy =
    Multivector<Algebra, B>().template set<3>(1);

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::xz =
    Multivector<Algebra, B>().template set<5>(1);

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::yz =
    Multivector<Algebra, B>().template set<6>(1);

// unary operations

template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::conjugation() const {
  return Conjugate<B>::Type::template Make(*this);
}
template <typename Algebra, typename B>
Multivector<Algebra, B> Multivector<Algebra, B>::involution() const {
  return Involute<B>::Type::template Make(*this);
}

template <typename algebra>
using GASca = Multivector<algebra, typename algebra::types::sca>;
template <typename algebra>
using GAVec = Multivector<algebra, typename algebra::types::vec>;
template <typename algebra>
using GABiv = Multivector<algebra, typename algebra::types::biv>;
template <typename algebra>
using GATri = Multivector<algebra, typename algebra::types::tri>;
template <typename algebra>
using GAPnt = Multivector<algebra, typename algebra::types::pnt>;
template <typename algebra>
using GADls = Multivector<algebra, typename algebra::types::dls>;
template <typename algebra>
using GAPar = Multivector<algebra, typename algebra::types::par>;
template <typename algebra>
using GACir = Multivector<algebra, typename algebra::types::cir>;
template <typename algebra>
using GASph = Multivector<algebra, typename algebra::types::sph>;
template <typename algebra>
using GAFlp = Multivector<algebra, typename algebra::types::flp>;
template <typename algebra>
using GADll = Multivector<algebra, typename algebra::types::dll>;
template <typename algebra>
using GALin = Multivector<algebra, typename algebra::types::lin>;
template <typename algebra>
using GADlp = Multivector<algebra, typename algebra::types::dlp>;
template <typename algebra>
using GAPln = Multivector<algebra, typename algebra::types::pln>;
template <typename algebra>
using GAMnk = Multivector<algebra, typename algebra::types::mnk>;
template <typename algebra>
using GAInf = Multivector<algebra, typename algebra::types::inf>;
template <typename algebra>
using GAOri = Multivector<algebra, typename algebra::types::ori>;
template <typename algebra>
using GAPss = Multivector<algebra, typename algebra::types::pss>;
template <typename algebra>
using GATnv = Multivector<algebra, typename algebra::types::tnv>;
template <typename algebra>
using GADrv = Multivector<algebra, typename algebra::types::drv>;
template <typename algebra>
using GATnb = Multivector<algebra, typename algebra::types::tnb>;
template <typename algebra>
using GADrb = Multivector<algebra, typename algebra::types::drb>;
template <typename algebra>
using GATnt = Multivector<algebra, typename algebra::types::tnt>;
template <typename algebra>
using GADrt = Multivector<algebra, typename algebra::types::drt>;
template <typename algebra>
using GARot = Multivector<algebra, typename algebra::types::rot>;
template <typename algebra>
using GATrs = Multivector<algebra, typename algebra::types::trs>;
template <typename algebra>
using GADil = Multivector<algebra, typename algebra::types::dil>;
template <typename algebra>
using GAMot = Multivector<algebra, typename algebra::types::mot>;
template <typename algebra>
using GABst = Multivector<algebra, typename algebra::types::bst>;
template <typename algebra>
using GATrv = Multivector<algebra, typename algebra::types::trv>;
template <typename algebra>
using GACon = Multivector<algebra, typename algebra::types::con>;
template <typename algebra>
using GATsd = Multivector<algebra, typename algebra::types::tsd>;
template <typename algebra>
using GAEucPss = Multivector<algebra, typename algebra::types::eucpss>;

template <typename algebra>
struct GAE {
  template <bits::type... NN>
  using e = typename algebra::types::template e<NN...>;
};

template <bits::type N, typename T = VSR_PRECISION>
using euclidean = algebra<metric<N>, T>;
template <bits::type N, typename T = VSR_PRECISION>
using conformal = algebra<metric<N - 1, 1, true>, T>;

// Euclidean template alias utility

template <bits::type N, typename T = VSR_PRECISION>
using NESca = GASca<euclidean<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NEVec = GAVec<euclidean<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NEBiv = GABiv<euclidean<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NETri = GATri<euclidean<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NERot = GARot<euclidean<N, T>>;

template <bits::type dim, typename T = VSR_PRECISION>
using euclidean_vector = GAVec<euclidean<dim, T>>;
template <bits::type dim, typename T = VSR_PRECISION>
using euclidean_bivector = GABiv<euclidean<dim, T>>;
template <bits::type dim, typename T = VSR_PRECISION>
using euclidean_trivector = GATri<euclidean<dim, T>>;
template <bits::type dim, typename T = VSR_PRECISION>
using euclidean_rotor = GARot<euclidean<dim, T>>;

template <typename T = VSR_PRECISION>
struct NE {
  template <bits::type... NN>
  using e = typename GAE<euclidean<bits::dimOf(bits::BladeMaker<NN...>::type),
                                   T>>::template e<NN...>;
};

// conformal template alias utility

template <bits::type N, typename T = VSR_PRECISION>
using NSca = GASca<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NVec = GAVec<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NBiv = GABiv<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NPnt = GAPnt<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NBst = GABst<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NPar = GAPar<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NCir = GACir<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NRot = GARot<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTnv = GATnv<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTrv = GATrv<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTrs = GATrs<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDrv = GADrv<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDil = GADil<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTsd = GATsd<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NOri = GAOri<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NInf = GAInf<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDls = GADls<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDll = GADll<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NLin = GALin<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NMnk = GAMnk<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NPss = GAPss<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NSph = GASph<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTri = GATri<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NFlp = GAFlp<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NPln = GAPln<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDlp = GADlp<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDrb = GADrb<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTnb = GATnb<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NTnt = GATnt<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NDrt = GADrt<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NMot = GAMot<conformal<N, T>>;
template <bits::type N, typename T = VSR_PRECISION>
using NCon = GACon<conformal<N, T>>;

}  // namespace vsr