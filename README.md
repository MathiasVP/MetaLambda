MetaLambda
==========

Untyped lambda calculus implementation in compile time C++

# Examples
The syntax is written entirely in C++ types, and thus the syntax is horrible.

## Defining identifiers and variables
Defining the identifier X is done using the templated `Id` struct which has an integer that definesd comparison between identifiers. Thus
```cpp
	using X = Id<0>;
```
Defines the identifier X, which can be used to create variables using the `Var` struct like so
```cpp
	using X = Id<0>;
	using VarX = Var<X>;
```
## The identity combinator
The following function just returns whatever it's given as its argument
```cpp
	using X = Id<0>;
	using Y = Id<1>;
	Eval<
		List<
			Pair<
				Y,
				Var<Y>
			>,
			Nil
		>,
		App<
			Abs<
				X,
				Var<X>
			>,
			Var<Y>
		>
	>::result();
```
## The looping term
The looping term, also called Omega, is an infinite loop that will spit out tons and tons of compiler template-instantiation stack overflows.
```cpp
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
```

# What's missing?
- Implementing helpful error messages during evaluating of expressions using ```static_assert```
- Adopt syntactic sugar notation commonly used when writing in lambda calculus
