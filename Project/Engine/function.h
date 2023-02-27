#pragma once

template<typename InputIter, typename OutputIter>
void myCopy(InputIter begin, InputIter end, OutputIter target)
{
	for (auto Iter{ begin }; Iter != end; ++Iter)
	{
		*target = *iter;
	}
}
