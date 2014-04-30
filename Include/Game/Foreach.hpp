#ifndef GAME_FOREACH_HPP
#define GAME_FOREACH_HPP

// Preprocessor trick to allow nested loops
#define GAME_PP_CAT_IMPL(a, b) a ## b
#define GAME_PP_CAT(a, b) GAME_PP_CAT_IMPL(a, b)
#define GAME_ID(identifier) GAME_PP_CAT(auroraDetail_, identifier)
#define GAME_LINE_ID(identifier) GAME_PP_CAT(GAME_ID(identifier), __LINE__)


// Macro to emulate C++11 range-based for loop
// Instead of for (decl : range) you write FOREACH(decl, range) as in the following example
//
// std::vector<int> v = ...;
// FOREACH(int& i, v)
// {
//     i += 2;
// }
#define FOREACH(declaration, container)																											\
	if (bool GAME_LINE_ID(broken) = false) {} else																								\
	for (auto GAME_LINE_ID(itr) = (container).begin(); GAME_LINE_ID(itr) != (container).end() && !GAME_LINE_ID(broken); ++GAME_LINE_ID(itr))	\
	if (bool GAME_LINE_ID(passed) = false) {} else																								\
	if (GAME_LINE_ID(broken) = true, false) {} else																								\
	for (declaration = *GAME_LINE_ID(itr); !GAME_LINE_ID(passed); GAME_LINE_ID(passed) = true, GAME_LINE_ID(broken) = false)

#endif // GAME_FOREACH_HPP
