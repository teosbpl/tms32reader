//---------------------------------------------------------------------------

/*
 * Extension of standard algorithms
 *
 * author: Michal Pietrusinski 2002, Teodor Buchner 2004
 *
 */
// FDeleteObject based on Scott Meyers Effective STL
// unary_compose and compose1 where taken from SGI implementation of STL. Below
// is the copyright notice.
/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996-1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */




#ifndef extalgH
#define extalgH
//---------------------------------------------------------------------------

#include <functional>

#include <cstddef>


//template <class T1, class T2>
template <class Pair>
struct compare_second {
  //bool operator() (const pair<T1,T2>& p1, const pair<T1,T2>& p2)
  bool operator() (const Pair& p1, const Pair& p2)
    {
      return p1.second < p2.second;
    }
};





template <class InputIterator, class Function, class Predicate>
Function for_each_if (InputIterator first, InputIterator last, Function f, Predicate pred)
{
  while (first != last) {
    if (pred(*first))
      f(*first);
    first++;
  }
  return f;
}

template<class ForwardIterator, class OutputIterator, class Predicate>
OutputIterator copy_if(ForwardIterator first, ForwardIterator last,
  OutputIterator result, Predicate f) {
  while(first != last) {
    if(f(*first))
      *result++ = *first;
    first++;
  }
  return result;
}


template <class FFunc>
class unary_ref : public std::unary_function<typename FFunc::argument_type,
                                                typename FFunc::result_type> {
protected:
  FFunc& Func;
public:
  unary_ref(FFunc& func) : Func(func) {}
  typename FFunc::result_type operator() (const typename FFunc::argument_type& item)
    { return Func(item); }
};

template <class FFunc>
inline unary_ref<FFunc>
ref1(FFunc& func)
{
  return unary_ref<FFunc>(func);
}


template <class Predicate, class Predicate2>
class unary_and : public std::unary_function<typename Predicate::argument_type, bool>
{
protected:
  Predicate Pred;
  Predicate2 Pred2;
public:
  typedef typename std::unary_function<typename Predicate::argument_type,bool>::argument_type argument_type;
  typedef typename std::unary_function<typename Predicate::argument_type,bool>::result_type result_type;
  unary_and (const Predicate& pred, const Predicate2& pred2)
    : Pred(pred), Pred2(pred2) {}
  bool operator() (const argument_type& x) const
  { return Pred(x) && Pred2(x); }
};

template <class Predicate, class Predicate2>
inline unary_and<Predicate, Predicate2> and1(const Predicate& pred, const Predicate2& pred2)
{
  return unary_and<Predicate, Predicate2>(pred, pred2);
}


template <class Predicate, class Predicate2>
class unary_or : public std::unary_function<typename Predicate::argument_type, bool>
{
protected:
  Predicate Pred;
  Predicate2 Pred2;
public:
  typedef typename std::unary_function<typename Predicate::argument_type,bool>::argument_type argument_type;
  typedef typename std::unary_function<typename Predicate::argument_type,bool>::result_type result_type;
  unary_or (const Predicate& pred, const Predicate2& pred2)
    : Pred(pred), Pred2(pred2) {}
  bool operator() (const argument_type& x) const
  { return Pred(x) || Pred2(x); }
};

template <class Predicate, class Predicate2>
inline unary_or<Predicate, Predicate2> or1(const Predicate& pred, const Predicate2& pred2)
{
  return unary_or<Predicate, Predicate2>(pred, pred2);
}



template <class _Operation1, class _Operation2>
class unary_compose
  : public std::unary_function<typename _Operation2::argument_type,
                          typename _Operation1::result_type>
{
protected:
  _Operation1 _M_fn1;
  _Operation2 _M_fn2;
public:
  unary_compose(const _Operation1& __x, const _Operation2& __y)
    : _M_fn1(__x), _M_fn2(__y) {}
  typename _Operation1::result_type
  operator()(const typename _Operation2::argument_type& __x) /* const */ {
    return _M_fn1(_M_fn2(__x));
  }
};

template <class _Operation1, class _Operation2>
inline unary_compose<_Operation1,_Operation2>
compose1(const _Operation1& __fn1, const _Operation2& __fn2)
{
  return unary_compose<_Operation1,_Operation2>(__fn1, __fn2);
}


template <class T>
class FDelete : public std::unary_function<T, void> {
public:
  void operator() (T& value)
    {
      delete value;
    }
};

struct FDeleteObject
{
  template<typename T>
  void operator()(T* ptr)
  {
     if( ptr )
       delete ptr;
     ptr = NULL;
  };
};

struct FDeleteSecond
{
  template<typename T>
  void operator()(T& t) const
  {
     if( t.second )
       delete t.second;
     t.second = NULL;
  };
};

#endif
