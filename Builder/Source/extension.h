#ifndef EXTENSION
#define EXTENSION

#include <vector>
#include <string>

namespace ext
{
	template <typename T>
	void intersection(std::vector<T> first, std::vector<T> second, std::vector<T>& result)
	{
		std::vector	<T> result{};
		for (int i = 0; i < first.size(); i++)
		{
			for (int j = 0; j < second.size(); j++)
			{
				if (first[i] = second[j])
				{
					result.push_back(first[i]);
				}
			}
		}
	}

	namespace string
	{
		void replace(std::string& value, std::string oldValue, std::string newValue);
	}
}

#endif
