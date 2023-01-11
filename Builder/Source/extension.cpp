#include "extension.h"

namespace ext
{
	namespace string
	{
		void replace(std::string& value, std::string oldValue, std::string newValue)
		{
			std::vector<uint32_t> repIndices{};
			for (uint32_t i = 0; i < value.size(); i++)
			{
				if (value[i] == oldValue[0])
				{
					for (uint32_t j = i; i - j < oldValue.size(); i++)
					{
						if (value[i] != oldValue[i - j]) break;
						if (i - j == oldValue.size() - 1) repIndices.push_back(j);
					}
				}
			}

			std::string result = std::string(value.size() + repIndices.size() * (newValue.size() - oldValue.size()), '-');

			uint32_t r{};
			for (uint32_t i = 0, j = 0; i < value.size(); i++)
			{
				if (i == repIndices[r])
				{
					for (uint32_t k = 0; k < newValue.size(); k++, j++)
						result[j] = newValue[k];
					i += oldValue.size() - 1;
					r++;
				}
				else
				{
					result[j] = value[i];
					j++;
				}
			}

			value = result;
		}
	}
}

