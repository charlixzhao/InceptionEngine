#pragma once

#include <string>
namespace inceptionengine
{

	class PathHelper
	{
	public:

		enum class FileType
		{
			File,
			Directory,
			SoftLink,
			HardLink
		};
		static void Normalize(std::string& path)
		{
			;
		}

		static bool IsAbsolutePath(std::string const& path)
		{
			return true;
		}

		static bool IsFileExist(std::string const& path)
		{
			return true;
		}

		static FileType GetFileType(std::string const& path)
		{
			return FileType::File;
		}

		static std::string GetFileExtension(std::string const& path)
		{
			return "";
		}
	};

}