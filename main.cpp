#include <typeinfo>
#include <type_traits>

//Lambda calculus AST
template<typename Id>
struct Var {};

template<typename X, typename T>
struct Abs {};

template<typename T1, typename T2>
struct App {};

//Helper meta datastructures
template<typename T, typename U>
struct Pair {
	using First = T;
	using Second = U;
};

template<int n>
struct Id {};

template<typename Id>
struct Prime {};

template<typename T, typename U>
struct List {
	using Head = T;
	using Tail = U;
};

struct Nil {};

//Meta constructs
template<bool b, typename True, typename False>
struct If {
	using type = False;
};

template<typename True, typename False>
struct If<true, True, False> {
	using type = True;
};

//Looking up a (key, value) pair in a list
template<typename Id, typename Map>
struct Assoc {};

template<typename Id>
struct Assoc<Id, Nil> {
	using result = Nil;
};

template<typename Id, typename Val, typename Tail>
struct Assoc<Id, List<Pair<Id, Val>, Tail>> {
	using result = Val;
};

template<typename Id, typename Id2, typename Val, typename Tail>
struct Assoc<Id, List<Pair<Id2, Val>, Tail>> {
	using result = typename Assoc<Id, Tail>::result;
};

//Helper for EnsureUnique
template<typename Id, typename Env, typename Env2>
struct Loop {};

//No name collisions
template<typename Id, typename Env>
struct Loop<Id, Nil, Env> {
	using result = Id;
};

//This name was taken, try another name and check for name collisions
template<typename Id, typename Value, typename Rest, typename Env>
struct Loop<Id, List<Pair<Id, Value>, Rest>, Env> {
	using result = typename Loop<
		Prime<Id>,
		Env,
		Env
	>::result;
};

//No name collision so far, continue recursion
template<typename Id1, typename Id2, typename Value, typename Rest, typename Env>
struct Loop<Id1, List<Pair<Id2, Value>, Rest>, Env> {
	using result = typename Loop<
		Id1,
		Rest,
		Env
	>::result;
};

template<typename Id, typename Env>
struct EnsureUnique {
private:
	using Id2 = typename Loop<Id, Env, Env>::result;

public:
	using result =
		typename If<std::is_same<Id, Id2>::value,
			Pair<Id, Env>,
			Pair<Id2, List<Pair<Id, Var<Id>>, Env>>
		>::type;
};

//Evaluating lambda calculus
template<typename Env, typename Term>
struct Eval {};

template<typename Env, typename Id>
struct Eval<Env, Var<Id>> {
private:
	using T = typename Assoc<Id, Env>::result;

	template<typename T>
	struct Identity {
		using result = T;
	};

public:
	//Template instansiation is not lazy, meaning that
	//if we wrote Eval<Env, T>::result we would not be guarenteed
	//that the result type existed.
	//By delaying this instansiation until we know
	//that T and Var<Id> are not equal we know that
	//the result type actually exists
	using result = typename If<
		std::is_same<T, Var<Id>>::value,
		Identity<T>,
		Eval<Env, T>
	>::type::result;
};

template<typename Env, typename X, typename T>
struct Eval<Env, Abs<X, T>> {
private:
	using Unique = typename EnsureUnique<X, Env>::result;
	using X2 = typename Unique::First;
	using Env2 = typename Unique::Second;
public: 
	using result = Abs<
		X2,
		typename Eval<
			List<Pair<X2, Var<X2>>, Env2>,
			T
		>::result
	>;
};

template<typename Env, typename T1, typename T2>
struct EvalApp {
	using result = App<T1, T2>;
};

template<typename Env, typename X, typename T, typename T2>
struct EvalApp<Env, Abs<X, T>, T2> {
private:
	using Unique = typename EnsureUnique<X, Env>::result;
	using X2 = typename Unique::First;
	using Env2 = typename Unique::Second;

public:
	using result = typename Eval<
		List<Pair<X2, T2>, Env2>,
		T
	>::result;
};

template<typename Env, typename T1, typename T2>
struct Eval<Env, App<T1, T2>> : EvalApp<Env, typename Eval<Env, T1>::result, typename Eval<Env, T2>::result> {};

int main(int argc, char* argv[]) {

	//Example:
	//Applying the looping combinator (lam x. x x)(lam x. x x) results
	//in infinite looping and will throw off any compiler!
	using X = Id<0>;
	Eval<
		Nil,
		App<
			Abs<
				X,
				App<Var<X>, Var<X>>
			>,
			Abs<
				X,
				App<Var<X>, Var<X>>
			>
		>
	>::result();

	return 0;
}
